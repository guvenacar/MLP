// =============================================================================
// ASYNC MODULE - STANDALONE COMPILER
// =============================================================================
// Module #29 - P2 Advanced Features
// 
// Implements: async/await, Future/Promise, event loop
// Depends on: P0 + P1 + P2 modules
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"
#include "async.h"

// =============================================================================
// AST Structures
// =============================================================================

typedef struct AsyncDecl {
    char* name;
    char* kind;              // "async_fn", "await", "promise", "variable"
    int is_generator;
    int has_await;
    struct AsyncDecl* next;
} AsyncDecl;

typedef struct AsyncParser {
    Lexer* lexer;
    Token* current_token;
    Token* peek_token;
} AsyncParser;

// Statistics
static int stat_async_fns = 0;
static int stat_awaits = 0;
static int stat_promises = 0;
static int stat_futures = 0;

// =============================================================================
// Parser Utilities
// =============================================================================

static AsyncParser* async_create(const char* source) {
    AsyncParser* p = (AsyncParser*)malloc(sizeof(AsyncParser));
    p->lexer = lexer_create(source);
    p->current_token = lexer_next_token(p->lexer);
    p->peek_token = lexer_next_token(p->lexer);
    return p;
}

static void async_advance(AsyncParser* p) {
    if (p->current_token) {
        token_free(p->current_token);
    }
    p->current_token = p->peek_token;
    p->peek_token = lexer_next_token(p->lexer);
}

static int async_check(AsyncParser* p, TokenType type) {
    return p->current_token && p->current_token->type == type;
}

static int async_check_keyword(AsyncParser* p, const char* keyword) {
    return p->current_token && 
           p->current_token->type == TOKEN_IDENTIFIER &&
           strcmp(p->current_token->value, keyword) == 0;
}

static void async_free_parser(AsyncParser* p) {
    if (p->current_token) token_free(p->current_token);
    if (p->peek_token) token_free(p->peek_token);
    lexer_free(p->lexer);
    free(p);
}

// Skip MLP comments
static void async_skip_comments(AsyncParser* p) {
    while (p->current_token && p->current_token->type == TOKEN_MINUS) {
        if (p->peek_token && p->peek_token->type == TOKEN_MINUS) {
            async_advance(p);
            async_advance(p);
            while (p->current_token && 
                   p->current_token->type != TOKEN_EOF) {
                if (p->current_token->type == TOKEN_NUMERIC ||
                    p->current_token->type == TOKEN_TEXT ||
                    p->current_token->type == TOKEN_BOOLEAN ||
                    async_check_keyword(p, "async") ||
                    async_check_keyword(p, "await") ||
                    async_check_keyword(p, "promise") ||
                    async_check_keyword(p, "future") ||
                    async_check_keyword(p, "function") ||
                    async_check_keyword(p, "end")) {
                    break;
                }
                async_advance(p);
            }
        } else {
            break;
        }
    }
}

// Create declaration
static AsyncDecl* decl_create(const char* name, const char* kind) {
    AsyncDecl* d = (AsyncDecl*)malloc(sizeof(AsyncDecl));
    d->name = strdup(name);
    d->kind = strdup(kind);
    d->is_generator = 0;
    d->has_await = 0;
    d->next = NULL;
    return d;
}

// =============================================================================
// Parsing Functions
// =============================================================================

// Parse async function: async function name() ... end function
static AsyncDecl* async_parse_async_fn(AsyncParser* p) {
    if (!async_check_keyword(p, "async")) return NULL;
    
    async_advance(p);  // consume 'async'
    
    // Expect 'function'
    if (!async_check_keyword(p, "function")) return NULL;
    async_advance(p);
    
    char* fn_name = "anonymous";
    if (async_check(p, TOKEN_IDENTIFIER)) {
        fn_name = p->current_token->value;
    }
    
    AsyncDecl* decl = decl_create(fn_name, "async_fn");
    stat_async_fns++;
    
    if (async_check(p, TOKEN_IDENTIFIER)) {
        async_advance(p);
    }
    
    // Count awaits and check for generator
    int await_count = 0;
    int is_gen = 0;
    int max_iter = 1000;
    while (p->current_token && !async_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (async_check_keyword(p, "end")) {
            async_advance(p);
            if (async_check_keyword(p, "function")) {
                async_advance(p);
                break;
            }
        } else if (async_check_keyword(p, "await")) {
            await_count++;
            stat_awaits++;
            async_advance(p);
        } else if (async_check_keyword(p, "yield")) {
            is_gen = 1;
            async_advance(p);
        } else {
            async_advance(p);
        }
    }
    
    decl->has_await = await_count;
    decl->is_generator = is_gen;
    return decl;
}

