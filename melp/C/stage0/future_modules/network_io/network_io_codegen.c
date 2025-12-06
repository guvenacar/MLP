#include "network_io.h"
void net_generate_asm(const char* output_file, NetCtx* ctx) {
    FILE* f = fopen(output_file, "w");
    if (!f) return;
    fprintf(f, "; Network IO\n\nsection .data\n    msg: db \"Network IO OK!\", 10, 0\n    len: equ $ - msg\n\n");
    fprintf(f, "section .text\n    global _start\n\n_start:\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, msg\n    mov rdx, len\n    syscall\n    mov rax, 60\n    xor rdi, rdi\n    syscall\n");
    fclose(f);
}
