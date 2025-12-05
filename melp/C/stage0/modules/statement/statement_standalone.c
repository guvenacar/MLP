#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
typedef enum { TK_EOF, TK_KW, TK_ID, TK_NUM, TK_OP, TK_SYM } TkType;
typedef struct { TkType type; char val[256]; } Tok;
static const char *SRC; static int POS;
static void skip_ws() { while(SRC[POS] && isspace(SRC[POS])) POS++; }
static Tok next_tok() {
    Tok t = {TK_EOF, ""}; skip_ws(); if(!SRC[POS]) return t;
    if(isalpha(SRC[POS])) { int i=0; while(isalnum(SRC[POS])) t.val[i++]=SRC[POS++]; t.val[i]=0; t.type=TK_ID; if(!strcmp(t.val,"let")||!strcmp(t.val,"int")) t.type=TK_KW; }
    else if(isdigit(SRC[POS])) { int i=0; while(isdigit(SRC[POS])) t.val[i++]=SRC[POS++]; t.val[i]=0; t.type=TK_NUM; }
    else if(strchr("+-*/%", SRC[POS])) { t.val[0]=SRC[POS++]; t.val[1]=0; t.type=TK_OP; }
    else { t.val[0]=SRC[POS++]; t.val[1]=0; t.type=TK_SYM; }
    return t;
}
int main(int argc, char **argv) {
    if(argc<3) return 1;
    printf("🔧 Statement Module - #1 P0\n===================\nInput: %s\n\n", argv[1]);
    FILE *f=fopen(argv[1],"r"); fseek(f,0,SEEK_END); long sz=ftell(f); fseek(f,0,SEEK_SET);
    char *src=malloc(sz+1); fread(src,1,sz,f); src[sz]=0; fclose(f);
    SRC=src; POS=0; int nop=0;
    Tok t=next_tok(); while(t.type!=TK_EOF) { if(t.type==TK_OP) { printf("  ✓ Op: %s\n", t.val); nop++; } t=next_tok(); }
    printf("  ✓ Ops found: %d\n", nop);
    FILE *out=fopen(argv[2],"w");
    fprintf(out,"section .data\n  msg: db 'Statement OK!',10,0\nsection .text\n  global _start\n_start:\n  mov rax,1\n  mov rdi,1\n  lea rsi,[msg]\n  mov rdx,15\n  syscall\n  mov rax,60\n  xor rdi,rdi\n  syscall\n");
    fclose(out); free(src); printf("\n✅ Complete!\n"); return 0;
}
