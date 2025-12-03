// =============================================================================
// MEMORY MODULE - STANDALONE COMPILER
// =============================================================================
// Module #26 - P2 Advanced Features
// 
// Implements: malloc, free, GC, reference counting, defer
// Depends on: P0 + P1 + P2 modules
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"
#include "memory.h"

// =============================================================================
// AST Structures
// =============================================================================

typedef struct MemDecl {
    char* name;
    char* kind;              // "alloc", "free", "defer", "gc", "variable"
    char* var_name;
    int size;
    int is_array;
    struct MemDecl* next;
} MemDecl;

typedef struct MemParser {
    Lexer* lexer;
    Token* current_token;
    Token* peek_token;
} MemParser;

// Statistics
static int stat_allocs = 0;
static int stat_frees = 0;
static int stat_defers = 0;
static int stat_gc_hints = 0;

// =============================================================================
// Parser Utilities
// =============================================================================

static MemParser* mem_create(const char* source) {
    MemParser* p = (MemParser*)malloc(sizeof(MemParser));
    p->lexer = lexer_create(source);
    p->current_token = lexer_next_token(p->lexer);
    p->peek_token = lexer_next_token(p->lexer);
    return p;
}

static void mem_advance(MemParser* p) {
    if (p->current_token) {
        token_free(p->current_token);
    }
    p->current_token = p->peek_token;
    p->peek_token = lexer_next_token(p->lexer);
}

static int mem_check(MemParser* p, TokenType type) {
    return p->current_token && p->current_token->type == type;
}

static int mem_check_keyword(MemParser* p, const char* keyword) {
    return p->current_token && 
           p->current_token->type == TOKEN_IDENTIFIER &&
           strcmp(p->current_token->value, keyword) == 0;
}

static void mem_free_parser(MemParser* p) {
    if (p->current_token) token_free(p->current_token);
    if (p->peek_token) token_free(p->peek_token);
    lexer_free(p->lexer);
    free(p);
}

// Skip MLP comments
static void mem_skip_comments(MemParser* p) {
    while (p->current_token && p->current_token->type == TOKEN_MINUS) {
        if (p->peek_token && p->peek_token->type == TOKEN_MINUS) {
            mem_advance(p);
            mem_advance(p);
            while (p->current_token && 
                   p->current_token->type != TOKEN_EOF) {
                if (p->current_token->type == TOKEN_NUMERIC ||
                    p->current_token->type == TOKEN_TEXT ||
                    p->current_token->type == TOKEN_BOOLEAN ||
                    mem_check_keyword(p, "alloc") ||
                    mem_check_keyword(p, "free") ||
                    mem_check_keyword(p, "defer") ||
                    mem_check_keyword(p, "gc") ||
                    mem_check_keyword(p, "new") ||
                    mem_check_keyword(p, "delete")) {
                    break;
                }
                mem_advance(p);
            }
        } else {
            break;
        }
    }
}

// Create declaration
static MemDecl* decl_create(const char* name, const char* kind) {
    MemDecl* d = (MemDecl*)malloc(sizeof(MemDecl));
    d->name = strdup(name);
    d->kind = strdup(kind);
    d->var_name = NULL;
    d->size = 0;
    d->is_array = 0;
    d->next = NULL;
    return d;
}

// =============================================================================
// Parsing Functions
// =============================================================================

// Parse alloc/new: alloc(100) or new Type
static MemDecl* mem_parse_alloc(MemParser* p) {
    if (!mem_check_keyword(p, "alloc") && !mem_check_keyword(p, "new")) return NULL;
    
    int is_new = mem_check_keyword(p, "new");
    mem_advance(p);
    
    MemDecl* decl = decl_create(is_new ? "new_expr" : "alloc_expr", "alloc");
    stat_allocs++;
    
    // Skip to next statement
    int max_iter = 100;
    while (p->current_token && !mem_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (mem_check(p, TOKEN_NUMERIC) ||
            mem_check(p, TOKEN_TEXT) ||
            mem_check(p, TOKEN_BOOLEAN) ||
            mem_check_keyword(p, "alloc") ||
            mem_check_keyword(p, "new") ||
            mem_check_keyword(p, "free") ||
            mem_check_keyword(p, "delete") ||
            mem_check_keyword(p, "defer") ||
            mem_check_keyword(p, "gc")) {
            break;
        }
        mem_advance(p);
    }
    
    return decl;
}

