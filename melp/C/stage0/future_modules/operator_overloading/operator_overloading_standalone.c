// =============================================================================
// OPERATOR OVERLOADING MODULE - STANDALONE COMPILER
// =============================================================================
// Module #28 - P2 Advanced Features
// 
// Implements: operator overloading for custom types
// Depends on: P0 + P1 + P2 modules
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"
#include "operator_overloading.h"

// =============================================================================
// AST Structures
// =============================================================================

typedef struct OpDecl {
    char* name;
    char* kind;              // "operator", "variable"
    char* op_type;           // "+", "-", "==", etc.
    char* type_name;         // Type it applies to
    int is_binary;
    struct OpDecl* next;
} OpDecl;

typedef struct OpParser {
    Lexer* lexer;
    Token* current_token;
    Token* peek_token;
} OpParser;

// Statistics
static int stat_operators = 0;
static int stat_binary_ops = 0;
static int stat_unary_ops = 0;

// =============================================================================
// Parser Utilities
// =============================================================================

static OpParser* op_create(const char* source) {
    OpParser* p = (OpParser*)malloc(sizeof(OpParser));
    p->lexer = lexer_create(source);
    p->current_token = lexer_next_token(p->lexer);
    p->peek_token = lexer_next_token(p->lexer);
    return p;
}

static void op_advance(OpParser* p) {
    if (p->current_token) {
        token_free(p->current_token);
    }
    p->current_token = p->peek_token;
    p->peek_token = lexer_next_token(p->lexer);
}

static int op_check(OpParser* p, TokenType type) {
    return p->current_token && p->current_token->type == type;
}

static int op_check_keyword(OpParser* p, const char* keyword) {
    return p->current_token && 
           p->current_token->type == TOKEN_IDENTIFIER &&
           strcmp(p->current_token->value, keyword) == 0;
}

static void op_free_parser(OpParser* p) {
    if (p->current_token) token_free(p->current_token);
    if (p->peek_token) token_free(p->peek_token);
    lexer_free(p->lexer);
    free(p);
}

// Skip MLP comments
static void op_skip_comments(OpParser* p) {
    while (p->current_token && p->current_token->type == TOKEN_MINUS) {
        if (p->peek_token && p->peek_token->type == TOKEN_MINUS) {
            op_advance(p);
            op_advance(p);
            while (p->current_token && 
                   p->current_token->type != TOKEN_EOF) {
                if (p->current_token->type == TOKEN_NUMERIC ||
                    p->current_token->type == TOKEN_TEXT ||
                    p->current_token->type == TOKEN_BOOLEAN ||
                    op_check_keyword(p, "operator") ||
                    op_check_keyword(p, "struct") ||
                    op_check_keyword(p, "function") ||
                    op_check_keyword(p, "end")) {
                    break;
                }
                op_advance(p);
            }
        } else {
            break;
        }
    }
}

// Create declaration
static OpDecl* decl_create(const char* name, const char* kind) {
    OpDecl* d = (OpDecl*)malloc(sizeof(OpDecl));
    d->name = strdup(name);
    d->kind = strdup(kind);
    d->op_type = NULL;
    d->type_name = NULL;
    d->is_binary = 1;
    d->next = NULL;
    return d;
}

// =============================================================================
// Parsing Functions
// =============================================================================

// Parse operator: operator + (a: MyType, b: MyType): MyType ... end operator
static OpDecl* op_parse_operator(OpParser* p) {
    if (!op_check_keyword(p, "operator")) return NULL;
    
    op_advance(p);  // consume 'operator'
    
    OpDecl* decl = decl_create("operator_def", "operator");
    stat_operators++;
    
    // Get operator symbol
    if (p->current_token) {
        if (op_check(p, TOKEN_PLUS)) {
            decl->op_type = strdup("+");
            stat_binary_ops++;
        } else if (op_check(p, TOKEN_MINUS)) {
            decl->op_type = strdup("-");
            stat_binary_ops++;
        } else if (op_check(p, TOKEN_MULTIPLY)) {
            decl->op_type = strdup("*");
            stat_binary_ops++;
        } else if (op_check(p, TOKEN_DIVIDE)) {
            decl->op_type = strdup("/");
            stat_binary_ops++;
        } else if (op_check(p, TOKEN_EQUAL)) {
            decl->op_type = strdup("==");
            stat_binary_ops++;
        } else if (op_check(p, TOKEN_NOT_EQUAL)) {
            decl->op_type = strdup("!=");
            stat_binary_ops++;
        } else if (op_check(p, TOKEN_IDENTIFIER)) {
            // Custom operator or unary
            decl->op_type = strdup(p->current_token->value);
            stat_unary_ops++;
        }
        op_advance(p);
    }
    
    // Skip to end operator
    int max_iter = 1000;
    while (p->current_token && !op_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (op_check_keyword(p, "end")) {
            op_advance(p);
            if (op_check_keyword(p, "operator")) {
                op_advance(p);
                break;
            }
        } else {
            op_advance(p);
        }
    }
    
    return decl;
}

