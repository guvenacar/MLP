#include "async_codegen.h"
#include <stdio.h>
#include <stdlib.h>

// Async function codegen
void codegen_async_function(AsyncFunction* func) {
    if (!func) return;
    
    // Async function için state machine assembly üret
    printf("; Async function: %s\n", func->name ? func->name : "anonymous");
    printf("section .data\n");
    printf("    .async_state_%s: db 0  ; 0=initial, 1=suspended, 2=completed\n", 
           func->name ? func->name : "anon");
    printf("    .async_ctx_%s: times 256 db 0  ; Context save area\n",
           func->name ? func->name : "anon");
    printf("\n");
    
    printf("section .text\n");
    printf("%s:\n", func->name ? func->name : ".async_anon");
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    \n");
    
    // Future allocation
    printf("    ; Future allocation\n");
    printf("    call future_create\n");
    printf("    mov [rbp - 8], rax  ; Store future pointer\n");
    printf("    \n");
    
    // State machine için suspend/resume point'ler
    printf("    ; Check state\n");
    printf("    mov al, [.async_state_%s]\n", func->name ? func->name : "anon");
    printf("    cmp al, 0\n");
    printf("    je .initial\n");
    printf("    cmp al, 1\n");
    printf("    je .resume\n");
    printf("    jmp .done\n");
    printf("    \n");
    
    printf(".initial:\n");
    printf("    ; Initial execution\n");
    printf("    ; Function body will be here\n");
    printf("    \n");
    
    printf(".resume:\n");
    printf("    ; Resume from suspend point\n");
    printf("    ; Restore context\n");
    printf("    lea rsi, [.async_ctx_%s]\n", func->name ? func->name : "anon");
    printf("    mov rdi, rsp\n");
    printf("    mov rcx, 256\n");
    printf("    rep movsb\n");
    printf("    \n");
    
    printf(".done:\n");
    printf("    ; Set future value\n");
    printf("    mov rdi, [rbp - 8]  ; Future pointer\n");
    printf("    mov rsi, rax        ; Return value\n");
    printf("    call future_set_value\n");
    printf("    \n");
    printf("    ; Mark as completed\n");
    printf("    mov byte [.async_state_%s], 2\n", func->name ? func->name : "anon");
    printf("    \n");
    printf("    mov rax, [rbp - 8]  ; Return future\n");
    printf("    pop rbp\n");
    printf("    ret\n");
    printf("\n");
}

