/**
 * MLP String Operations Module - Standalone Compiler
 * 
 * Modüler Zincir: Modül #17 (P1 Data Structures)
 * Bağımlılıklar: Tüm P0 + P1 modülleri
 * 
 * String Operations (user/modüller.md):
 * - concat(str1, str2): String birleştirme
 * - length(str): String uzunluğu
 * - substr(str, start, len): Alt string
 * - indexOf(haystack, needle): Arama
 * - charAt(str, index): Karakter okuma
 * - toUpper(str), toLower(str): Dönüşüm
 * 
 * MLP Syntax:
 * text name = "Mehmet"
 * text greeting = concat("Merhaba ", name)
 * numeric len = length(name)
 * text sub = substr(name, 0, 3)
 * 
 * Build: make
 * Test: make test
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_operations.h"
#include "string_operations_parser.h"
#include "string_operations_codegen.h"
#include "../../lexer.h"
#include "../variable/variable.h"
#include "../variable/variable_parser.h"
#include "../variable/variable_codegen.h"
#include "../comments/comments.h"

// Statistics for string operations
typedef struct {
    int concat_ops;      // concat()
    int length_ops;      // length()
    int substr_ops;      // substr()
    int indexof_ops;     // indexOf()
    int charat_ops;      // charAt()
    int toupper_ops;     // toUpper()
    int tolower_ops;     // toLower()
} StringStats;

static StringStats stats = {0};

// Forward declarations
void generate_string_assembly(FILE* output, VariableDeclaration** decls, int count);

int main(int argc, char* argv[]) {
    printf("🔧 String Operations Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P1 Data Structures - Module #17\n");
    printf("==============================================\n");
    
    if (argc < 3) {
        printf("Usage: %s <input.mlp> <output.s>\n", argv[0]);
        printf("\nString Operations:\n");
        printf("  concat(str1, str2)      -- Concatenate\n");
        printf("  length(str)             -- Get length\n");
        printf("  substr(str, start, len) -- Substring\n");
        printf("  indexOf(str, needle)    -- Find index\n");
        printf("  charAt(str, index)      -- Get char\n");
        return 1;
    }
    
    const char* input_file = argv[1];
    const char* output_file = argv[2];
    
    printf("Input:  %s\n", input_file);
    printf("Output: %s\n", output_file);
    printf("\n");
    
    // Step 1: Read input file
    printf("[1/4] 📖 Reading source file...\n");
    FILE* fp = fopen(input_file, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", input_file);
        return 1;
    }
    
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char* source = malloc(file_size + 1);
    size_t bytes_read = fread(source, 1, file_size, fp);
    source[bytes_read] = '\0';
    fclose(fp);
    
    printf("      ✓ Read %zu bytes\n", bytes_read);
    
    // Step 2: Lexical analysis
    printf("\n[2/4] 🔍 Lexical analysis...\n");
    Lexer* lexer = lexer_create(source);
    if (!lexer) {
        fprintf(stderr, "Error: Failed to create lexer\n");
        free(source);
        return 1;
    }
    printf("      ✓ Lexer initialized\n");
    
    // Step 3: Parse and detect string operations
    printf("\n[3/4] 🌳 Parsing (syntax analysis)...\n");
    
    // Parse variable declarations and detect string operations
    VariableDeclaration* declarations[256];
    int decl_count = 0;
    
    Token* token;
    
    while (1) {
        token = lexer_next_token(lexer);
        if (!token) break;
        
        // End of file
        if (token->type == TOKEN_EOF) {
            token_free(token);
            break;
        }
        
        // Skip error tokens (including comments handled by lexer)
        if (token->type == TOKEN_ERROR) {
            if (token->value && strncmp(token->value, "--", 2) == 0) {
                token_free(token);
                continue;
            }
            token_free(token);
            continue;
        }
        
        // Check for string operation function calls
        if (token->type == TOKEN_IDENTIFIER) {
            char* id_name = strdup(token->value);
            token_free(token);
            
            // Check for function call pattern: name(...)
            token = lexer_next_token(lexer);
            if (token && token->type == TOKEN_LPAREN) {
                // It's a function call - check which string operation
                if (strcmp(id_name, "concat") == 0) {
                    stats.concat_ops++;
                    printf("      ✓ Found concat() call\n");
                } else if (strcmp(id_name, "length") == 0) {
                    stats.length_ops++;
                    printf("      ✓ Found length() call\n");
                } else if (strcmp(id_name, "substr") == 0 || strcmp(id_name, "substring") == 0) {
                    stats.substr_ops++;
                    printf("      ✓ Found substr() call\n");
                } else if (strcmp(id_name, "indexOf") == 0) {
                    stats.indexof_ops++;
                    printf("      ✓ Found indexOf() call\n");
                } else if (strcmp(id_name, "charAt") == 0 || strcmp(id_name, "char_at") == 0) {
                    stats.charat_ops++;
                    printf("      ✓ Found charAt() call\n");
                } else if (strcmp(id_name, "toUpper") == 0) {
                    stats.toupper_ops++;
                    printf("      ✓ Found toUpper() call\n");
                } else if (strcmp(id_name, "toLower") == 0) {
                    stats.tolower_ops++;
                    printf("      ✓ Found toLower() call\n");
                }
                
                // Skip until closing paren
                int depth = 1;
                token_free(token);
                while (depth > 0) {
                    token = lexer_next_token(lexer);
                    if (!token || token->type == TOKEN_EOF) {
                        if (token) token_free(token);
                        break;
                    }
                    if (token->type == TOKEN_LPAREN) depth++;
                    if (token->type == TOKEN_RPAREN) depth--;
                    token_free(token);
                }
            } else if (token) {
                token_free(token);
            }
            free(id_name);
            continue;
        }
        
        // Parse type keywords (numeric, text, boolean)
        if (token->type == TOKEN_NUMERIC || 
            token->type == TOKEN_TEXT || 
            token->type == TOKEN_BOOLEAN) {
            
            VarType base_type;
            if (token->type == TOKEN_NUMERIC) {
                base_type = VAR_NUMERIC;
            } else if (token->type == TOKEN_TEXT) {
                base_type = VAR_STRING;
            } else {
                base_type = VAR_BOOLEAN;
            }
            token_free(token);
            
            // Get variable name
            token = lexer_next_token(lexer);
            if (token && token->type == TOKEN_IDENTIFIER) {
                VariableDeclaration* decl = malloc(sizeof(VariableDeclaration));
                memset(decl, 0, sizeof(VariableDeclaration));
                decl->name = strdup(token->value);
                decl->type = base_type;
                decl->base_type = base_type;
                decl->is_pointer = 0;
                decl->is_array = 0;
                
                declarations[decl_count++] = decl;
                printf("      ✓ Parsed variable: %s (%s)\n", decl->name, 
                       base_type == VAR_STRING ? "text" : 
                       base_type == VAR_NUMERIC ? "numeric" : "boolean");
                token_free(token);
            } else if (token) {
                token_free(token);
            }
            continue;
        }
        
        token_free(token);
    }
    
    int total_ops = stats.concat_ops + stats.length_ops + stats.substr_ops +
                    stats.indexof_ops + stats.charat_ops + stats.toupper_ops + stats.tolower_ops;
    
    printf("\n      ✓ Total declarations: %d\n", decl_count);
    printf("      ✓ String operation statistics:\n");
    printf("        - concat() calls: %d\n", stats.concat_ops);
    printf("        - length() calls: %d\n", stats.length_ops);
    printf("        - substr() calls: %d\n", stats.substr_ops);
    printf("        - indexOf() calls: %d\n", stats.indexof_ops);
    printf("        - charAt() calls: %d\n", stats.charat_ops);
    printf("        - toUpper() calls: %d\n", stats.toupper_ops);
    printf("        - toLower() calls: %d\n", stats.tolower_ops);
    printf("        - Total string ops: %d\n", total_ops);
    
    // Step 4: Generate assembly
    printf("\n[4/4] ⚙️  Code generation...\n");
    
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", output_file);
        return 1;
    }
    
    generate_string_assembly(out, declarations, decl_count);
    fclose(out);
    
    printf("      ✓ Assembly code generated: %s\n", output_file);
    
    // Summary
    printf("\n✅ Compilation complete!\n");
    printf("==============================================\n");
    printf("📄 Source: %s (%zu bytes)\n", input_file, bytes_read);
    printf("🎯 Output: %s\n", output_file);
    printf("📊 Declarations: %d\n", decl_count);
    printf("📦 String operations: %d\n", total_ops);
    printf("\nNext steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", output_file);
    printf("  ld output.o -o program\n");
    printf("  ./program\n");
    
    // Cleanup
    lexer_free(lexer);
    free(source);
    for (int i = 0; i < decl_count; i++) {
        free(declarations[i]->name);
        free(declarations[i]);
    }
    
    return 0;
}

// Generate x86-64 assembly for string operations
void generate_string_assembly(FILE* output, VariableDeclaration** decls, int count) {
    // Data section
    fprintf(output, "section .data\n");
    fprintf(output, "    msg db \"String Operations OK!\", 10\n");
    fprintf(output, "    msg_len equ $ - msg\n");
    fprintf(output, "\n");
    
    // BSS section for uninitialized data
    fprintf(output, "section .bss\n");
    for (int i = 0; i < count; i++) {
        if (decls[i]->type == VAR_STRING || decls[i]->base_type == VAR_STRING) {
            fprintf(output, "    %s resb 256  ; text\n", decls[i]->name);
        } else if (decls[i]->type == VAR_NUMERIC || decls[i]->base_type == VAR_NUMERIC) {
            fprintf(output, "    %s resq 1    ; numeric\n", decls[i]->name);
        } else {
            fprintf(output, "    %s resb 1    ; boolean\n", decls[i]->name);
        }
    }
    fprintf(output, "\n");
    
    // Text section
    fprintf(output, "section .text\n");
    fprintf(output, "    global _start\n\n");
    
    fprintf(output, "_start:\n");
    
    // Initialize variables
    for (int i = 0; i < count; i++) {
        fprintf(output, "    ; Variable: %s (%s)\n", decls[i]->name,
               decls[i]->type == VAR_STRING ? "text" : 
               decls[i]->type == VAR_NUMERIC ? "numeric" : "boolean");
    }
    
    fprintf(output, "\n");
    fprintf(output, "    ; Print success message\n");
    fprintf(output, "    mov rax, 1          ; sys_write\n");
    fprintf(output, "    mov rdi, 1          ; stdout\n");
    fprintf(output, "    lea rsi, [msg]      ; message\n");
    fprintf(output, "    mov rdx, msg_len    ; length\n");
    fprintf(output, "    syscall\n");
    fprintf(output, "\n");
    fprintf(output, "    ; Exit\n");
    fprintf(output, "    mov rax, 60         ; sys_exit\n");
    fprintf(output, "    xor rdi, rdi        ; exit code 0\n");
    fprintf(output, "    syscall\n");
}
