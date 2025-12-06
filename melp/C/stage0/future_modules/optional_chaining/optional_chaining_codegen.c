/**
 * MLP Optional Chaining Module - Code Generator #72
 */

#include "optional_chaining.h"

void optchain_generate_asm(const char* output_file, OptChainCtx* ctx) {
    FILE* f = fopen(output_file, "w");
    if (!f) return;

    fprintf(f, "; MLP Optional Chaining Module\n\n");

    fprintf(f, "section .data\n");
    fprintf(f, "    msg_ok: db \"Optional Chaining OK!\", 10, 0\n");
    fprintf(f, "    msg_ok_len: equ $ - msg_ok\n");
    fprintf(f, "    null_value: dq 0\n\n");
    
    fprintf(f, "    ; Optional chains: %d\n", ctx->chain_count);
    for (int i = 0; i < ctx->chain_count; i++) {
        OptionalChain* chain = &ctx->chains[i];
        const char* type = chain->type == CHAIN_FIELD ? "field" :
                          chain->type == CHAIN_METHOD ? "method" : "index";
        fprintf(f, "    ; %s?.%s [%s]\n", chain->base, chain->accessor, type);
    }
    fprintf(f, "\n");

    fprintf(f, "section .text\n");
    fprintf(f, "    global _start\n\n");

    fprintf(f, "_start:\n");
    
    for (int i = 0; i < ctx->chain_count; i++) {
        OptionalChain* chain = &ctx->chains[i];
        fprintf(f, "    ; %s?.%s\n", chain->base, chain->accessor);
        fprintf(f, "    mov rax, [%s]     ; Load base\n", chain->base);
        fprintf(f, "    test rax, rax     ; Check null\n");
        fprintf(f, "    jz .chain_%d_null\n", i);
        
        switch (chain->type) {
            case CHAIN_FIELD:
                fprintf(f, "    mov rbx, [rax + offset_%s]\n", chain->accessor);
                break;
            case CHAIN_METHOD:
                fprintf(f, "    ; Call method %s(%s)\n", chain->accessor, chain->args);
                fprintf(f, "    call [rax + vtable_%s]\n", chain->accessor);
                break;
            case CHAIN_INDEX:
                fprintf(f, "    mov rcx, %s       ; Index\n", chain->accessor);
                fprintf(f, "    mov rbx, [rax + rcx*8]\n");
                break;
        }
        
        fprintf(f, "    jmp .chain_%d_done\n", i);
        fprintf(f, ".chain_%d_null:\n", i);
        fprintf(f, "    mov rbx, [null_value]  ; Return null\n");
        fprintf(f, ".chain_%d_done:\n\n", i);
    }
    
    fprintf(f, "    mov rax, 1\n");
    fprintf(f, "    mov rdi, 1\n");
    fprintf(f, "    mov rsi, msg_ok\n");
    fprintf(f, "    mov rdx, msg_ok_len\n");
    fprintf(f, "    syscall\n\n");

    fprintf(f, "    mov rax, 60\n");
    fprintf(f, "    xor rdi, rdi\n");
    fprintf(f, "    syscall\n");

    fclose(f);
}
