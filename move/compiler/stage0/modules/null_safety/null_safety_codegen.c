#include "null_safety_codegen.h"
#include <string.h>

// Optional için kod üret
void codegen_optional(FILE* out, Optional* opt) {
    fprintf(out, "    ; Optional<%s>\n", opt->type_name);
    
    if (opt->state == OPT_SOME) {
        fprintf(out, "    ; Some değer\n");
        fprintf(out, "    mov byte [rbp - 8], 1    ; state = SOME\n");
        fprintf(out, "    mov rax, [rbp - 16]      ; value\n");
        fprintf(out, "    mov [rbp - 24], rax      ; optional.value\n");
    } else {
        fprintf(out, "    ; None\n");
        fprintf(out, "    mov byte [rbp - 8], 0    ; state = NONE\n");
        fprintf(out, "    xor rax, rax\n");
        fprintf(out, "    mov [rbp - 24], rax      ; optional.value = NULL\n");
    }
}

// Null kontrol için kod üret
void codegen_null_safety_check(FILE* out, NullCheck* check) {
    fprintf(out, "    ; Null kontrol\n");
    
    // İfadeyi değerlendir
    fprintf(out, "    mov rax, [rbp - 8]    ; İfade değeri\n");
    
    switch (check->type) {
        case CHECK_IS_NULL:
            fprintf(out, "    test rax, rax\n");
            fprintf(out, "    setz al              ; al = (rax == 0)\n");
            break;
            
        case CHECK_IS_NOT_NULL:
            fprintf(out, "    test rax, rax\n");
            fprintf(out, "    setnz al             ; al = (rax != 0)\n");
            break;
            
        case CHECK_HAS_VALUE:
            fprintf(out, "    mov al, [rax]        ; Optional.state\n");
            fprintf(out, "    cmp al, 1            ; state == SOME\n");
            fprintf(out, "    sete al\n");
            break;
    }
    
    fprintf(out, "    movzx rax, al\n");
    fprintf(out, "    mov [rbp - 16], rax  ; Sonuç\n");
}

// Null-coalescing için kod üret
void codegen_null_coalesce(FILE* out, NullCoalesce* coal) {
    fprintf(out, "    ; Null-coalescing (??)\n");
    
    // Sol operandı değerlendir
    fprintf(out, "    mov rax, [rbp - 8]    ; Sol operand\n");
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jnz .coal_use_left    ; NULL değilse sol'u kullan\n");
    
    // Sol NULL ise sağı kullan
    fprintf(out, "    mov rax, [rbp - 16]   ; Sağ operand (fallback)\n");
    fprintf(out, "    jmp .coal_done\n");
    
    fprintf(out, ".coal_use_left:\n");
    fprintf(out, "    ; Sol değeri kullan (zaten rax'te)\n");
    
    fprintf(out, ".coal_done:\n");
    fprintf(out, "    mov [rbp - 24], rax   ; Sonuç\n");
}

// Safe navigation için kod üret
void codegen_safe_nav(FILE* out, SafeNav* nav) {
    fprintf(out, "    ; Safe navigation (?.)\n");
    
    // Nesneyi kontrol et
    fprintf(out, "    mov rax, [rbp - 8]    ; Nesne pointer\n");
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jz .safe_nav_null     ; NULL ise\n");
    
    // NULL değilse üyeye eriş
    fprintf(out, "    ; Üye: %s\n", nav->member);
    fprintf(out, "    mov rax, [rax + 0]    ; member offset (örnek)\n");
    fprintf(out, "    jmp .safe_nav_done\n");
    
    fprintf(out, ".safe_nav_null:\n");
    fprintf(out, "    xor rax, rax          ; NULL dön\n");
    
    fprintf(out, ".safe_nav_done:\n");
    fprintf(out, "    mov [rbp - 16], rax   ; Sonuç\n");
}

// Null assertion için kod üret
void codegen_null_assert(FILE* out, NullAssert* assert) {
    fprintf(out, "    ; Null assertion (!)\n");
    
    // İfadeyi değerlendir
    fprintf(out, "    mov rax, [rbp - 8]    ; İfade değeri\n");
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jnz .assert_ok        ; NULL değilse OK\n");
    
    // NULL ise panic
    codegen_panic_on_null(out, assert->message);
    
    fprintf(out, ".assert_ok:\n");
    fprintf(out, "    ; Değer rax'te\n");
}

// Optional oluştur
void codegen_optional_create(FILE* out, void* value, const char* type) {
    fprintf(out, "    ; Optional<%s> oluştur\n", type);
    fprintf(out, "    mov byte [rbp - 8], 1    ; state = SOME\n");
    fprintf(out, "    mov rax, [rbp - 16]      ; value\n");
    fprintf(out, "    mov [rbp - 24], rax\n");
}

// Optional.get()
void codegen_optional_get(FILE* out, Optional* opt) {
    fprintf(out, "    ; Optional.get()\n");
    fprintf(out, "    mov rax, [rbp - 8]       ; Optional pointer\n");
    fprintf(out, "    mov al, [rax]            ; state\n");
    fprintf(out, "    test al, al\n");
    fprintf(out, "    jz .opt_get_panic        ; None ise panic\n");
    
    fprintf(out, "    mov rax, [rbp - 8]\n");
    fprintf(out, "    mov rax, [rax + 8]       ; value\n");
    fprintf(out, "    jmp .opt_get_done\n");
    
    fprintf(out, ".opt_get_panic:\n");
    codegen_panic_on_null(out, "Optional.get() called on None");
    
    fprintf(out, ".opt_get_done:\n");
}

// Optional.has_value()
void codegen_optional_has_value(FILE* out, Optional* opt) {
    fprintf(out, "    ; Optional.has_value()\n");
    fprintf(out, "    mov rax, [rbp - 8]       ; Optional pointer\n");
    fprintf(out, "    mov al, [rax]            ; state\n");
    fprintf(out, "    movzx rax, al\n");
}

// Optional.get_or(default)
void codegen_optional_get_or(FILE* out, Optional* opt, void* default_val) {
    fprintf(out, "    ; Optional.get_or()\n");
    fprintf(out, "    mov rax, [rbp - 8]       ; Optional pointer\n");
    fprintf(out, "    mov al, [rax]            ; state\n");
    fprintf(out, "    test al, al\n");
    fprintf(out, "    jnz .opt_has_value       ; Some ise value al\n");
    
    fprintf(out, "    mov rax, [rbp - 16]      ; Default value\n");
    fprintf(out, "    jmp .opt_get_or_done\n");
    
    fprintf(out, ".opt_has_value:\n");
    fprintf(out, "    mov rax, [rbp - 8]\n");
    fprintf(out, "    mov rax, [rax + 8]       ; value\n");
    
    fprintf(out, ".opt_get_or_done:\n");
}

// NULL kontrol ifadesi
void codegen_null_safety_check_expr(FILE* out, void* expr) {
    fprintf(out, "    ; Null check expression\n");
    fprintf(out, "    mov rax, [rbp - 8]\n");
    fprintf(out, "    test rax, rax\n");
}

// Panic on null
void codegen_panic_on_null(FILE* out, const char* message) {
    fprintf(out, "    ; Panic: %s\n", message);
    fprintf(out, "    lea rdi, [rel .panic_msg]\n");
    fprintf(out, "    call puts\n");
    fprintf(out, "    mov rdi, 1\n");
    fprintf(out, "    call exit\n");
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".panic_msg: db \"%s\", 0\n", message);
    fprintf(out, "section .text\n");
}