// Await expression codegen
void codegen_await_expr(AwaitExpr* expr) {
    if (!expr) return;
    
    printf("; Await expression\n");
    
    switch (expr->type) {
        case AWAIT_FUTURE:
            printf("    ; await future\n");
            printf("    mov rdi, [rbp - 8]  ; Future pointer\n");
            printf("    call future_is_ready\n");
            printf("    test al, al\n");
            printf("    jnz .await_ready\n");
            printf("    \n");
            printf("    ; Suspend current task\n");
            printf("    ; Save context\n");
            printf("    mov rdi, rsp\n");
            printf("    lea rsi, [.async_ctx]\n");
            printf("    mov rcx, 256\n");
            printf("    rep movsb\n");
            printf("    \n");
            printf("    ; Mark as suspended\n");
            printf("    mov byte [.async_state], 1\n");
            printf("    \n");
            printf("    ; Return to event loop\n");
            printf("    pop rbp\n");
            printf("    ret\n");
            printf("    \n");
            printf(".await_ready:\n");
            printf("    ; Get future value\n");
            printf("    mov rdi, [rbp - 8]\n");
            printf("    call future_get_value\n");
            printf("    ; Value in rax\n");
            break;
            
        case AWAIT_PROMISE:
            printf("    ; await promise\n");
            printf("    mov rdi, [rbp - 8]  ; Promise pointer\n");
            printf("    call promise_get_future\n");
            printf("    mov [rbp - 16], rax  ; Store future\n");
            printf("    \n");
            printf("    ; Check if ready\n");
            printf("    mov rdi, rax\n");
            printf("    call future_is_ready\n");
            printf("    test al, al\n");
            printf("    jnz .await_ready\n");
            printf("    ; Suspend if not ready\n");
            break;
            
        case AWAIT_ALL:
            printf("    ; await_all\n");
            printf("    mov rcx, [rbp - 8]   ; Future count\n");
            printf("    lea rdi, [rbp - 16]  ; Future array\n");
            printf("    \n");
            printf(".await_all_loop:\n");
            printf("    push rcx\n");
            printf("    mov rdi, [rdi]\n");
            printf("    call future_is_ready\n");
            printf("    pop rcx\n");
            printf("    test al, al\n");
            printf("    jz .await_all_suspend  ; Not ready, suspend\n");
            printf("    add rdi, 8\n");
            printf("    loop .await_all_loop\n");
            printf("    ; All ready\n");
            printf("    jmp .await_all_done\n");
            printf("    \n");
            printf(".await_all_suspend:\n");
            printf("    ; Suspend task\n");
            printf("    mov byte [.async_state], 1\n");
            printf("    pop rbp\n");
            printf("    ret\n");
            printf("    \n");
            printf(".await_all_done:\n");
            printf("    ; All futures completed\n");
            break;
            
        case AWAIT_ANY:
            printf("    ; await_any\n");
            printf("    mov rcx, [rbp - 8]   ; Future count\n");
            printf("    lea rdi, [rbp - 16]  ; Future array\n");
            printf("    \n");
            printf(".await_any_loop:\n");
            printf("    push rcx\n");
            printf("    push rdi\n");
            printf("    mov rdi, [rdi]\n");
            printf("    call future_is_ready\n");
            printf("    pop rdi\n");
            printf("    pop rcx\n");
            printf("    test al, al\n");
            printf("    jnz .await_any_ready  ; One is ready\n");
            printf("    add rdi, 8\n");
            printf("    loop .await_any_loop\n");
            printf("    \n");
            printf("    ; None ready, suspend\n");
            printf("    mov byte [.async_state], 1\n");
            printf("    pop rbp\n");
            printf("    ret\n");
            printf("    \n");
            printf(".await_any_ready:\n");
            printf("    ; Get ready future value\n");
            printf("    mov rdi, [rdi]\n");
            printf("    call future_get_value\n");
            break;
            
        case AWAIT_RACE:
            printf("    ; await_race (same as await_any)\n");
            printf("    ; First completed future wins\n");
            break;
            
        default:
            break;
    }
    
    printf("\n");
}

// Future operations codegen
void codegen_future_ops(Future* future) {
    if (!future) return;
    
    printf("; Future operations\n");
    
    printf("    ; future_create\n");
    printf("    call future_create\n");
    printf("    mov [rbp - 8], rax\n");
    printf("    \n");
    
    printf("    ; future_set_value\n");
    printf("    mov rdi, [rbp - 8]   ; Future\n");
    printf("    mov rsi, [rbp - 16]  ; Value\n");
    printf("    call future_set_value\n");
    printf("    \n");
    
    printf("    ; future_get_value\n");
    printf("    mov rdi, [rbp - 8]\n");
    printf("    call future_get_value\n");
    printf("    ; Value in rax\n");
    printf("    \n");
    
    printf("    ; future.then(callback)\n");
    printf("    mov rdi, [rbp - 8]   ; Future\n");
    printf("    lea rsi, [.then_callback]\n");
    printf("    call future_then\n");
    printf("    \n");
    
    printf("    ; future.catch(callback)\n");
    printf("    mov rdi, [rbp - 8]   ; Future\n");
    printf("    lea rsi, [.catch_callback]\n");
    printf("    call future_catch\n");
    printf("    \n");
    
    printf("    ; future.finally(callback)\n");
    printf("    mov rdi, [rbp - 8]   ; Future\n");
    printf("    lea rsi, [.finally_callback]\n");
    printf("    call future_finally\n");
    printf("    \n");
    
    printf("    ; future_all\n");
    printf("    lea rdi, [rbp - 24]  ; Future array\n");
    printf("    mov rsi, 3           ; Count\n");
    printf("    call future_all\n");
    printf("    \n");
    
    printf("    ; future_any\n");
    printf("    lea rdi, [rbp - 24]\n");
    printf("    mov rsi, 3\n");
    printf("    call future_any\n");
    printf("    \n");
    
    printf("    ; future_race\n");
    printf("    lea rdi, [rbp - 24]\n");
    printf("    mov rsi, 3\n");
    printf("    call future_race\n");
    printf("\n");
}

