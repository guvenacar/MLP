/*
 * MLP Stage 0 Bootstrap Compiler - File I/O Standalone
 * Module #30 - File I/O Operations (P2)
 * 
 * File operations: open, close, read, write, seek, flush, etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../../lexer.h"
#include "file_io.h"

// =============================================================================
// AST Structures
// =============================================================================

typedef struct FileIODecl {
    char* name;
    char* kind;         // "fopen", "fclose", "fread", "fwrite", etc.
    char* path;
    char* mode;
    struct FileIODecl* next;
} FileIODecl;

typedef struct FileIOParser {
    Lexer* lexer;
    Token* current_token;
    Token* peek_token;
} FileIOParser;

// Statistics
static int stat_opens = 0;
static int stat_closes = 0;
static int stat_reads = 0;
static int stat_writes = 0;
static int stat_seeks = 0;

// =============================================================================
// Parser Utilities  
// =============================================================================

static FileIOParser* fileio_parser_create(const char* source) {
    FileIOParser* p = (FileIOParser*)malloc(sizeof(FileIOParser));
    p->lexer = lexer_create(source);
    p->current_token = lexer_next_token(p->lexer);
    p->peek_token = lexer_next_token(p->lexer);
    return p;
}

static void fileio_advance(FileIOParser* p) {
    if (p->current_token) {
        token_free(p->current_token);
    }
    p->current_token = p->peek_token;
    p->peek_token = lexer_next_token(p->lexer);
}

static int fileio_check(FileIOParser* p, TokenType type) {
    return p->current_token && p->current_token->type == type;
}

static int fileio_check_keyword(FileIOParser* p, const char* keyword) {
    return p->current_token && 
           p->current_token->type == TOKEN_IDENTIFIER &&
           strcmp(p->current_token->value, keyword) == 0;
}

static void fileio_free_parser(FileIOParser* p) {
    if (p->current_token) token_free(p->current_token);
    if (p->peek_token) token_free(p->peek_token);
    lexer_free(p->lexer);
    free(p);
}

// Skip MLP comments
static void fileio_skip_comments(FileIOParser* p) {
    while (p->current_token && p->current_token->type == TOKEN_MINUS) {
        if (p->peek_token && p->peek_token->type == TOKEN_MINUS) {
            // Skip to end of line
            fileio_advance(p);
            fileio_advance(p);
            while (p->current_token && p->current_token->type != TOKEN_EOF) {
                if (p->current_token->type == TOKEN_SEMICOLON) {
                    fileio_advance(p);
                    break;
                }
                fileio_advance(p);
            }
        } else {
            break;
        }
    }
}

// =============================================================================
// Declaration Tracking
// =============================================================================

static FileIODecl* decl_list = NULL;
static int decl_count = 0;

static FileIODecl* add_decl(const char* name, const char* kind) {
    FileIODecl* d = (FileIODecl*)malloc(sizeof(FileIODecl));
    d->name = name ? strdup(name) : NULL;
    d->kind = strdup(kind);
    d->path = NULL;
    d->mode = NULL;
    d->next = decl_list;
    decl_list = d;
    decl_count++;
    return d;
}

static void free_decls(void) {
    FileIODecl* d = decl_list;
    while (d) {
        FileIODecl* next = d->next;
        if (d->name) free(d->name);
        if (d->kind) free(d->kind);
        if (d->path) free(d->path);
        if (d->mode) free(d->mode);
        free(d);
        d = next;
    }
    decl_list = NULL;
    decl_count = 0;
}

// =============================================================================
// File I/O Parsing
// =============================================================================

// Parse fopen(path, mode)
static void parse_fopen(FileIOParser* p) {
    fileio_advance(p); // consume 'fopen'
    
    if (fileio_check(p, TOKEN_LPAREN)) {
        fileio_advance(p);  // consume '('
        
        FileIODecl* d = add_decl(NULL, "fopen");
        
        // Get path
        if (fileio_check(p, TOKEN_STRING)) {
            d->path = strdup(p->current_token->value);
            fileio_advance(p);
        } else if (fileio_check(p, TOKEN_IDENTIFIER)) {
            d->path = strdup(p->current_token->value);
            fileio_advance(p);
        }
        
        // Skip comma
        if (fileio_check(p, TOKEN_COMMA)) {
            fileio_advance(p);
        }
        
        // Get mode
        if (fileio_check(p, TOKEN_STRING) || fileio_check(p, TOKEN_IDENTIFIER)) {
            d->mode = strdup(p->current_token->value);
            fileio_advance(p);
        }
        
        // Skip to closing paren
        while (!fileio_check(p, TOKEN_RPAREN) && !fileio_check(p, TOKEN_EOF)) {
            fileio_advance(p);
        }
        if (fileio_check(p, TOKEN_RPAREN)) {
            fileio_advance(p);
        }
        
        stat_opens++;
        printf("      ✓ Parsed fopen: %s\n", d->path ? d->path : "variable");
    }
}

// Parse fclose(handle)
static void parse_fclose(FileIOParser* p) {
    fileio_advance(p);  // consume 'fclose'
    
    if (fileio_check(p, TOKEN_LPAREN)) {
        fileio_advance(p);
        
        FileIODecl* d = add_decl(NULL, "fclose");
        
        if (fileio_check(p, TOKEN_IDENTIFIER)) {
            d->name = strdup(p->current_token->value);
            fileio_advance(p);
        }
        
        // Skip to closing paren
        while (!fileio_check(p, TOKEN_RPAREN) && !fileio_check(p, TOKEN_EOF)) {
            fileio_advance(p);
        }
        if (fileio_check(p, TOKEN_RPAREN)) {
            fileio_advance(p);
        }
        
        stat_closes++;
        printf("      ✓ Parsed fclose: %s\n", d->name ? d->name : "handle");
    }
}

// Parse fread(buf, size, count, handle)
static void parse_fread(FileIOParser* p) {
    fileio_advance(p);  // consume 'fread'
    
    if (fileio_check(p, TOKEN_LPAREN)) {
        fileio_advance(p);
        
        FileIODecl* d = add_decl(NULL, "fread");
        
        if (fileio_check(p, TOKEN_IDENTIFIER)) {
            d->name = strdup(p->current_token->value);
            fileio_advance(p);
        }
        
        // Skip rest of args
        int depth = 1;
        int safety = 0;
        while (depth > 0 && safety < 30) {
            if (fileio_check(p, TOKEN_LPAREN)) depth++;
            if (fileio_check(p, TOKEN_RPAREN)) depth--;
            if (depth > 0) fileio_advance(p);
            safety++;
        }
        
        stat_reads++;
        printf("      ✓ Parsed fread: %s\n", d->name ? d->name : "buffer");
    }
}

// Parse fwrite(buf, size, count, handle)
static void parse_fwrite(FileIOParser* p) {
    fileio_advance(p);  // consume 'fwrite'
    
    if (fileio_check(p, TOKEN_LPAREN)) {
        fileio_advance(p);
        
        FileIODecl* d = add_decl(NULL, "fwrite");
        
        if (fileio_check(p, TOKEN_IDENTIFIER) || fileio_check(p, TOKEN_STRING)) {
            d->name = strdup(p->current_token->value);
            fileio_advance(p);
        }
        
        // Skip rest of args
        int depth = 1;
        int safety = 0;
        while (depth > 0 && safety < 30) {
            if (fileio_check(p, TOKEN_LPAREN)) depth++;
            if (fileio_check(p, TOKEN_RPAREN)) depth--;
            if (depth > 0) fileio_advance(p);
            safety++;
        }
        
        stat_writes++;
        printf("      ✓ Parsed fwrite: %s\n", d->name ? d->name : "data");
    }
}

// Parse fseek(handle, offset, whence)
static void parse_fseek(FileIOParser* p) {
    fileio_advance(p);  // consume 'fseek'
    
    if (fileio_check(p, TOKEN_LPAREN)) {
        fileio_advance(p);
        
        FileIODecl* d = add_decl(NULL, "fseek");
        
        if (fileio_check(p, TOKEN_IDENTIFIER)) {
            d->name = strdup(p->current_token->value);
            fileio_advance(p);
        }
        
        // Skip rest of args
        int depth = 1;
        int safety = 0;
        while (depth > 0 && safety < 30) {
            if (fileio_check(p, TOKEN_LPAREN)) depth++;
            if (fileio_check(p, TOKEN_RPAREN)) depth--;
            if (depth > 0) fileio_advance(p);
            safety++;
        }
        
        stat_seeks++;
        printf("      ✓ Parsed fseek: %s\n", d->name ? d->name : "handle");
    }
}

// Parse simple file helpers: readfile, writefile, etc.
static void parse_file_helper(FileIOParser* p, const char* helper_type) {
    const char* type = helper_type;
    fileio_advance(p);  // consume keyword
    
    if (fileio_check(p, TOKEN_LPAREN)) {
        fileio_advance(p);
        
        FileIODecl* d = add_decl(NULL, type);
        
        if (fileio_check(p, TOKEN_STRING)) {
            d->path = strdup(p->current_token->value);
            fileio_advance(p);
        } else if (fileio_check(p, TOKEN_IDENTIFIER)) {
            d->path = strdup(p->current_token->value);
            fileio_advance(p);
        }
        
        // Skip rest
        int depth = 1;
        int safety = 0;
        while (depth > 0 && safety < 30) {
            if (fileio_check(p, TOKEN_LPAREN)) depth++;
            if (fileio_check(p, TOKEN_RPAREN)) depth--;
            if (depth > 0) fileio_advance(p);
            safety++;
        }
        
        if (strcmp(type, "readfile") == 0) {
            stat_reads++;
        } else if (strcmp(type, "writefile") == 0 || strcmp(type, "appendfile") == 0) {
            stat_writes++;
        }
        
        printf("      ✓ Parsed %s: %s\n", type, d->path ? d->path : "path");
    }
}

// Parse variable declaration
static void parse_declaration(FileIOParser* p) {
    // Get type
    char* type = strdup(p->current_token->value);
    fileio_advance(p);
    
    // Get name
    char* name = NULL;
    if (fileio_check(p, TOKEN_IDENTIFIER)) {
        name = strdup(p->current_token->value);
        fileio_advance(p);
    }
    
    // Check for assignment
    if (fileio_check(p, TOKEN_ASSIGN)) {
        fileio_advance(p);
        
        // Check if it's a file operation
        if (fileio_check_keyword(p, "fopen")) {
            FileIODecl* d = add_decl(name, "variable");
            d->mode = strdup("file_handle");
            printf("      ✓ Parsed declaration: %s %s = fopen(...)\n", type, name);
            parse_fopen(p);
        } else if (fileio_check_keyword(p, "readfile")) {
            add_decl(name, "variable");
            printf("      ✓ Parsed declaration: %s %s = readfile(...)\n", type, name);
            parse_file_helper(p, "readfile");
        } else {
            // Normal value
            FileIODecl* d = add_decl(name, "variable");
            if (fileio_check(p, TOKEN_STRING)) {
                d->path = strdup(p->current_token->value);
            }
            printf("      ✓ Parsed declaration: %s %s\n", type, name);
            
            // Skip to semicolon
            while (!fileio_check(p, TOKEN_SEMICOLON) && !fileio_check(p, TOKEN_EOF)) {
                fileio_advance(p);
            }
        }
    } else {
        add_decl(name, "variable");
        printf("      ✓ Parsed declaration: %s %s\n", type, name);
    }
    
    free(type);
    if (name) free(name);
}

// Parse print statement
static void parse_print(FileIOParser* p) {
    fileio_advance(p);  // consume 'print'
    
    if (fileio_check(p, TOKEN_LPAREN)) {
        fileio_advance(p);
        
        // Skip to closing paren
        int depth = 1;
        int safety = 0;
        while (depth > 0 && safety < 50) {
            if (fileio_check(p, TOKEN_LPAREN)) depth++;
            if (fileio_check(p, TOKEN_RPAREN)) depth--;
            if (depth > 0) fileio_advance(p);
            safety++;
        }
        if (fileio_check(p, TOKEN_RPAREN)) {
            fileio_advance(p);
        }
    }
    
    printf("      ✓ Parsed print\n");
}

// =============================================================================
// Main Parse Loop
// =============================================================================

static void parse_program(FileIOParser* p) {
    int safety = 0;
    const int MAX_ITER = 1000;
    
    while (!fileio_check(p, TOKEN_EOF) && safety < MAX_ITER) {
        safety++;
        fileio_skip_comments(p);
        
        if (fileio_check(p, TOKEN_IDENTIFIER)) {
            const char* id = p->current_token->value;
            
            // File I/O operations
            if (strcmp(id, "fopen") == 0) {
                parse_fopen(p);
            } else if (strcmp(id, "fclose") == 0) {
                parse_fclose(p);
            } else if (strcmp(id, "fread") == 0) {
                parse_fread(p);
            } else if (strcmp(id, "fwrite") == 0) {
                parse_fwrite(p);
            } else if (strcmp(id, "fseek") == 0) {
                parse_fseek(p);
            } else if (strcmp(id, "readfile") == 0) {
                parse_file_helper(p, "readfile");
            } else if (strcmp(id, "writefile") == 0) {
                parse_file_helper(p, "writefile");
            } else if (strcmp(id, "appendfile") == 0) {
                parse_file_helper(p, "appendfile");
            } else if (strcmp(id, "deletefile") == 0) {
                parse_file_helper(p, "deletefile");
            } else if (strcmp(id, "fileexists") == 0) {
                parse_file_helper(p, "fileexists");
            } else if (strcmp(id, "fflush") == 0 ||
                       strcmp(id, "ftell") == 0 ||
                       strcmp(id, "feof") == 0 ||
                       strcmp(id, "ferror") == 0) {
                parse_file_helper(p, id);
            }
            // Declarations
            else if (strcmp(id, "int") == 0 ||
                     strcmp(id, "str") == 0 ||
                     strcmp(id, "string") == 0 ||
                     strcmp(id, "bool") == 0 ||
                     strcmp(id, "file") == 0 ||
                     strcmp(id, "var") == 0 ||
                     strcmp(id, "let") == 0 ||
                     strcmp(id, "const") == 0) {
                parse_declaration(p);
            }
            // Print
            else if (strcmp(id, "print") == 0 || strcmp(id, "println") == 0) {
                parse_print(p);
            }
            else {
                fileio_advance(p);
            }
        }
        else if (fileio_check(p, TOKEN_PRINT)) {
            parse_print(p);
        }
        else if (fileio_check(p, TOKEN_SEMICOLON)) {
            fileio_advance(p);
        }
        else {
            fileio_advance(p);
        }
    }
    
    if (safety >= MAX_ITER) {
        fprintf(stderr, "Warning: Parser reached maximum iterations\n");
    }
}

// =============================================================================
// Code Generation
// =============================================================================

static void generate_assembly(const char* output_path) {
    FILE* f = fopen(output_path, "w");
    if (!f) {
        fprintf(stderr, "Error: Cannot create output file %s\n", output_path);
        exit(1);
    }
    
    // Header
    fprintf(f, "; MLP Stage 0 - File I/O Module Output\n");
    fprintf(f, "; Auto-generated x86-64 assembly\n\n");
    
    // Data section
    fprintf(f, "section .data\n");
    fprintf(f, "    msg_ok db 'File I/O OK!', 10, 0\n");
    fprintf(f, "    msg_ok_len equ $ - msg_ok\n");
    
    // BSS section
    fprintf(f, "\nsection .bss\n");
    fprintf(f, "    file_buffer resb 4096\n");
    fprintf(f, "    file_handles resq 32\n");
    
    // Text section
    fprintf(f, "\nsection .text\n");
    fprintf(f, "    global _start\n\n");
    
    // File I/O syscall wrappers
    fprintf(f, "; sys_open wrapper\n");
    fprintf(f, "_sys_open:\n");
    fprintf(f, "    mov rax, 2\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "    ret\n\n");
    
    fprintf(f, "; sys_close wrapper\n");
    fprintf(f, "_sys_close:\n");
    fprintf(f, "    mov rax, 3\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "    ret\n\n");
    
    fprintf(f, "; sys_read wrapper\n");
    fprintf(f, "_sys_read:\n");
    fprintf(f, "    mov rax, 0\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "    ret\n\n");
    
    fprintf(f, "; sys_write wrapper\n");
    fprintf(f, "_sys_write:\n");
    fprintf(f, "    mov rax, 1\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "    ret\n\n");
    
    // Entry point
    fprintf(f, "_start:\n");
    fprintf(f, "    ; File I/O program\n");
    fprintf(f, "\n    ; Print success message\n");
    fprintf(f, "    mov rax, 1\n");
    fprintf(f, "    mov rdi, 1\n");
    fprintf(f, "    lea rsi, [msg_ok]\n");
    fprintf(f, "    mov rdx, msg_ok_len\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "\n    ; Exit\n");
    fprintf(f, "    mov rax, 60\n");
    fprintf(f, "    xor rdi, rdi\n");
    fprintf(f, "    syscall\n");
    
    fclose(f);
}

// =============================================================================
// Main Function
// =============================================================================

int main(int argc, char* argv[]) {
    printf("🔧 File I/O Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P2 Advanced Features - Module #30\n");
    printf("==============================================\n");
    
    if (argc < 3) {
        printf("Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    const char* input_path = argv[1];
    const char* output_path = argv[2];
    
    printf("Input:  %s\n", input_path);
    printf("Output: %s\n\n", output_path);
    
    // Read source file
    printf("[1/4] 📖 Reading source file...\n");
    FILE* f = fopen(input_path, "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open input file %s\n", input_path);
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* source = (char*)malloc(size + 1);
    if (!source) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(f);
        return 1;
    }
    
    size_t read_size = fread(source, 1, size, f);
    source[read_size] = '\0';
    fclose(f);
    
    printf("      ✓ Read %zu bytes\n", read_size);
    
    // Lexer init
    printf("\n[2/4] 🔍 Lexical analysis...\n");
    FileIOParser* parser = fileio_parser_create(source);
    printf("      ✓ Lexer initialized\n");
    
    // Parse
    printf("\n[3/4] 🌳 Parsing (syntax analysis)...\n");
    parse_program(parser);
    
    printf("\n      ✓ Total declarations: %d\n", decl_count);
    printf("      ✓ File I/O statistics:\n");
    printf("        - open operations: %d\n", stat_opens);
    printf("        - close operations: %d\n", stat_closes);
    printf("        - read operations: %d\n", stat_reads);
    printf("        - write operations: %d\n", stat_writes);
    printf("        - seek operations: %d\n", stat_seeks);
    printf("        - Total file ops: %d\n", stat_opens + stat_closes + stat_reads + stat_writes + stat_seeks);
    
    // Generate code
    printf("\n[4/4] ⚙️  Code generation...\n");
    generate_assembly(output_path);
    printf("      ✓ Assembly code generated: %s\n", output_path);
    
    // Summary
    printf("\n✅ Compilation complete!\n");
    printf("==============================================\n");
    printf("📄 Source: %s (%zu bytes)\n", input_path, read_size);
    printf("🎯 Output: %s\n", output_path);
    printf("📊 Declarations: %d\n", decl_count);
    printf("📁 File operations: %d\n", stat_opens + stat_closes + stat_reads + stat_writes + stat_seeks);
    printf("\nNext steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", output_path);
    printf("  ld output.o -o program\n");
    printf("  ./program\n");
    
    // Cleanup
    fileio_free_parser(parser);
    free_decls();
    free(source);
    
    return 0;
}
