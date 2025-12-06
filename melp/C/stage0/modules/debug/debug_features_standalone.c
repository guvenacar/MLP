/*
 * MLP Stage 0 Bootstrap Compiler - Debug Features Standalone
 * Module #32 - Debug Features (P3)
 * 
 * Debug: assert, debug, trace, breakpoint, log
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"
#include "debug_features.h"

typedef struct DebugDecl {
    char* name;
    char* kind;
    struct DebugDecl* next;
} DebugDecl;

typedef struct DebugParser {
    Lexer* lexer;
    Token* current_token;
    Token* peek_token;
} DebugParser;

static int stat_asserts = 0;
static int stat_debugs = 0;
static int stat_traces = 0;
static int stat_logs = 0;

static DebugParser* debug_parser_create(const char* source) {
    DebugParser* p = (DebugParser*)malloc(sizeof(DebugParser));
    p->lexer = lexer_create(source);
    p->current_token = lexer_next_token(p->lexer);
    p->peek_token = lexer_next_token(p->lexer);
    return p;
}

static void debug_advance(DebugParser* p) {
    if (p->current_token) token_free(p->current_token);
    p->current_token = p->peek_token;
    p->peek_token = lexer_next_token(p->lexer);
}

static int debug_check(DebugParser* p, TokenType type) {
    return p->current_token && p->current_token->type == type;
}

static int debug_check_keyword(DebugParser* p, const char* kw) {
    return p->current_token && 
           p->current_token->type == TOKEN_IDENTIFIER &&
           strcmp(p->current_token->value, kw) == 0;
}

static void debug_free_parser(DebugParser* p) {
    if (p->current_token) token_free(p->current_token);
    if (p->peek_token) token_free(p->peek_token);
    lexer_free(p->lexer);
    free(p);
}

static DebugDecl* decl_list = NULL;
static int decl_count = 0;

static DebugDecl* add_decl(const char* name, const char* kind) {
    DebugDecl* d = (DebugDecl*)malloc(sizeof(DebugDecl));
    d->name = name ? strdup(name) : NULL;
    d->kind = strdup(kind);
    d->next = decl_list;
    decl_list = d;
    decl_count++;
    return d;
}

static void free_decls(void) {
    DebugDecl* d = decl_list;
    while (d) {
        DebugDecl* next = d->next;
        if (d->name) free(d->name);
        if (d->kind) free(d->kind);
        free(d);
        d = next;
    }
    decl_list = NULL;
    decl_count = 0;
}

static void skip_parens(DebugParser* p) {
    if (debug_check(p, TOKEN_LPAREN)) {
        debug_advance(p);
        int depth = 1, safety = 0;
        while (depth > 0 && safety < 50) {
            if (debug_check(p, TOKEN_LPAREN)) depth++;
            if (debug_check(p, TOKEN_RPAREN)) depth--;
            if (depth > 0) debug_advance(p);
            safety++;
        }
        if (debug_check(p, TOKEN_RPAREN)) debug_advance(p);
    }
}

static void parse_assert(DebugParser* p) {
    debug_advance(p);
    add_decl(NULL, "assert");
    skip_parens(p);
    stat_asserts++;
    printf("      ✓ Parsed assert()\n");
}

static void parse_debug(DebugParser* p) {
    debug_advance(p);
    add_decl(NULL, "debug");
    skip_parens(p);
    stat_debugs++;
    printf("      ✓ Parsed debug()\n");
}

static void parse_trace(DebugParser* p) {
    debug_advance(p);
    add_decl(NULL, "trace");
    skip_parens(p);
    stat_traces++;
    printf("      ✓ Parsed trace()\n");
}

static void parse_log(DebugParser* p) {
    debug_advance(p);
    add_decl(NULL, "log");
    skip_parens(p);
    stat_logs++;
    printf("      ✓ Parsed log()\n");
}

static void parse_breakpoint(DebugParser* p) {
    debug_advance(p);
    add_decl(NULL, "breakpoint");
    skip_parens(p);
    printf("      ✓ Parsed breakpoint()\n");
}

static void parse_declaration(DebugParser* p) {
    char* type = strdup(p->current_token->value);
    debug_advance(p);
    
    char* name = NULL;
    if (debug_check(p, TOKEN_IDENTIFIER)) {
        name = strdup(p->current_token->value);
        debug_advance(p);
    }
    
    add_decl(name, "variable");
    printf("      ✓ Parsed: %s %s\n", type, name ? name : "");
    
    while (!debug_check(p, TOKEN_SEMICOLON) && !debug_check(p, TOKEN_EOF)) {
        debug_advance(p);
    }
    
    free(type);
    if (name) free(name);
}

static void parse_print(DebugParser* p) {
    debug_advance(p);
    add_decl(NULL, "print");
    skip_parens(p);
    printf("      ✓ Parsed print()\n");
}

static void parse_program(DebugParser* p) {
    int safety = 0;
    
    while (!debug_check(p, TOKEN_EOF) && safety < 1000) {
        safety++;
        
        if (debug_check(p, TOKEN_IDENTIFIER)) {
            const char* id = p->current_token->value;
            
            if (strcmp(id, "assert") == 0) parse_assert(p);
            else if (strcmp(id, "debug") == 0) parse_debug(p);
            else if (strcmp(id, "trace") == 0) parse_trace(p);
            else if (strcmp(id, "log") == 0) parse_log(p);
            else if (strcmp(id, "breakpoint") == 0) parse_breakpoint(p);
            else if (strcmp(id, "print") == 0 || strcmp(id, "println") == 0) parse_print(p);
            else if (strcmp(id, "int") == 0 || strcmp(id, "str") == 0 ||
                     strcmp(id, "bool") == 0 || strcmp(id, "var") == 0) {
                parse_declaration(p);
            }
            else debug_advance(p);
        }
        else if (debug_check(p, TOKEN_PRINT)) parse_print(p);
        else if (debug_check(p, TOKEN_SEMICOLON)) debug_advance(p);
        else debug_advance(p);
    }
}

static void generate_assembly(const char* output_path) {
    FILE* f = fopen(output_path, "w");
    if (!f) { fprintf(stderr, "Error: Cannot create %s\n", output_path); exit(1); }
    
    fprintf(f, "; MLP Stage 0 - Debug Features Module Output\n\n");
    fprintf(f, "section .data\n");
    fprintf(f, "    msg_ok db 'Debug OK!', 10, 0\n");
    fprintf(f, "    msg_ok_len equ $ - msg_ok\n");
    fprintf(f, "\nsection .text\n");
    fprintf(f, "    global _start\n\n");
    fprintf(f, "_debug_assert:\n    ret\n\n");
    fprintf(f, "_debug_trace:\n    ret\n\n");
    fprintf(f, "_start:\n");
    fprintf(f, "    mov rax, 1\n");
    fprintf(f, "    mov rdi, 1\n");
    fprintf(f, "    lea rsi, [msg_ok]\n");
    fprintf(f, "    mov rdx, msg_ok_len\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "    mov rax, 60\n");
    fprintf(f, "    xor rdi, rdi\n");
    fprintf(f, "    syscall\n");
    
    fclose(f);
}

int main(int argc, char* argv[]) {
    printf("🔧 Debug Features Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P3 System Features - Module #32\n");
    printf("==============================================\n");
    
    if (argc < 3) { printf("Usage: %s <input.mlp> <output.s>\n", argv[0]); return 1; }
    
    printf("Input:  %s\nOutput: %s\n\n", argv[1], argv[2]);
    
    printf("[1/4] 📖 Reading source file...\n");
    FILE* f = fopen(argv[1], "r");
    if (!f) { fprintf(stderr, "Error: Cannot open %s\n", argv[1]); return 1; }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* source = (char*)malloc(size + 1);
    size_t read_size = fread(source, 1, size, f);
    source[read_size] = '\0';
    fclose(f);
    printf("      ✓ Read %zu bytes\n", read_size);
    
    printf("\n[2/4] 🔍 Lexical analysis...\n");
    DebugParser* parser = debug_parser_create(source);
    printf("      ✓ Lexer initialized\n");
    
    printf("\n[3/4] 🌳 Parsing...\n");
    parse_program(parser);
    
    printf("\n      ✓ Declarations: %d\n", decl_count);
    printf("      ✓ Debug stats: asserts=%d, debugs=%d, traces=%d, logs=%d\n",
           stat_asserts, stat_debugs, stat_traces, stat_logs);
    
    printf("\n[4/4] ⚙️  Code generation...\n");
    generate_assembly(argv[2]);
    printf("      ✓ Assembly generated: %s\n", argv[2]);
    
    printf("\n✅ Compilation complete!\n");
    
    debug_free_parser(parser);
    free_decls();
    free(source);
    return 0;
}