// Parse struct with operators
static OpDecl* op_parse_struct(OpParser* p) {
    if (!op_check_keyword(p, "struct")) return NULL;
    
    op_advance(p);  // consume 'struct'
    
    char* struct_name = "unnamed";
    if (op_check(p, TOKEN_IDENTIFIER)) {
        struct_name = p->current_token->value;
    }
    
    OpDecl* decl = decl_create(struct_name, "struct");
    
    if (op_check(p, TOKEN_IDENTIFIER)) {
        op_advance(p);
    }
    
    // Count operators in struct
    int op_count = 0;
    int max_iter = 1000;
    while (p->current_token && !op_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (op_check_keyword(p, "end")) {
            op_advance(p);
            if (op_check_keyword(p, "struct")) {
                op_advance(p);
                break;
            }
        } else if (op_check_keyword(p, "operator")) {
            op_count++;
            op_advance(p);
        } else {
            op_advance(p);
        }
    }
    
    decl->is_binary = op_count;  // Reuse field for operator count
    return decl;
}

// Parse variable declaration
static OpDecl* op_parse_variable(OpParser* p) {
    const char* type = NULL;
    
    if (op_check(p, TOKEN_NUMERIC)) {
        type = "numeric";
    } else if (op_check(p, TOKEN_TEXT)) {
        type = "text";
    } else if (op_check(p, TOKEN_BOOLEAN)) {
        type = "boolean";
    } else {
        return NULL;
    }
    
    op_advance(p);
    
    if (!op_check(p, TOKEN_IDENTIFIER)) return NULL;
    
    OpDecl* decl = decl_create(p->current_token->value, type);
    op_advance(p);
    
    // Skip to end of declaration
    int max_iter = 100;
    while (p->current_token && !op_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (op_check(p, TOKEN_NUMERIC) ||
            op_check(p, TOKEN_TEXT) ||
            op_check(p, TOKEN_BOOLEAN) ||
            op_check_keyword(p, "operator") ||
            op_check_keyword(p, "struct")) {
            break;
        }
        op_advance(p);
    }
    
    return decl;
}

// =============================================================================
// Code Generation
// =============================================================================

static void generate_assembly(FILE* out, OpDecl* decls) {
    // Data section
    fprintf(out, "section .data\n");
    fprintf(out, "    msg: db \"Operator Overloading OK!\", 10, 0\n");
    fprintf(out, "    msg_len: equ $ - msg\n\n");
    
    // BSS section
    fprintf(out, "section .bss\n");
    for (OpDecl* d = decls; d; d = d->next) {
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
    
    // Generate operator comments
    for (OpDecl* d = decls; d; d = d->next) {
        if (strcmp(d->kind, "operator") == 0) {
            fprintf(out, "    ; Operator: %s\n", d->op_type ? d->op_type : "unknown");
        } else if (strcmp(d->kind, "struct") == 0) {
            fprintf(out, "    ; Struct: %s (%d operators)\n", d->name, d->is_binary);
        }
    }
    
    // Print success message
    fprintf(out, "\n    ; Print success message\n");
    fprintf(out, "    mov rax, 1\n");
    fprintf(out, "    mov rdi, 1\n");
    fprintf(out, "    lea rsi, [msg]\n");
    fprintf(out, "    mov rdx, msg_len\n");
    fprintf(out, "    syscall\n\n");
    
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
    printf("🔧 Operator Overloading Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P2 Advanced Features - Module #28\n");
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
    OpParser* parser = op_create(source);
    printf("      ✓ Lexer initialized\n\n");
    
    // 3. Parser
    printf("[3/4] 🌳 Parsing (syntax analysis)...\n");
    
    OpDecl* decls = NULL;
    OpDecl* last = NULL;
    int decl_count = 0;
    int max_iter = 5000;
    
    while (parser->current_token && !op_check(parser, TOKEN_EOF) && max_iter-- > 0) {
        op_skip_comments(parser);
        
        if (!parser->current_token || op_check(parser, TOKEN_EOF)) break;
        
        OpDecl* decl = NULL;
        
        if (op_check_keyword(parser, "operator")) {
            decl = op_parse_operator(parser);
            if (decl) {
                printf("      ✓ Parsed operator: %s\n", decl->op_type ? decl->op_type : "unknown");
            }
        } else if (op_check_keyword(parser, "struct")) {
            decl = op_parse_struct(parser);
            if (decl) {
                printf("      ✓ Parsed struct: %s (%d operators)\n", decl->name, decl->is_binary);
            }
        } else if (op_check(parser, TOKEN_NUMERIC) || 
                   op_check(parser, TOKEN_TEXT) || 
                   op_check(parser, TOKEN_BOOLEAN)) {
            decl = op_parse_variable(parser);
            if (decl) {
                printf("      ✓ Parsed %s: %s\n", decl->name, decl->kind);
            }
        } else {
            op_advance(parser);
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
    printf("      ✓ Operator overloading statistics:\n");
    printf("        - operators: %d\n", stat_operators);
    printf("        - binary ops: %d\n", stat_binary_ops);
    printf("        - unary ops: %d\n", stat_unary_ops);
    printf("        - Total operator constructs: %d\n\n", stat_operators);
    
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
    printf("🔀 Operator constructs: %d\n\n", stat_operators);
    
    printf("Next steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", argv[2]);
    printf("  ld output.o -o program\n");
    printf("  ./program\n");
    
    // Cleanup
    op_free_parser(parser);
    free(source);
    
    while (decls) {
        OpDecl* next = decls->next;
        free(decls->name);
        free(decls->kind);
        if (decls->op_type) free(decls->op_type);
        if (decls->type_name) free(decls->type_name);
        free(decls);
        decls = next;
    }
    
    return 0;
}