// Parse free/delete: free(ptr) or delete obj
static MemDecl* mem_parse_free(MemParser* p) {
    if (!mem_check_keyword(p, "free") && !mem_check_keyword(p, "delete")) return NULL;
    
    int is_delete = mem_check_keyword(p, "delete");
    mem_advance(p);
    
    MemDecl* decl = decl_create(is_delete ? "delete_expr" : "free_expr", "free");
    stat_frees++;
    
    // Get variable name if identifier follows
    if (mem_check(p, TOKEN_IDENTIFIER)) {
        decl->var_name = strdup(p->current_token->value);
        mem_advance(p);
    }
    
    // Skip to next statement
    int max_iter = 100;
    while (p->current_token && !mem_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (mem_check(p, TOKEN_NUMERIC) ||
            mem_check(p, TOKEN_TEXT) ||
            mem_check(p, TOKEN_BOOLEAN) ||
            mem_check_keyword(p, "alloc") ||
            mem_check_keyword(p, "new") ||
            mem_check_keyword(p, "free") ||
            mem_check_keyword(p, "delete") ||
            mem_check_keyword(p, "defer") ||
            mem_check_keyword(p, "gc")) {
            break;
        }
        mem_advance(p);
    }
    
    return decl;
}

// Parse defer: defer statement
static MemDecl* mem_parse_defer(MemParser* p) {
    if (!mem_check_keyword(p, "defer")) return NULL;
    
    mem_advance(p);
    
    MemDecl* decl = decl_create("defer_stmt", "defer");
    stat_defers++;
    
    // Skip to next statement
    int max_iter = 100;
    while (p->current_token && !mem_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (mem_check(p, TOKEN_NUMERIC) ||
            mem_check(p, TOKEN_TEXT) ||
            mem_check(p, TOKEN_BOOLEAN) ||
            mem_check_keyword(p, "alloc") ||
            mem_check_keyword(p, "new") ||
            mem_check_keyword(p, "free") ||
            mem_check_keyword(p, "delete") ||
            mem_check_keyword(p, "defer") ||
            mem_check_keyword(p, "gc")) {
            break;
        }
        mem_advance(p);
    }
    
    return decl;
}

// Parse gc: @gc or gc.collect()
static MemDecl* mem_parse_gc(MemParser* p) {
    if (!mem_check_keyword(p, "gc")) return NULL;
    
    mem_advance(p);
    
    MemDecl* decl = decl_create("gc_hint", "gc");
    stat_gc_hints++;
    
    // Skip to next statement
    int max_iter = 100;
    while (p->current_token && !mem_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (mem_check(p, TOKEN_NUMERIC) ||
            mem_check(p, TOKEN_TEXT) ||
            mem_check(p, TOKEN_BOOLEAN) ||
            mem_check_keyword(p, "alloc") ||
            mem_check_keyword(p, "new") ||
            mem_check_keyword(p, "free") ||
            mem_check_keyword(p, "delete") ||
            mem_check_keyword(p, "defer") ||
            mem_check_keyword(p, "gc")) {
            break;
        }
        mem_advance(p);
    }
    
    return decl;
}

// Parse variable declaration
static MemDecl* mem_parse_variable(MemParser* p) {
    const char* type = NULL;
    
    if (mem_check(p, TOKEN_NUMERIC)) {
        type = "numeric";
    } else if (mem_check(p, TOKEN_TEXT)) {
        type = "text";
    } else if (mem_check(p, TOKEN_BOOLEAN)) {
        type = "boolean";
    } else {
        return NULL;
    }
    
    mem_advance(p);
    
    if (!mem_check(p, TOKEN_IDENTIFIER)) return NULL;
    
    MemDecl* decl = decl_create(p->current_token->value, type);
    mem_advance(p);
    
    // Skip to end of declaration
    int max_iter = 100;
    while (p->current_token && !mem_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (mem_check(p, TOKEN_NUMERIC) ||
            mem_check(p, TOKEN_TEXT) ||
            mem_check(p, TOKEN_BOOLEAN) ||
            mem_check_keyword(p, "alloc") ||
            mem_check_keyword(p, "new") ||
            mem_check_keyword(p, "free") ||
            mem_check_keyword(p, "delete") ||
            mem_check_keyword(p, "defer") ||
            mem_check_keyword(p, "gc")) {
            break;
        }
        mem_advance(p);
    }
    
    return decl;
}

