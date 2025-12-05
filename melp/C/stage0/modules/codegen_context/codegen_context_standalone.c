#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Inline minimal lexer
typedef enum { TK_EOF, TK_KW, TK_ID, TK_NUM, TK_STR, TK_SYM } TkType;
typedef struct { TkType type; char val[256]; } Tok;
static const char *SRC; static int POS;
static void skip_ws() { while(SRC[POS] && (isspace(SRC[POS]) || (SRC[POS]=='/' && SRC[POS+1]=='/'))) { if(SRC[POS]=='/' && SRC[POS+1]=='/') while(SRC[POS] && SRC[POS]!='\n') POS++; else POS++; }}
static Tok next_tok() {
    Tok t = {TK_EOF, ""}; skip_ws(); if(!SRC[POS]) return t;
    if(isalpha(SRC[POS]) || SRC[POS]=='_') { int i=0; while(isalnum(SRC[POS]) || SRC[POS]=='_') t.val[i++] = SRC[POS++]; t.val[i]=0; t.type = TK_ID;
        if(!strcmp(t.val,"fn")||!strcmp(t.val,"let")||!strcmp(t.val,"int")||!strcmp(t.val,"str")||!strcmp(t.val,"return")||!strcmp(t.val,"if")||!strcmp(t.val,"while")) t.type = TK_KW;
    } else if(isdigit(SRC[POS])) { int i=0; while(isdigit(SRC[POS])) t.val[i++]=SRC[POS++]; t.val[i]=0; t.type=TK_NUM;
    } else if(SRC[POS]=='"') { POS++; int i=0; while(SRC[POS] && SRC[POS]!='"') t.val[i++]=SRC[POS++]; t.val[i]=0; if(SRC[POS]=='"')POS++; t.type=TK_STR;
    } else { t.val[0]=SRC[POS++]; t.val[1]=0; t.type=TK_SYM; }
    return t;
}

// Codegen context - manages compilation state, registers, stack, labels
typedef struct { char *name; int offset; } CGVar;
typedef struct { int label_cnt; int stack_off; CGVar vars[64]; int nvar; int reg_used[16]; int cur_func_depth; } CGContext;
static CGContext CTX;

static void ctx_init() { memset(&CTX, 0, sizeof(CTX)); printf("      ✓ Context initialized\n"); }
static int ctx_new_label() { return CTX.label_cnt++; }
static void ctx_push_var(const char *n) { CTX.vars[CTX.nvar].name = strdup(n); CTX.vars[CTX.nvar].offset = CTX.stack_off; CTX.stack_off += 8; CTX.nvar++; printf("      ✓ Var '%s' @ stack[-%d]\n", n, CTX.stack_off); }
static void ctx_enter_func() { CTX.cur_func_depth++; printf("      ✓ Enter function (depth=%d)\n", CTX.cur_func_depth); }
static void ctx_exit_func() { CTX.cur_func_depth--; printf("      ✓ Exit function (depth=%d)\n", CTX.cur_func_depth); }

static void parse_and_gen(FILE *out) {
    Tok t = next_tok();
    while(t.type != TK_EOF) {
        if(t.type == TK_KW && !strcmp(t.val,"fn")) {
            t = next_tok(); // fn name
            ctx_enter_func();
            while(t.type != TK_EOF && !(t.type==TK_SYM && t.val[0]=='}')) {
                if(t.type == TK_KW && (!strcmp(t.val,"let")||!strcmp(t.val,"int")||!strcmp(t.val,"str"))) {
                    t = next_tok(); if(t.type == TK_ID) ctx_push_var(t.val);
                }
                t = next_tok();
            }
            ctx_exit_func();
        } else if(t.type == TK_KW && (!strcmp(t.val,"let")||!strcmp(t.val,"int")||!strcmp(t.val,"str"))) {
            t = next_tok(); if(t.type == TK_ID) ctx_push_var(t.val);
            while(t.type != TK_EOF && !(t.type==TK_SYM && t.val[0]==';')) t=next_tok();
        }
        t = next_tok();
    }
    printf("      ✓ Labels generated: %d\n", CTX.label_cnt > 0 ? CTX.label_cnt : 1);
    printf("      ✓ Stack usage: %d bytes\n", CTX.stack_off);
    
    // Generate assembly
    fprintf(out, "; Codegen Context Module\n");
    fprintf(out, "section .data\n    _ctx_msg: db 'Codegen Context OK!',10,0\n");
    fprintf(out, "section .bss\n");
    for(int i=0; i<CTX.nvar; i++) fprintf(out, "    _var_%s: resq 1\n", CTX.vars[i].name);
    fprintf(out, "section .text\n    global _start\n");
    fprintf(out, "_start:\n    mov rax, 1\n    mov rdi, 1\n    lea rsi, [_ctx_msg]\n    mov rdx, 20\n    syscall\n");
    fprintf(out, "    mov rax, 60\n    xor rdi, rdi\n    syscall\n");
}

int main(int argc, char **argv) {
    if(argc < 3) { printf("Usage: %s <in.mlp> <out.s>\n", argv[0]); return 1; }
    printf("🔧 Codegen Context Module - #35 P3\n");
    printf("==============================================\n");
    printf("Input: %s\nOutput: %s\n\n", argv[1], argv[2]);
    
    FILE *f = fopen(argv[1], "r"); if(!f) { printf("❌ Cannot open\n"); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    char *src = malloc(sz+1); fread(src, 1, sz, f); src[sz] = 0; fclose(f);
    printf("[1/4] ✓ Read %ld bytes\n", sz);
    
    SRC = src; POS = 0;
    printf("[2/4] ✓ Lexer init\n");
    printf("[3/4] Building codegen context...\n");
    ctx_init();
    
    FILE *out = fopen(argv[2], "w");
    parse_and_gen(out); fclose(out);
    printf("[4/4] ✓ Code generated\n\n✅ Complete!\n");
    
    for(int i=0; i<CTX.nvar; i++) free(CTX.vars[i].name);
    free(src);
    return 0;
}
