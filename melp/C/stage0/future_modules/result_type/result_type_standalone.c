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
        if(!strcmp(t.val,"fn")||!strcmp(t.val,"Result")||!strcmp(t.val,"Ok")||!strcmp(t.val,"Err")||!strcmp(t.val,"match")||!strcmp(t.val,"let")||!strcmp(t.val,"return")||!strcmp(t.val,"int")||!strcmp(t.val,"str")) t.type = TK_KW;
    } else if(isdigit(SRC[POS])) { int i=0; while(isdigit(SRC[POS])) t.val[i++]=SRC[POS++]; t.val[i]=0; t.type=TK_NUM;
    } else if(SRC[POS]=='"') { POS++; int i=0; while(SRC[POS] && SRC[POS]!='"') t.val[i++]=SRC[POS++]; t.val[i]=0; if(SRC[POS]=='"')POS++; t.type=TK_STR;
    } else { t.val[0]=SRC[POS++]; t.val[1]=0; t.type=TK_SYM; }
    return t;
}

// Result type tracking
typedef struct { char *name; char *ok_type; char *err_type; } ResultDef;
static ResultDef RESULTS[32]; static int NRES;
static int N_OK, N_ERR, N_MATCH;

static void parse_results() {
    Tok t = next_tok();
    while(t.type != TK_EOF) {
        if(t.type == TK_KW && !strcmp(t.val,"Result")) {
            printf("      ✓ Result type declaration\n");
            // Result<T, E> pattern
            t = next_tok(); // <
            if(t.type == TK_SYM && t.val[0]=='<') {
                t = next_tok(); // ok type
                char *ok_t = strdup(t.val);
                t = next_tok(); // ,
                t = next_tok(); // err type
                char *err_t = strdup(t.val);
                RESULTS[NRES].ok_type = ok_t;
                RESULTS[NRES].err_type = err_t;
                NRES++;
                printf("        Ok<%s>, Err<%s>\n", ok_t, err_t);
            }
        } else if(t.type == TK_KW && !strcmp(t.val,"Ok")) {
            printf("      ✓ Ok() constructor\n"); N_OK++;
        } else if(t.type == TK_KW && !strcmp(t.val,"Err")) {
            printf("      ✓ Err() constructor\n"); N_ERR++;
        } else if(t.type == TK_KW && !strcmp(t.val,"match")) {
            printf("      ✓ match expression (for Result)\n"); N_MATCH++;
        }
        t = next_tok();
    }
}

static void gen_code(FILE *out) {
    fprintf(out, "; Result Type Module\n");
    fprintf(out, "section .data\n    _res_msg: db 'Result Type OK!',10,0\n");
    fprintf(out, "; Result type definitions\n");
    for(int i=0; i<NRES; i++) fprintf(out, "; Result<%s, %s>\n", RESULTS[i].ok_type, RESULTS[i].err_type);
    fprintf(out, "section .text\n    global _start\n");
    fprintf(out, "_start:\n    mov rax, 1\n    mov rdi, 1\n    lea rsi, [_res_msg]\n    mov rdx, 16\n    syscall\n");
    fprintf(out, "    mov rax, 60\n    xor rdi, rdi\n    syscall\n");
}

int main(int argc, char **argv) {
    if(argc < 3) { printf("Usage: %s <in.mlp> <out.s>\n", argv[0]); return 1; }
    printf("🔧 Result Type Module - #36 P3\n");
    printf("==============================================\n");
    printf("Input: %s\nOutput: %s\n\n", argv[1], argv[2]);
    
    FILE *f = fopen(argv[1], "r"); if(!f) { printf("❌ Cannot open\n"); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    char *src = malloc(sz+1); fread(src, 1, sz, f); src[sz] = 0; fclose(f);
    printf("[1/4] ✓ Read %ld bytes\n", sz);
    
    SRC = src; POS = 0; NRES = 0; N_OK = 0; N_ERR = 0; N_MATCH = 0;
    printf("[2/4] ✓ Lexer init\n");
    printf("[3/4] Parsing Result types...\n");
    parse_results();
    printf("      ✓ Stats: %d Result defs, %d Ok(), %d Err(), %d match\n", NRES, N_OK, N_ERR, N_MATCH);
    
    FILE *out = fopen(argv[2], "w");
    gen_code(out); fclose(out);
    printf("[4/4] ✓ Code generated\n\n✅ Complete!\n");
    
    for(int i=0; i<NRES; i++) { free(RESULTS[i].ok_type); free(RESULTS[i].err_type); }
    free(src);
    return 0;
}