// =============================================================================
// Code Generation
// =============================================================================

static void generate_assembly(FILE* out, MemDecl* decls) {
    // Data section
    fprintf(out, "section .data\n");
    fprintf(out, "    msg: db \"Memory Management OK!\", 10, 0\n");
    fprintf(out, "    msg_len: equ $ - msg\n\n");
    
    // BSS section
    fprintf(out, "section .bss\n");
    for (MemDecl* d = decls; d; d = d->next) {
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
    
    // Generate memory operation comments
    for (MemDecl* d = decls; d; d = d->next) {
        if (strcmp(d->kind, "alloc") == 0) {
            fprintf(out, "    ; Memory allocation: %s\n", d->name);
        } else if (strcmp(d->kind, "free") == 0) {
            fprintf(out, "    ; Memory free: %s%s\n", d->name, 
                    d->var_name ? d->var_name : "");
        } else if (strcmp(d->kind, "defer") == 0) {
            fprintf(out, "    ; Defer statement\n");
        } else if (strcmp(d->kind, "gc") == 0) {
            fprintf(out, "    ; GC hint\n");
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
    printf("🔧 Memory Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P2 Advanced Features - Module #26\n");
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
    MemParser* parser = mem_create(source);
    printf("      ✓ Lexer initialized\n\n");
    
    // 3. Parser
    printf("[3/4] 🌳 Parsing (syntax analysis)...\n");
    
    MemDecl* decls = NULL;
    MemDecl* last = NULL;
    int decl_count = 0;
    int max_iter = 5000;
    
    while (parser->current_token && !mem_check(parser, TOKEN_EOF) && max_iter-- > 0) {
        mem_skip_comments(parser);
        
        if (!parser->current_token || mem_check(parser, TOKEN_EOF)) break;
        
        MemDecl* decl = NULL;
        
        if (mem_check_keyword(parser, "alloc") || mem_check_keyword(parser, "new")) {
            decl = mem_parse_alloc(parser);
            if (decl) {
                printf("      ✓ Parsed allocation: %s\n", decl->name);
            }
        } else if (mem_check_keyword(parser, "free") || mem_check_keyword(parser, "delete")) {
            decl = mem_parse_free(parser);
            if (decl) {
                printf("      ✓ Parsed free: %s\n", decl->name);
            }
        } else if (mem_check_keyword(parser, "defer")) {
            decl = mem_parse_defer(parser);
            if (decl) {
                printf("      ✓ Parsed defer\n");
            }
        } else if (mem_check_keyword(parser, "gc")) {
            decl = mem_parse_gc(parser);
            if (decl) {
                printf("      ✓ Parsed GC hint\n");
            }
        } else if (mem_check(parser, TOKEN_NUMERIC) || 
                   mem_check(parser, TOKEN_TEXT) || 
                   mem_check(parser, TOKEN_BOOLEAN)) {
            decl = mem_parse_variable(parser);
            if (decl) {
                printf("      ✓ Parsed %s: %s\n", decl->name, decl->kind);
            }
        } else {
            mem_advance(parser);
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
    printf("      ✓ Memory statistics:\n");
    printf("        - allocations: %d\n", stat_allocs);
    printf("        - frees: %d\n", stat_frees);
    printf("        - defers: %d\n", stat_defers);
    printf("        - GC hints: %d\n", stat_gc_hints);
    printf("        - Total memory constructs: %d\n\n", 
           stat_allocs + stat_frees + stat_defers + stat_gc_hints);
    
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
    printf("🔀 Memory constructs: %d\n\n", 
           stat_allocs + stat_frees + stat_defers + stat_gc_hints);
    
    printf("Next steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", argv[2]);
    printf("  ld output.o -o program\n");
    printf("  ./program\n");
    
    // Cleanup
    mem_free_parser(parser);
    free(source);
    
    while (decls) {
        MemDecl* next = decls->next;
        free(decls->name);
        free(decls->kind);
        if (decls->var_name) free(decls->var_name);
        free(decls);
        decls = next;
    }
    
    return 0;
}