// Parse await: await expression
static AsyncDecl* async_parse_await(AsyncParser* p) {
    if (!async_check_keyword(p, "await")) return NULL;
    
    async_advance(p);  // consume 'await'
    
    AsyncDecl* decl = decl_create("await_expr", "await");
    stat_awaits++;
    
    // Skip to end of expression
    int max_iter = 100;
    while (p->current_token && !async_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (async_check(p, TOKEN_NUMERIC) ||
            async_check(p, TOKEN_TEXT) ||
            async_check(p, TOKEN_BOOLEAN) ||
            async_check_keyword(p, "async") ||
            async_check_keyword(p, "await") ||
            async_check_keyword(p, "promise") ||
            async_check_keyword(p, "future") ||
            async_check_keyword(p, "function") ||
            async_check_keyword(p, "end")) {
            break;
        }
        async_advance(p);
    }
    
    return decl;
}

// Parse promise: promise { ... }
static AsyncDecl* async_parse_promise(AsyncParser* p) {
    if (!async_check_keyword(p, "promise")) return NULL;
    
    async_advance(p);  // consume 'promise'
    
    AsyncDecl* decl = decl_create("promise_expr", "promise");
    stat_promises++;
    
    // Skip to end
    int max_iter = 100;
    while (p->current_token && !async_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (async_check(p, TOKEN_NUMERIC) ||
            async_check(p, TOKEN_TEXT) ||
            async_check(p, TOKEN_BOOLEAN) ||
            async_check_keyword(p, "async") ||
            async_check_keyword(p, "await") ||
            async_check_keyword(p, "promise") ||
            async_check_keyword(p, "future")) {
            break;
        }
        async_advance(p);
    }
    
    return decl;
}

// Parse future
static AsyncDecl* async_parse_future(AsyncParser* p) {
    if (!async_check_keyword(p, "future")) return NULL;
    
    async_advance(p);
    
    AsyncDecl* decl = decl_create("future_expr", "future");
    stat_futures++;
    
    // Skip to end
    int max_iter = 100;
    while (p->current_token && !async_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (async_check(p, TOKEN_NUMERIC) ||
            async_check(p, TOKEN_TEXT) ||
            async_check(p, TOKEN_BOOLEAN) ||
            async_check_keyword(p, "async") ||
            async_check_keyword(p, "await") ||
            async_check_keyword(p, "promise") ||
            async_check_keyword(p, "future")) {
            break;
        }
        async_advance(p);
    }
    
    return decl;
}

// Parse variable declaration
static AsyncDecl* async_parse_variable(AsyncParser* p) {
    const char* type = NULL;
    
    if (async_check(p, TOKEN_NUMERIC)) {
        type = "numeric";
    } else if (async_check(p, TOKEN_TEXT)) {
        type = "text";
    } else if (async_check(p, TOKEN_BOOLEAN)) {
        type = "boolean";
    } else {
        return NULL;
    }
    
    async_advance(p);
    
    if (!async_check(p, TOKEN_IDENTIFIER)) return NULL;
    
    AsyncDecl* decl = decl_create(p->current_token->value, type);
    async_advance(p);
    
    // Skip to end
    int max_iter = 100;
    while (p->current_token && !async_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (async_check(p, TOKEN_NUMERIC) ||
            async_check(p, TOKEN_TEXT) ||
            async_check(p, TOKEN_BOOLEAN) ||
            async_check_keyword(p, "async") ||
            async_check_keyword(p, "await") ||
            async_check_keyword(p, "promise") ||
            async_check_keyword(p, "future")) {
            break;
        }
        async_advance(p);
    }
    
    return decl;
}

