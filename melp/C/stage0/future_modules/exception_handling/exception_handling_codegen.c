#include "exception_handling_codegen.h"
#include <string.h>

// Try-catch-finally için kod üret
void codegen_try_catch_finally(FILE* out, TryCatchFinally* tcf) {
    fprintf(out, "    ; Try-catch-finally bloğu\n");
    
    // Setup setjmp
    codegen_try_setup(out);
    
    // Try bloğu
    fprintf(out, ".try_block:\n");
    fprintf(out, "    ; Try bloğu kodu\n");
    // tcf->try_block burada üretilecek
    
    fprintf(out, "    jmp .try_success    ; Exception yok, finally'ye geç\n");
    
    // Catch dispatch
    fprintf(out, ".catch_dispatch:\n");
    codegen_catch_dispatch(out, tcf);
    
    // Success path
    fprintf(out, ".try_success:\n");
    
    // Finally bloğu
    if (tcf->finally_block) {
        fprintf(out, ".finally_block:\n");
        codegen_finally_block(out, tcf->finally_block);
    }
    
    fprintf(out, "    jmp .try_end\n");
    
    // Unhandled exception
    fprintf(out, ".unhandled_exception:\n");
    codegen_unhandled_exception(out);
    
    fprintf(out, ".try_end:\n");
    fprintf(out, "    ; Try-catch-finally sonu\n");
}

// Catch bloğu için kod üret
void codegen_catch_block(FILE* out, CatchBlock* cb, int index) {
    fprintf(out, ".catch_%d:\n", index);
    fprintf(out, "    ; Catch: %s\n", cb->exception_type);
    
    // Exception tipini kontrol et
    if (strcmp(cb->exception_type, "*") != 0) {
        codegen_exception_match(out, cb->exception_type);
        fprintf(out, "    test rax, rax\n");
        fprintf(out, "    jz .catch_%d_next    ; Tip eşleşmedi\n", index);
    }
    
    // Exception değişkenine ata
    if (cb->var_name) {
        fprintf(out, "    ; Exception değişkeni: %s\n", cb->var_name);
        fprintf(out, "    mov rax, [rel .current_exception]\n");
        fprintf(out, "    mov [rbp - 8], rax    ; %s\n", cb->var_name);
    }
    
    // Handler kodu
    fprintf(out, "    ; Catch handler\n");
    // cb->handler burada üretilecek
    
    fprintf(out, "    jmp .finally_block    ; Finally'ye geç\n");
    fprintf(out, ".catch_%d_next:\n", index);
}

// Throw için kod üret
void codegen_throw(FILE* out, Throw* thr) {
    fprintf(out, "    ; Throw exception\n");
    
    // Exception oluştur
    codegen_exception_create(out, thr->exception);
    
    // Current exception'ı güncelle
    fprintf(out, "    mov [rel .current_exception], rax\n");
    
    // longjmp ile catch'e atla
    fprintf(out, "    mov rdi, [rel .jump_buffer]\n");
    fprintf(out, "    mov rsi, 1    ; Exception value\n");
    codegen_longjmp(out, 1);
}

// Exception oluştur
void codegen_exception_create(FILE* out, Exception* exc) {
    fprintf(out, "    ; Exception oluştur: %s\n", exc->type);
    
    // malloc ile exception allocate et
    fprintf(out, "    mov rdi, 64    ; Exception struct size\n");
    fprintf(out, "    call malloc\n");
    fprintf(out, "    mov rbx, rax    ; Exception pointer\n");
    
    // Type string
    fprintf(out, "    lea rdi, [rel .exc_type_%s]\n", exc->type);
    fprintf(out, "    mov [rbx], rdi    ; exc->type\n");
    
    // Message string
    fprintf(out, "    lea rdi, [rel .exc_msg]\n");
    fprintf(out, "    mov [rbx + 8], rdi    ; exc->message\n");
    
    fprintf(out, "    mov rax, rbx    ; Return exception\n");
}

// Try setup (setjmp)
void codegen_try_setup(FILE* out) {
    fprintf(out, "    ; Try setup (setjmp)\n");
    fprintf(out, "    lea rdi, [rel .jump_buffer]\n");
    codegen_setjmp(out);
    
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jnz .catch_dispatch    ; Exception yakalandı\n");
}

// Catch dispatch (exception type matching)
void codegen_catch_dispatch(FILE* out, TryCatchFinally* tcf) {
    fprintf(out, "    ; Catch dispatch\n");
    
    CatchBlock** blocks = (CatchBlock**)tcf->catch_blocks;
    
    for (int i = 0; i < tcf->catch_count; i++) {
        codegen_catch_block(out, blocks[i], i);
    }
    
    // Hiçbir catch eşleşmedi
    fprintf(out, "    jmp .unhandled_exception\n");
}

// Finally bloğu
void codegen_finally_block(FILE* out, void* finally) {
    fprintf(out, "    ; Finally bloğu\n");
    fprintf(out, "    ; Finally kodu buraya\n");
    
    // Cleanup
    codegen_exception_cleanup(out);
}

// Exception cleanup
void codegen_exception_cleanup(FILE* out) {
    fprintf(out, "    ; Exception cleanup\n");
    fprintf(out, "    mov rax, [rel .current_exception]\n");
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jz .cleanup_done\n");
    
    // Free exception
    fprintf(out, "    mov rdi, rax\n");
    fprintf(out, "    call free\n");
    fprintf(out, "    mov qword [rel .current_exception], 0\n");
    
    fprintf(out, ".cleanup_done:\n");
}

// Exception tip eşleşmesi
void codegen_exception_match(FILE* out, const char* type) {
    fprintf(out, "    ; Exception match: %s\n", type);
    fprintf(out, "    mov rax, [rel .current_exception]\n");
    fprintf(out, "    mov rdi, [rax]    ; exc->type\n");
    fprintf(out, "    lea rsi, [rel .exc_type_%s]\n", type);
    fprintf(out, "    call strcmp\n");
    fprintf(out, "    test rax, rax    ; 0 = eşleşti\n");
    fprintf(out, "    setz al\n");
    fprintf(out, "    movzx rax, al\n");
}

// Exception rethrow
void codegen_exception_rethrow(FILE* out) {
    fprintf(out, "    ; Rethrow exception\n");
    fprintf(out, "    mov rdi, [rel .jump_buffer]\n");
    fprintf(out, "    mov rsi, 1\n");
    codegen_longjmp(out, 1);
}

// Unhandled exception
void codegen_unhandled_exception(FILE* out) {
    fprintf(out, "    ; Unhandled exception\n");
    fprintf(out, "    mov rax, [rel .current_exception]\n");
    fprintf(out, "    mov rdi, rax\n");
    fprintf(out, "    call exception_print    ; exception_print(exc)\n");
    fprintf(out, "    mov rdi, 1\n");
    fprintf(out, "    call exit\n");
}

// setjmp (C runtime)
void codegen_setjmp(FILE* out) {
    fprintf(out, "    call setjmp    ; int setjmp(jmp_buf)\n");
}

// longjmp (C runtime)
void codegen_longjmp(FILE* out, int value) {
    fprintf(out, "    call longjmp    ; void longjmp(jmp_buf, int)\n");
}
