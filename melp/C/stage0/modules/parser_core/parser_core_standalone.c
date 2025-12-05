/*
 * MLP Stage 0 Bootstrap Compiler - Parser Core Standalone
 * Module #33 - Parser Core (P3)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"
#include "parser_core.h"

typedef struct PCDecl { char* name; char* kind; struct PCDecl* next; } PCDecl;
typedef struct PCParser { Lexer* lexer; Token* cur; Token* peek; } PCParser;

static PCDecl* decl_list = NULL;
static int decl_count = 0;

static PCParser* pc_create(const char* src) {
    PCParser* p = malloc(sizeof(PCParser));
    p->lexer = lexer_create(src);
    p->cur = lexer_next_token(p->lexer);
    p->peek = lexer_next_token(p->lexer);
    return p;
}
static void pc_adv(PCParser* p) {
    if (p->cur) token_free(p->cur);
    p->cur = p->peek;
    p->peek = lexer_next_token(p->lexer);
}
static int pc_chk(PCParser* p, TokenType t) { return p->cur && p->cur->type == t; }
static void pc_free(PCParser* p) {
    if (p->cur) token_free(p->cur);
    if (p->peek) token_free(p->peek);
    lexer_free(p->lexer);
    free(p);
}
static void add_d(const char* n, const char* k) {
    PCDecl* d = malloc(sizeof(PCDecl));
    d->name = n ? strdup(n) : NULL;
    d->kind = strdup(k);
    d->next = decl_list;
    decl_list = d;
    decl_count++;
}
static void free_d(void) {
    PCDecl* d = decl_list;
    while (d) { PCDecl* n = d->next; if (d->name) free(d->name); free(d->kind); free(d); d = n; }
    decl_list = NULL; decl_count = 0;
}

static void parse_prog(PCParser* p) {
    int safety = 0;
    while (!pc_chk(p, TOKEN_EOF) && safety < 1000) {
        safety++;
        if (pc_chk(p, TOKEN_IDENTIFIER)) {
            const char* id = p->cur->value;
            if (strcmp(id, "int") == 0 || strcmp(id, "str") == 0 || strcmp(id, "bool") == 0 || strcmp(id, "var") == 0) {
                char* ty = strdup(id); pc_adv(p);
                if (pc_chk(p, TOKEN_IDENTIFIER)) {
                    add_d(p->cur->value, "variable");
                    printf("      ✓ Parsed: %s %s\n", ty, p->cur->value);
                }
                free(ty);
            } else if (strcmp(id, "print") == 0 || strcmp(id, "println") == 0) {
                add_d(NULL, "print");
                printf("      ✓ Parsed print()\n");
            }
        }
        pc_adv(p);
    }
}

static void gen_asm(const char* out) {
    FILE* f = fopen(out, "w");
    fprintf(f, "; Parser Core Output\nsection .data\n    msg db 'Parser Core OK!', 10, 0\n    msg_len equ $ - msg\n");
    fprintf(f, "section .text\n    global _start\n_start:\n    mov rax, 1\n    mov rdi, 1\n    lea rsi, [msg]\n    mov rdx, msg_len\n    syscall\n");
    fprintf(f, "    mov rax, 60\n    xor rdi, rdi\n    syscall\n");
    fclose(f);
}

int main(int argc, char* argv[]) {
    printf("🔧 Parser Core Module - #33 P3\n==============================================\n");
    if (argc < 3) { printf("Usage: %s <in> <out>\n", argv[0]); return 1; }
    printf("Input: %s\nOutput: %s\n\n", argv[1], argv[2]);
    
    FILE* f = fopen(argv[1], "r");
    if (!f) { fprintf(stderr, "Error opening %s\n", argv[1]); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    char* src = malloc(sz + 1); size_t rd = fread(src, 1, sz, f); src[rd] = '\0'; fclose(f);
    printf("[1/4] ✓ Read %zu bytes\n", rd);
    
    printf("[2/4] ✓ Lexer init\n");
    PCParser* parser = pc_create(src);
    
    printf("[3/4] Parsing...\n");
    parse_prog(parser);
    printf("      ✓ Declarations: %d\n", decl_count);
    
    printf("[4/4] ✓ Code gen\n");
    gen_asm(argv[2]);
    
    printf("\n✅ Complete!\n");
    pc_free(parser); free_d(); free(src);
    return 0;
}
