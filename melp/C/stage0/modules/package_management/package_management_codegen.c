#include "package_management.h"

void pkg_generate_asm(const char* output_file, PkgCtx* ctx) {
    FILE* f = fopen(output_file, "w");
    if (!f) return;
    fprintf(f, "; MLP Package Management\n\nsection .data\n");
    fprintf(f, "    msg: db \"Package Management OK!\", 10, 0\n");
    fprintf(f, "    len: equ $ - msg\n\n");
    fprintf(f, "section .text\n    global _start\n\n_start:\n");
    fprintf(f, "    mov rax, 1\n    mov rdi, 1\n");
    fprintf(f, "    mov rsi, msg\n    mov rdx, len\n    syscall\n");
    fprintf(f, "    mov rax, 60\n    xor rdi, rdi\n    syscall\n");
    fclose(f);
}
