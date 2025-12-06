/**
 * MLP Error Propagation Module - Code Generator #71
 */

#include "error_propagation.h"

void errorprop_generate_asm(const char* output_file, ErrorPropCtx* ctx) {
    FILE* f = fopen(output_file, "w");
    if (!f) {
        fprintf(stderr, "Error: Cannot write to %s\n", output_file);
        return;
    }

    fprintf(f, "; MLP Error Propagation Module Assembly Output\n\n");

    fprintf(f, "section .data\n");
    fprintf(f, "    msg_ok: db \"Error Propagation OK!\", 10, 0\n");
    fprintf(f, "    msg_ok_len: equ $ - msg_ok\n\n");
    
    fprintf(f, "    ; Question operators: %d\n", ctx->question_count);
    for (int i = 0; i < ctx->question_count; i++) {
        QuestionOp* op = &ctx->question_ops[i];
        const char* type_str = op->type == QUESTION_RESULT ? "Result" :
                               op->type == QUESTION_OPTION ? "Option" : "Custom";
        fprintf(f, "    ; %s? [%s]\n", op->expr, type_str);
    }
    fprintf(f, "\n");
    
    fprintf(f, "    ; Try blocks: %d\n", ctx->try_count);
    fprintf(f, "\n");

    fprintf(f, "section .bss\n");
    fprintf(f, "    error_state: resq 1\n");
    fprintf(f, "    error_value: resq 1\n\n");

    fprintf(f, "section .text\n");
    fprintf(f, "    global _start\n\n");

    fprintf(f, "_start:\n");
    fprintf(f, "    ; Error propagation operations\n\n");
    
    // Generate ? operators
    for (int i = 0; i < ctx->question_count; i++) {
        QuestionOp* op = &ctx->question_ops[i];
        fprintf(f, "    ; %s?\n", op->expr);
        fprintf(f, "    call evaluate_%d    ; Evaluate expression\n", i);
        fprintf(f, "    test rax, rax       ; Check tag (0=Err/None)\n");
        fprintf(f, "    jz .error_return_%d\n", i);
        fprintf(f, "    ; Success path - extract value\n");
        fprintf(f, "    mov rbx, [rax + 8]  ; Get inner value\n");
        if (strlen(op->result_var) > 0) {
            fprintf(f, "    mov [%s], rbx\n", op->result_var);
        }
        fprintf(f, "    jmp .continue_%d\n", i);
        fprintf(f, ".error_return_%d:\n", i);
        fprintf(f, "    ; Early return with error\n");
        fprintf(f, "    mov rax, 0          ; Return error\n");
        fprintf(f, "    ret\n");
        fprintf(f, ".continue_%d:\n\n", i);
    }
    
    // Generate try blocks
    for (int i = 0; i < ctx->try_count; i++) {
        TryBlock* block = &ctx->try_blocks[i];
        fprintf(f, "    ; try block %d\n", i);
        fprintf(f, ".try_%d:\n", i);
        fprintf(f, "    ; Try body\n");
        fprintf(f, "    ; ...\n");
        if (block->has_catch) {
            fprintf(f, "    jmp .try_%d_end\n", i);
            fprintf(f, ".catch_%d:\n", i);
            fprintf(f, "    ; Catch block (var: %s)\n", block->catch_var);
            fprintf(f, "    ; ...\n");
        }
        fprintf(f, ".try_%d_end:\n\n", i);
    }
    
    fprintf(f, "    ; Print success\n");
    fprintf(f, "    mov rax, 1\n");
    fprintf(f, "    mov rdi, 1\n");
    fprintf(f, "    mov rsi, msg_ok\n");
    fprintf(f, "    mov rdx, msg_ok_len\n");
    fprintf(f, "    syscall\n\n");

    fprintf(f, "    mov rax, 60\n");
    fprintf(f, "    xor rdi, rdi\n");
    fprintf(f, "    syscall\n\n");

    // Generate evaluate functions (stubs)
    for (int i = 0; i < ctx->question_count; i++) {
        fprintf(f, "evaluate_%d:\n", i);
        fprintf(f, "    ; Evaluate: %s\n", ctx->question_ops[i].expr);
        fprintf(f, "    mov rax, 1    ; Simulate success\n");
        fprintf(f, "    ret\n\n");
    }

    fclose(f);
}
