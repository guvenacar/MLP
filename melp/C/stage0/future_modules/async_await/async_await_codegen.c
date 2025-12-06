/**
 * MLP Async/Await Module - Code Generator #70 (Stage 1)
 * 
 * Generates x86-64 assembly for async/await
 */

#include "async_await.h"

void asyncawait_generate_asm(const char* output_file, AsyncAwaitCtx* ctx) {
    FILE* f = fopen(output_file, "w");
    if (!f) {
        fprintf(stderr, "Error: Cannot write to %s\n", output_file);
        return;
    }

    fprintf(f, "; MLP Async/Await Module Assembly Output\n");
    fprintf(f, "; Generated from async_await_codegen.c\n\n");

    // DATA SECTION
    fprintf(f, "section .data\n");
    fprintf(f, "    msg_ok: db \"Async/Await OK!\", 10, 0\n");
    fprintf(f, "    msg_ok_len: equ $ - msg_ok\n\n");
    
    fprintf(f, "    ; Async functions: %d\n", ctx->async_fn_count);
    for (int i = 0; i < ctx->async_fn_count; i++) {
        AsyncFn* fn = &ctx->async_fns[i];
        fprintf(f, "    ; async fn %s(%s) -> %s\n", 
                fn->name, fn->params, fn->return_type);
    }
    fprintf(f, "\n");
    
    fprintf(f, "    ; Await expressions: %d\n", ctx->await_count);
    for (int i = 0; i < ctx->await_count; i++) {
        AwaitExpr* await = &ctx->awaits[i];
        fprintf(f, "    ; %s.await", await->expr);
        if (await->has_timeout) {
            fprintf(f, " (timeout: %dms)", await->timeout_ms);
        }
        fprintf(f, "\n");
    }
    fprintf(f, "\n");
    
    fprintf(f, "    ; Futures: %d\n", ctx->future_count);
    for (int i = 0; i < ctx->future_count; i++) {
        Future* fut = &ctx->futures[i];
        fprintf(f, "    ; Future<%s> %s\n", fut->inner_type, fut->name);
    }
    fprintf(f, "\n");
    
    fprintf(f, "    ; Tasks: %d\n", ctx->task_count);
    for (int i = 0; i < ctx->task_count; i++) {
        Task* task = &ctx->tasks[i];
        fprintf(f, "    ; spawn(%s)%s\n", task->async_fn,
                task->is_detached ? " [detached]" : "");
    }
    fprintf(f, "\n");

    // BSS SECTION
    fprintf(f, "section .bss\n");
    fprintf(f, "    ; Runtime storage for async/await\n");
    for (int i = 0; i < ctx->future_count; i++) {
        fprintf(f, "    future_%s: resq 4    ; [state, value, waker, context]\n",
                ctx->futures[i].name);
    }
    fprintf(f, "\n");

    // TEXT SECTION
    fprintf(f, "section .text\n");
    fprintf(f, "    global _start\n\n");

    // Generate async functions
    for (int i = 0; i < ctx->async_fn_count; i++) {
        AsyncFn* fn = &ctx->async_fns[i];
        if (fn->type == ASYNC_FN_DECL) {
            fprintf(f, "%s:\n", fn->name);
            fprintf(f, "    ; Async function implementation\n");
            fprintf(f, "    push rbp\n");
            fprintf(f, "    mov rbp, rsp\n");
            fprintf(f, "    sub rsp, 64    ; Local variables\n");
            fprintf(f, "\n");
            fprintf(f, "    ; Create future\n");
            fprintf(f, "    mov qword [rbp - 8], 0    ; state = PENDING\n");
            fprintf(f, "\n");
            fprintf(f, "    ; Function body here\n");
            fprintf(f, "    ; ...\n");
            fprintf(f, "\n");
            fprintf(f, "    ; Set future to READY\n");
            fprintf(f, "    mov qword [rbp - 8], 1\n");
            fprintf(f, "\n");
            fprintf(f, "    mov rsp, rbp\n");
            fprintf(f, "    pop rbp\n");
            fprintf(f, "    ret\n\n");
        }
    }

    // Main entry point
    fprintf(f, "_start:\n");
    fprintf(f, "    ; Async/await operations\n\n");
    
    // Generate await calls
    for (int i = 0; i < ctx->await_count; i++) {
        AwaitExpr* await = &ctx->awaits[i];
        fprintf(f, "    ; %s.await\n", await->expr);
        fprintf(f, "    call poll_future    ; Poll the future\n");
        fprintf(f, "    test rax, rax\n");
        fprintf(f, "    jz .await_%d_pending\n", i);
        fprintf(f, "    ; Future ready, result in rax\n");
        if (strlen(await->result_var) > 0) {
            fprintf(f, "    mov [%s], rax\n", await->result_var);
        }
        fprintf(f, "    jmp .await_%d_done\n", i);
        fprintf(f, ".await_%d_pending:\n", i);
        fprintf(f, "    ; Yield execution\n");
        fprintf(f, "    call yield_task\n");
        fprintf(f, ".await_%d_done:\n\n", i);
    }
    
    // Generate task spawns
    for (int i = 0; i < ctx->task_count; i++) {
        Task* task = &ctx->tasks[i];
        fprintf(f, "    ; spawn(%s)\n", task->async_fn);
        fprintf(f, "    lea rdi, [%s]\n", task->async_fn);
        fprintf(f, "    call task_spawn\n\n");
    }
    
    fprintf(f, "    ; Print success message\n");
    fprintf(f, "    mov rax, 1\n");
    fprintf(f, "    mov rdi, 1\n");
    fprintf(f, "    mov rsi, msg_ok\n");
    fprintf(f, "    mov rdx, msg_ok_len\n");
    fprintf(f, "    syscall\n\n");

    fprintf(f, "    ; Exit\n");
    fprintf(f, "    mov rax, 60\n");
    fprintf(f, "    xor rdi, rdi\n");
    fprintf(f, "    syscall\n\n");

    // Runtime functions
    fprintf(f, "poll_future:\n");
    fprintf(f, "    ; Poll future state\n");
    fprintf(f, "    mov rax, [rdi]    ; Get state\n");
    fprintf(f, "    cmp rax, 1        ; Check if READY\n");
    fprintf(f, "    je .ready\n");
    fprintf(f, "    xor rax, rax      ; Return 0 (pending)\n");
    fprintf(f, "    ret\n");
    fprintf(f, ".ready:\n");
    fprintf(f, "    mov rax, [rdi + 8]    ; Get value\n");
    fprintf(f, "    ret\n\n");

    fprintf(f, "yield_task:\n");
    fprintf(f, "    ; Yield to scheduler\n");
    fprintf(f, "    ret\n\n");

    fprintf(f, "task_spawn:\n");
    fprintf(f, "    ; Spawn new task\n");
    fprintf(f, "    call rdi    ; Execute function\n");
    fprintf(f, "    ret\n");

    fclose(f);
}
