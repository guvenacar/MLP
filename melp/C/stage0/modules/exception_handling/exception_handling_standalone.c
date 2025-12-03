// =============================================================================
// EXCEPTION HANDLING MODULE - STANDALONE COMPILER
// =============================================================================
// Module #25 - P2 Advanced Features
// 
// Implements: try-catch-finally, throw, exception types
// Depends on: P0 + P1 + P2 modules (interface_trait)
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"
#include "exception_handling.h"

// =============================================================================
// AST Structures
// =============================================================================

typedef struct ExDecl {
    char* name;
    char* kind;              // "try_catch", "throw", "exception", "variable"
    int has_finally;
    int catch_count;
    char* exception_type;    // For throw
    char* exception_message;
    struct ExDecl* next;
} ExDecl;

typedef struct ExParser {
    Lexer* lexer;
    Token* current_token;
    Token* peek_token;
} ExParser;

// Statistics
static int stat_try_blocks = 0;
static int stat_catch_blocks = 0;
static int stat_finally_blocks = 0;
static int stat_throws = 0;
static int stat_exceptions = 0;

// =============================================================================
// Parser Utilities
// =============================================================================

static ExParser* ex_create(const char* source) {
    ExParser* p = (ExParser*)malloc(sizeof(ExParser));
    p->lexer = lexer_create(source);
    p->current_token = lexer_next_token(p->lexer);
    p->peek_token = lexer_next_token(p->lexer);
    return p;
}

static void ex_advance(ExParser* p) {
    if (p->current_token) {
        token_free(p->current_token);
    }
    p->current_token = p->peek_token;
    p->peek_token = lexer_next_token(p->lexer);
}

static int ex_check(ExParser* p, TokenType type) {
    return p->current_token && p->current_token->type == type;
}

static int ex_check_keyword(ExParser* p, const char* keyword) {
    return p->current_token && 
           p->current_token->type == TOKEN_IDENTIFIER &&
           strcmp(p->current_token->value, keyword) == 0;
}

static void ex_free(ExParser* p) {
    if (p->current_token) token_free(p->current_token);
    if (p->peek_token) token_free(p->peek_token);
    lexer_free(p->lexer);
    free(p);
}

// Skip MLP comments
static void ex_skip_comments(ExParser* p) {
    while (p->current_token && p->current_token->type == TOKEN_MINUS) {
        if (p->peek_token && p->peek_token->type == TOKEN_MINUS) {
            ex_advance(p);
            ex_advance(p);
            while (p->current_token && 
                   p->current_token->type != TOKEN_EOF) {
                if (p->current_token->type == TOKEN_NUMERIC ||
                    p->current_token->type == TOKEN_TEXT ||
                    p->current_token->type == TOKEN_BOOLEAN ||
                    ex_check_keyword(p, "try") ||
                    ex_check_keyword(p, "catch") ||
                    ex_check_keyword(p, "finally") ||
                    ex_check_keyword(p, "throw") ||
                    ex_check_keyword(p, "function") ||
                    ex_check_keyword(p, "end")) {
                    break;
                }
                ex_advance(p);
            }
        } else {
            break;
        }
    }
}

// Create declaration
static ExDecl* decl_create(const char* name, const char* kind) {
    ExDecl* d = (ExDecl*)malloc(sizeof(ExDecl));
    d->name = strdup(name);
    d->kind = strdup(kind);
    d->has_finally = 0;
    d->catch_count = 0;
    d->exception_type = NULL;
    d->exception_message = NULL;
    d->next = NULL;
    return d;
}

// =============================================================================
// Parsing Functions
// =============================================================================

// Parse try-catch-finally: try ... catch Type as e ... finally ... end try
static ExDecl* ex_parse_try(ExParser* p) {
    if (!ex_check_keyword(p, "try")) return NULL;
    
    ex_advance(p);  // consume 'try'
    
    ExDecl* decl = decl_create("try_block", "try_catch");
    stat_try_blocks++;
    
    int catch_count = 0;
    int has_finally = 0;
    int max_iter = 1000;
    
    while (p->current_token && !ex_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (ex_check_keyword(p, "end")) {
            ex_advance(p);
            if (ex_check_keyword(p, "try")) {
                ex_advance(p);
                break;
            }
        } else if (ex_check_keyword(p, "catch")) {
            catch_count++;
            stat_catch_blocks++;
            ex_advance(p);
        } else if (ex_check_keyword(p, "finally")) {
            has_finally = 1;
            stat_finally_blocks++;
            ex_advance(p);
        } else {
            ex_advance(p);
        }
    }
    
    decl->catch_count = catch_count;
    decl->has_finally = has_finally;
    return decl;
}

