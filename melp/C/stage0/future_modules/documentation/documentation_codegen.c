#include "documentation.h"
void doc_generate_asm(const char* output_file, DocCtx* ctx) {
    FILE* f = fopen(output_file, "w");
    if (!f) return;
    fprintf(f, "; MLP Documentation\n\nsection .data\n    msg: db \"Documentation OK!\", 10, 0\n    len: equ $ - msg\n\n");
    fprintf(f, "section .text\n    global _start\n\n_start:\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, msg\n    mov rdx, len\n    syscall\n");
    fprintf(f, "    mov rax, 60\n    xor rdi, rdi\n    syscall\n"); fclose(f);
}