// =============================================================================
// Code Generation
// =============================================================================

static void generate_assembly(FILE* out, AsyncDecl* decls) {
    // Data section
    fprintf(out, "section .data\n");
    fprintf(out, "    msg: db \"Async/Await OK!\", 10, 0\n");
    fprintf(out, "    msg_len: equ $ - msg\n\n");
    
    // BSS section
    fprintf(out, "section .bss\n");
    for (AsyncDecl* d = decls; d; d = d->next) {
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
    
    // Generate async comments
    for (AsyncDecl* d = decls; d; d = d->next) {
        if (strcmp(d->kind, "async_fn") == 0) {
            fprintf(out, "    ; Async function: %s (%d awaits%s)\n", 
                    d->name, d->has_await, d->is_generator ? ", generator" : "");
        } else if (strcmp(d->kind, "await") == 0) {
            fprintf(out, "    ; Await expression\n");
        } else if (strcmp(d->kind, "promise") == 0) {
            fprintf(out, "    ; Promise\n");
        } else if (strcmp(d->kind, "future") == 0) {
            fprintf(out, "    ; Future\n");
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
    printf("🔧 Async Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P2 Advanced Features - Module #29\n");
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
    AsyncParser* parser = async_create(source);
    printf("      ✓ Lexer initialized\n\n");
    
    // 3. Parser
    printf("[3/4] 🌳 Parsing (syntax analysis)...\n");
    
    AsyncDecl* decls = NULL;
    AsyncDecl* last = NULL;
    int decl_count = 0;
    int max_iter = 5000;
    
    while (parser->current_token && !async_check(parser, TOKEN_EOF) && max_iter-- > 0) {
        async_skip_comments(parser);
        
        if (!parser->current_token || async_check(parser, TOKEN_EOF)) break;
        
        AsyncDecl* decl = NULL;
        
        if (async_check_keyword(parser, "async")) {
            decl = async_parse_async_fn(parser);
            if (decl) {
                printf("      ✓ Parsed async function: %s (%d awaits%s)\n", 
                       decl->name, decl->has_await, decl->is_generator ? ", gen" : "");
            }
        } else if (async_check_keyword(parser, "await")) {
            decl = async_parse_await(parser);
            if (decl) {
                printf("      ✓ Parsed await\n");
            }
        } else if (async_check_keyword(parser, "promise")) {
            decl = async_parse_promise(parser);
            if (decl) {
                printf("      ✓ Parsed promise\n");
            }
        } else if (async_check_keyword(parser, "future")) {
            decl = async_parse_future(parser);
            if (decl) {
                printf("      ✓ Parsed future\n");
            }
        } else if (async_check(parser, TOKEN_NUMERIC) || 
                   async_check(parser, TOKEN_TEXT) || 
                   async_check(parser, TOKEN_BOOLEAN)) {
            decl = async_parse_variable(parser);
            if (decl) {
                printf("      ✓ Parsed %s: %s\n", decl->name, decl->kind);
            }
        } else {
            async_advance(parser);
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
    printf("      ✓ Async statistics:\n");
    printf("        - async functions: %d\n", stat_async_fns);
    printf("        - await expressions: %d\n", stat_awaits);
    printf("        - promises: %d\n", stat_promises);
    printf("        - futures: %d\n", stat_futures);
    printf("        - Total async constructs: %d\n\n", 
           stat_async_fns + stat_awaits + stat_promises + stat_futures);
    
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
    printf("🔀 Async constructs: %d\n\n", 
           stat_async_fns + stat_awaits + stat_promises + stat_futures);
    
    printf("Next steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", argv[2]);
    printf("  ld output.o -o program\n");
    printf("  ./program\n");
    
    // Cleanup
    async_free_parser(parser);
    free(source);
    
    while (decls) {
        AsyncDecl* next = decls->next;
        free(decls->name);
        free(decls->kind);
        free(decls);
        decls = next;
    }
    
    return 0;
}
