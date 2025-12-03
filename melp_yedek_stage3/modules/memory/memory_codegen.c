#include "memory_codegen.h"

// Bellek ayırma için kod üret
void codegen_memory_alloc(FILE* out, MemoryAlloc* alloc) {
    fprintf(out, "    ; Bellek ayırma: %s (%zu byte)\n", 
            alloc->var_name, alloc->size);
    
    switch (alloc->strategy) {
        case ALLOC_STACK:
            // Stack üzerinde ayır (hızlı)
            fprintf(out, "    sub rsp, %zu    ; Stack'te %s\n", 
                    alloc->size, alloc->var_name);
            fprintf(out, "    lea rax, [rbp - %zu]    ; Adres\n", alloc->size);
            break;
            
        case ALLOC_HEAP:
            // Heap üzerinde ayır (malloc çağrısı)
            fprintf(out, "    mov rdi, %zu    ; Boyut\n", alloc->size);
            fprintf(out, "    call malloc    ; Heap ayırma\n");
            fprintf(out, "    ; rax'te yeni bellek adresi\n");
            
            // NULL kontrolü
            fprintf(out, "    test rax, rax\n");
            fprintf(out, "    jz .alloc_failed    ; Ayırma başarısız\n");
            break;
            
        case ALLOC_STATIC:
            // Static bellek (data/bss segment'te)
            fprintf(out, "    ; Static ayırma: .bss segment\n");
            fprintf(out, "section .bss\n");
            fprintf(out, "    %s: resb %zu\n", alloc->var_name, alloc->size);
            fprintf(out, "section .text\n");
            break;
    }
}

// Bellek serbest bırakma için kod üret
void codegen_memory_free(FILE* out, MemoryFree* free_op) {
    fprintf(out, "    ; Bellek serbest bırakma: %s\n", free_op->var_name);
    
    // NULL kontrolü
    fprintf(out, "    mov rdi, [rbp - 8]    ; %s adresi\n", free_op->var_name);
    fprintf(out, "    test rdi, rdi\n");
    fprintf(out, "    jz .free_skip    ; NULL ise atla\n");
    
    // free() çağrısı
    fprintf(out, "    call free    ; Belleği serbest bırak\n");
    
    // Pointer'ı NULL yap (dangling pointer önleme)
    fprintf(out, "    mov qword [rbp - 8], 0    ; NULL yap\n");
    fprintf(out, ".free_skip:\n");
}

// Bellek kopyalama için kod üret
void codegen_memory_copy(FILE* out, MemoryCopy* copy) {
    fprintf(out, "    ; Bellek kopyalama: %s <- %s (%zu byte)\n",
            copy->dest_var, copy->src_var, copy->size);
    
    // memcpy benzeri: küçük bloklar için inline, büyük için çağrı
    if (copy->size <= 64) {
        // Inline kopyalama (küçük boyutlar için)
        fprintf(out, "    mov rsi, [rbp - 16]    ; Kaynak\n");
        fprintf(out, "    mov rdi, [rbp - 8]     ; Hedef\n");
        fprintf(out, "    mov rcx, %zu    ; Boyut\n", copy->size);
        fprintf(out, "    rep movsb    ; Byte-byte kopyala\n");
    } else {
        // memcpy çağrısı (büyük boyutlar için)
        fprintf(out, "    mov rdi, [rbp - 8]     ; Hedef\n");
        fprintf(out, "    mov rsi, [rbp - 16]    ; Kaynak\n");
        fprintf(out, "    mov rdx, %zu    ; Boyut\n", copy->size);
        fprintf(out, "    call memcpy    ; C runtime memcpy\n");
    }
}

// Bellek sıfırlama için kod üret
void codegen_memory_zero(FILE* out, MemoryZero* zero) {
    fprintf(out, "    ; Bellek sıfırlama: %s (%zu byte)\n",
            zero->var_name, zero->size);
    
    if (zero->size <= 64) {
        // Inline sıfırlama
        fprintf(out, "    mov rdi, [rbp - 8]    ; Adres\n");
        fprintf(out, "    xor rax, rax    ; Sıfır\n");
        fprintf(out, "    mov rcx, %zu    ; Sayaç\n", zero->size);
        fprintf(out, "    rep stosb    ; Byte-byte sıfırla\n");
    } else {
        // memset çağrısı
        fprintf(out, "    mov rdi, [rbp - 8]    ; Adres\n");
        fprintf(out, "    xor rsi, rsi    ; Değer (0)\n");
        fprintf(out, "    mov rdx, %zu    ; Boyut\n", zero->size);
        fprintf(out, "    call memset    ; C runtime memset\n");
    }
}

// Bellek yeniden boyutlandırma için kod üret
void codegen_memory_realloc(FILE* out, MemoryRealloc* realloc_op) {
    fprintf(out, "    ; Bellek yeniden boyutlandırma: %s (%zu -> %zu)\n",
            realloc_op->var_name, realloc_op->old_size, realloc_op->new_size);
    
    // realloc çağrısı
    fprintf(out, "    mov rdi, [rbp - 8]    ; Eski adres\n");
    fprintf(out, "    mov rsi, %zu    ; Yeni boyut\n", realloc_op->new_size);
    fprintf(out, "    call realloc    ; Yeniden ayır\n");
    
    // NULL kontrolü
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jz .realloc_failed    ; Başarısız\n");
    
    // Yeni adresi kaydet
    fprintf(out, "    mov [rbp - 8], rax    ; Yeni adres\n");
}

// malloc çağrısı için yardımcı
void codegen_malloc_call(FILE* out, size_t size) {
    fprintf(out, "    mov rdi, %zu    ; Boyut\n", size);
    fprintf(out, "    call malloc\n");
}

// free çağrısı için yardımcı
void codegen_free_call(FILE* out, const char* var_name) {
    fprintf(out, "    mov rdi, [rbp - 8]    ; %s\n", var_name);
    fprintf(out, "    call free\n");
}

// memset çağrısı için yardımcı
void codegen_memset_call(FILE* out, const char* var_name, int value, size_t size) {
    fprintf(out, "    mov rdi, [rbp - 8]    ; %s\n", var_name);
    fprintf(out, "    mov rsi, %d    ; Değer\n", value);
    fprintf(out, "    mov rdx, %zu    ; Boyut\n", size);
    fprintf(out, "    call memset\n");
}