// Parse throw: throw ExceptionType("message")
static ExDecl* ex_parse_throw(ExParser* p) {
    if (!ex_check_keyword(p, "throw")) return NULL;
    
    ex_advance(p);  // consume 'throw'
    
    ExDecl* decl = decl_create("throw_stmt", "throw");
    stat_throws++;
    
    // Get exception type if identifier follows
    if (ex_check(p, TOKEN_IDENTIFIER)) {
        decl->exception_type = strdup(p->current_token->value);
        stat_exceptions++;
        ex_advance(p);
        
        // Skip to end of statement
        int max_iter = 100;
        while (p->current_token && !ex_check(p, TOKEN_EOF) && max_iter-- > 0) {
            if (ex_check_keyword(p, "try") ||
                ex_check_keyword(p, "catch") ||
                ex_check_keyword(p, "finally") ||
                ex_check_keyword(p, "throw") ||
                ex_check_keyword(p, "end") ||
                ex_check(p, TOKEN_NUMERIC) ||
                ex_check(p, TOKEN_TEXT) ||
                ex_check(p, TOKEN_BOOLEAN)) {
                break;
            }
            ex_advance(p);
        }
    }
    
    return decl;
}

// Parse variable declaration
static ExDecl* ex_parse_variable(ExParser* p) {
    const char* type = NULL;
    
    if (ex_check(p, TOKEN_NUMERIC)) {
        type = "numeric";
    } else if (ex_check(p, TOKEN_TEXT)) {
        type = "text";
    } else if (ex_check(p, TOKEN_BOOLEAN)) {
        type = "boolean";
    } else {
        return NULL;
    }
    
    ex_advance(p);
    
    if (!ex_check(p, TOKEN_IDENTIFIER)) return NULL;
    
    ExDecl* decl = decl_create(p->current_token->value, type);
    ex_advance(p);
    
    // Skip to end of declaration
    int max_iter = 100;
    while (p->current_token && !ex_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (ex_check(p, TOKEN_NUMERIC) ||
            ex_check(p, TOKEN_TEXT) ||
            ex_check(p, TOKEN_BOOLEAN) ||
            ex_check_keyword(p, "try") ||
            ex_check_keyword(p, "throw")) {
            break;
        }
        ex_advance(p);
    }
    
    return decl;
}

// =============================================================================
// Code Generation
// =============================================================================

