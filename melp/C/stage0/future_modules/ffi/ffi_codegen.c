/**
 * MLP FFI Module - Code Generator #43 (Stage 1)
 */

#include "ffi.h"

void ffi_generate_asm(const char* output_file, FFICtx* ctx) {
    FILE* f = fopen(output_file, "w");
    if (!f) return;

    fprintf(f, "; MLP FFI Module Assembly\n\n");
    fprintf(f, "section .data\n");
    fprintf(f, "    msg_ok: db \"FFI OK!\", 10, 0\n");
    fprintf(f, "    msg_ok_len: equ $ - msg_ok\n\n");
    
    fprintf(f, "    ; Extern functions: %d\n", ctx->extern_fn_count);
    fprintf(f, "    ; Link directives: %d\n", ctx->link_count);
    fprintf(f, "    ; Unsafe blocks: %d\n", ctx->unsafe_block_count);
    fprintf(f, "    ; Raw pointers: %d\n\n", ctx->raw_ptr_count);

    fprintf(f, "section .text\n");
    fprintf(f, "    global _start\n\n");
    fprintf(f, "_start:\n");
    
    for (int i = 0; i < ctx->op_count; i++) {
        fprintf(f, "    ; Op %d: %s\n", i, ctx->ops[i].name);
    }
    
    fprintf(f, "\n    mov rax, 1\n");
    fprintf(f, "    mov rdi, 1\n");
    fprintf(f, "    mov rsi, msg_ok\n");
    fprintf(f, "    mov rdx, msg_ok_len\n");
    fprintf(f, "    syscall\n\n");
    fprintf(f, "    mov rax, 60\n");
    fprintf(f, "    xor rdi, rdi\n");
    fprintf(f, "    syscall\n");

    fclose(f);
}