// Promise operations codegen
void codegen_promise_ops(Promise* promise) {
    if (!promise) return;
    
    printf("; Promise operations\n");
    
    printf("    ; promise_create\n");
    printf("    call promise_create\n");
    printf("    mov [rbp - 8], rax\n");
    printf("    \n");
    
    printf("    ; promise_resolve\n");
    printf("    mov rdi, [rbp - 8]   ; Promise\n");
    printf("    mov rsi, [rbp - 16]  ; Value\n");
    printf("    call promise_resolve\n");
    printf("    \n");
    
    printf("    ; promise_reject\n");
    printf("    mov rdi, [rbp - 8]   ; Promise\n");
    printf("    mov rsi, [rbp - 16]  ; Error\n");
    printf("    call promise_reject\n");
    printf("    \n");
    
    printf("    ; promise_get_future\n");
    printf("    mov rdi, [rbp - 8]\n");
    printf("    call promise_get_future\n");
    printf("    mov [rbp - 24], rax  ; Store future\n");
    printf("\n");
}

// Task scheduling codegen
void codegen_task_schedule(Task* task) {
    if (!task) return;
    
    printf("; Task scheduling\n");
    
    printf("    ; task_create\n");
    printf("    mov rdi, [rbp - 8]   ; AsyncFunction\n");
    printf("    lea rsi, [rbp - 16]  ; Args\n");
    printf("    mov rdx, 2           ; Arg count\n");
    printf("    call task_create\n");
    printf("    mov [rbp - 24], rax  ; Store task\n");
    printf("    \n");
    
    printf("    ; executor_submit\n");
    printf("    mov rdi, [.executor] ; Executor\n");
    printf("    mov rsi, [rbp - 24]  ; Task\n");
    printf("    call executor_submit\n");
    printf("    \n");
    
    printf("    ; task_run\n");
    printf("    mov rdi, [rbp - 24]\n");
    printf("    call task_run\n");
    printf("    \n");
    
    printf("    ; event_loop_add_task\n");
    printf("    mov rdi, [.event_loop] ; EventLoop\n");
    printf("    mov rsi, [rbp - 24]    ; Task\n");
    printf("    call event_loop_add_task\n");
    printf("    \n");
    
    printf("    ; event_loop_tick\n");
    printf("    mov rdi, [.event_loop]\n");
    printf("    call event_loop_tick\n");
    printf("\n");
}

// Async runtime codegen
void codegen_async_runtime(AsyncRuntime* runtime) {
    if (!runtime) return;
    
    printf("; Async runtime\n");
    printf("section .data\n");
    printf("    .runtime: times 256 db 0  ; Runtime structure\n");
    printf("    .executor: times 128 db 0 ; Executor structure\n");
    printf("    .event_loop: times 128 db 0 ; EventLoop structure\n");
    printf("\n");
    
    printf("section .text\n");
    
    printf("    ; async_runtime_create\n");
    printf("    call async_runtime_create\n");
    printf("    mov [.runtime], rax\n");
    printf("    \n");
    
    printf("    ; async_runtime_init\n");
    printf("    mov rdi, [.runtime]\n");
    printf("    mov rsi, %d  ; EXECUTOR_THREAD_POOL\n", EXECUTOR_THREAD_POOL);
    printf("    call async_runtime_init\n");
    printf("    \n");
    
    printf("    ; async_runtime_spawn\n");
    printf("    mov rdi, [.runtime]      ; Runtime\n");
    printf("    mov rsi, [rbp - 8]       ; AsyncFunction\n");
    printf("    lea rdx, [rbp - 16]      ; Args\n");
    printf("    mov rcx, 2               ; Arg count\n");
    printf("    call async_runtime_spawn\n");
    printf("    mov [rbp - 24], rax      ; Store future\n");
    printf("    \n");
    
    printf("    ; async_runtime_block_on\n");
    printf("    mov rdi, [.runtime]\n");
    printf("    mov rsi, [rbp - 24]      ; Future\n");
    printf("    call async_runtime_block_on\n");
    printf("    \n");
    
    printf("    ; async_runtime_shutdown\n");
    printf("    mov rdi, [.runtime]\n");
    printf("    call async_runtime_shutdown\n");
    printf("    \n");
    
    printf("    ; async_runtime_free\n");
    printf("    mov rdi, [.runtime]\n");
    printf("    call async_runtime_free\n");
    printf("\n");
}