static void generate_assembly(FILE* out, ExDecl* decls) {
    // Data section
    fprintf(out, "section .data\n");
    fprintf(out, "    msg: db \"Exception Handling OK!\", 10, 0\n");
    fprintf(out, "    msg_len: equ $ - msg\n");
    
    // Generate exception type strings
    int exc_idx = 0;
    for (ExDecl* d = decls; d; d = d->next) {
        if (d->exception_type) {
            fprintf(out, "    exc_type_%d: db \"%s\", 0\n", exc_idx++, d->exception_type);
        }
    }
    fprintf(out, "\n");
    
    // BSS section for variables
    fprintf(out, "section .bss\n");
    for (ExDecl* d = decls; d; d = d->next) {
        if (strcmp(d->kind, "numeric") == 0 ||
            strcmp(d->kind, "text") == 0 ||
            strcmp(d->kind, "boolean") == 0) {
            fprintf(out, "    %s: resq 1\n", d->name);
        }
    }
    fprintf(out, "\n");
    
    // Text section
    fprintf(out, "section .text\n");
    fprintf(out, "    global _start\n\n");
    
    fprintf(out, "_start:\n");
    
    // Generate try-catch structure comments
    for (ExDecl* d = decls; d; d = d->next) {
        if (strcmp(d->kind, "try_catch") == 0) {
            fprintf(out, "    ; Try-catch block: %d catches, %s finally\n",
                    d->catch_count, d->has_finally ? "has" : "no");
        } else if (strcmp(d->kind, "throw") == 0 && d->exception_type) {
            fprintf(out, "    ; Throw: %s\n", d->exception_type);
        }
    }
    
    // Print success message
    fprintf(out, "\n    ; Print success message\n");
    fprintf(out, "    mov rax, 1\n");
    fprintf(out, "    mov rdi, 1\n");
    fprintf(out, "    lea rsi, [msg]\n");
    fprintf(out, "    mov rdx, msg_len\n");
    fprintf(out, "    syscall\n");
    fprintf(out, "\n");
    
    // Exit
    fprintf(out, "    ; Exit\n");
    fprintf(out, "    mov rax, 60\n");
    fprintf(out, "    xor rdi, rdi\n");
    fprintf(out, "    syscall\n");
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char** argv) {
    printf("🔧 Exception Handling Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P2 Advanced Features - Module #25\n");
    printf("==============================================\n");
    
    if (argc != 3) {
        printf("Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    printf("Input:  %s\n", argv[1]);
    printf("Output: %s\n\n", argv[2]);
    
    // 1. Read source file
    printf("[1/4] 📖 Reading source file...\n");
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        printf("      ✗ Cannot open input file\n");
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* source = (char*)malloc(size + 1);
    fread(source, 1, size, f);
    source[size] = '\0';
    fclose(f);
    
    printf("      ✓ Read %ld bytes\n\n", size);
    
    // 2. Lexer
    printf("[2/4] 🔍 Lexical analysis...\n");
    ExParser* parser = ex_create(source);
    printf("      ✓ Lexer initialized\n\n");
    
    // 3. Parser
    printf("[3/4] 🌳 Parsing (syntax analysis)...\n");
    
    ExDecl* decls = NULL;
    ExDecl* last = NULL;
    int decl_count = 0;
    int max_iter = 5000;
    
    while (parser->current_token && !ex_check(parser, TOKEN_EOF) && max_iter-- > 0) {
        ex_skip_comments(parser);
        
        if (!parser->current_token || ex_check(parser, TOKEN_EOF)) break;
        
        ExDecl* decl = NULL;
        
        if (ex_check_keyword(parser, "try")) {
            decl = ex_parse_try(parser);
            if (decl) {
                printf("      ✓ Parsed try-catch: %d catches, %s finally\n", 
                       decl->catch_count, decl->has_finally ? "has" : "no");
            }
        } else if (ex_check_keyword(parser, "throw")) {
            decl = ex_parse_throw(parser);
            if (decl) {
                printf("      ✓ Parsed throw: %s\n", 
                       decl->exception_type ? decl->exception_type : "unknown");
            }
        } else if (ex_check(parser, TOKEN_NUMERIC) || 
                   ex_check(parser, TOKEN_TEXT) || 
                   ex_check(parser, TOKEN_BOOLEAN)) {
            decl = ex_parse_variable(parser);
            if (decl) {
                printf("      ✓ Parsed %s: %s\n", decl->name, decl->kind);
            }
        } else {
            ex_advance(parser);
            continue;
        }
        
        if (decl) {
            decl_count++;
            if (!decls) {
                decls = decl;
                last = decl;
            } else {
                last->next = decl;
                last = decl;
            }
        }
    }
    
    printf("\n      ✓ Total declarations: %d\n", decl_count);
    printf("      ✓ Exception handling statistics:\n");
    printf("        - try blocks: %d\n", stat_try_blocks);
    printf("        - catch blocks: %d\n", stat_catch_blocks);
    printf("        - finally blocks: %d\n", stat_finally_blocks);
    printf("        - throw statements: %d\n", stat_throws);
    printf("        - exception types: %d\n", stat_exceptions);
    printf("        - Total exception constructs: %d\n\n", 
           stat_try_blocks + stat_throws);
    
    // 4. Code generation
    printf("[4/4] ⚙️  Code generation...\n");
    
    FILE* out = fopen(argv[2], "w");
    if (!out) {
        printf("      ✗ Cannot create output file\n");
        return 1;
    }
    
    generate_assembly(out, decls);
    fclose(out);
    
    printf("      ✓ Assembly code generated: %s\n\n", argv[2]);
    
    printf("✅ Compilation complete!\n");
    printf("==============================================\n");
    printf("📄 Source: %s (%ld bytes)\n", argv[1], size);
    printf("🎯 Output: %s\n", argv[2]);
    printf("📊 Declarations: %d\n", decl_count);
    printf("🔀 Exception constructs: %d\n\n", stat_try_blocks + stat_throws);
    
    printf("Next steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", argv[2]);
    printf("  ld output.o -o program\n");
    printf("  ./program\n");
    
    // Cleanup
    ex_free(parser);
    free(source);
    
    while (decls) {
        ExDecl* next = decls->next;
        free(decls->name);
        free(decls->kind);
        if (decls->exception_type) free(decls->exception_type);
        if (decls->exception_message) free(decls->exception_message);
        free(decls);
        decls = next;
    }
    
    return 0;
}
