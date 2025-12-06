#include "generator_codegen.h"
#include <string.h>

// Generator için kod üret (state machine)
void codegen_generator(FILE* out, Generator* gen) {
    fprintf(out, "    ; Generator: %s\n", gen->name);
    
    const char* name = get_gen_mangled_name(gen);
    
    fprintf(out, "\n%s:    ; Generator fonksiyonu\n", name);
    codegen_gen_prologue(out, gen);
    
    // State machine üret
    codegen_gen_state_machine(out, gen);
    
    codegen_gen_epilogue(out, gen);
}

// Yield için kod üret
void codegen_yield(FILE* out, Yield* yld) {
    fprintf(out, "    ; Yield #%d\n", yld->yield_id);
    
    // Değeri kaydet
    fprintf(out, "    mov rax, [rbp - 8]    ; Yield değeri\n");
    fprintf(out, "    mov [rbp - 16], rax    ; Context'e kaydet\n");
    
    // Durumu güncelle
    fprintf(out, "    mov dword [rbp - 24], %d    ; Resume point\n", 
            yld->yield_id + 1);
    fprintf(out, "    mov dword [rbp - 28], 2    ; State: SUSPENDED\n");
    
    // Context'i kaydet
    fprintf(out, "    call .gen_save_context\n");
    
    // Yield değeri ile dön
    fprintf(out, "    mov rax, [rbp - 16]    ; Return değeri\n");
    fprintf(out, "    jmp .gen_return\n");
    
    // Resume noktası
    codegen_yield_point(out, yld->yield_id + 1);
}

// Generator çağrısı için kod üret
void codegen_gen_call(FILE* out, GenCall* call) {
    fprintf(out, "    ; Generator çağrısı\n");
    
    if (call->next_call) {
        // next() çağrısı
        fprintf(out, "    mov rdi, [rbp - 8]    ; Generator pointer\n");
        
        const char* name = get_gen_mangled_name(call->generator);
        fprintf(out, "    call %s    ; Generator'ı devam ettir\n", name);
        
        // Dönüş değeri rax'te
        fprintf(out, "    ; rax = yield edilen değer\n");
    }
}

// State machine üret
void codegen_gen_state_machine(FILE* out, Generator* gen) {
    fprintf(out, "    ; State machine dispatch\n");
    
    // Resume point'e göre jump
    fprintf(out, "    mov eax, [rbp - 24]    ; Resume point\n");
    fprintf(out, "    cmp eax, 0\n");
    fprintf(out, "    je .gen_start    ; İlk çağrı\n");
    
    // Her yield noktası için case
    for (int i = 1; i <= gen->yield_count; i++) {
        fprintf(out, "    cmp eax, %d\n", i);
        fprintf(out, "    je .yield_point_%d\n", i);
    }
    
    fprintf(out, "    jmp .gen_finished    ; Bilinmeyen durum\n\n");
    
    // Başlangıç noktası
    fprintf(out, ".gen_start:\n");
    fprintf(out, "    ; Generator gövdesi\n");
    // Body burada üretilecek
    
    // Bitmiş durumu
    fprintf(out, ".gen_finished:\n");
    fprintf(out, "    mov dword [rbp - 28], 3    ; State: FINISHED\n");
    fprintf(out, "    xor rax, rax    ; NULL dön\n");
    fprintf(out, "    jmp .gen_return\n");
}

// Context kaydetme
void codegen_gen_context_save(FILE* out, Generator* gen) {
    fprintf(out, ".gen_save_context:\n");
    fprintf(out, "    ; Yerel değişkenleri kaydet\n");
    
    // Yerel değişkenleri heap'e kopyala
    fprintf(out, "    mov rdi, [rbp - 32]    ; Context pointer\n");
    fprintf(out, "    lea rsi, [rbp - 64]    ; Local vars başlangıcı\n");
    fprintf(out, "    mov rdx, 32    ; Boyut (örnek)\n");
    fprintf(out, "    call memcpy\n");
    
    fprintf(out, "    ret\n\n");
}

// Context geri yükleme
void codegen_gen_context_restore(FILE* out, Generator* gen) {
    fprintf(out, ".gen_restore_context:\n");
    fprintf(out, "    ; Yerel değişkenleri geri yükle\n");
    
    // Heap'ten yerel değişkenlere kopyala
    fprintf(out, "    lea rdi, [rbp - 64]    ; Local vars hedefi\n");
    fprintf(out, "    mov rsi, [rbp - 32]    ; Context pointer\n");
    fprintf(out, "    mov rdx, 32    ; Boyut\n");
    fprintf(out, "    call memcpy\n");
    
    fprintf(out, "    ret\n\n");
}

// Generator prologue
void codegen_gen_prologue(FILE* out, Generator* gen) {
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    fprintf(out, "    sub rsp, 128    ; Local space + context\n");
    
    // Context pointer'ı al (ilk parametre)
    fprintf(out, "    mov [rbp - 32], rdi    ; Context pointer\n");
    
    // Context'ten durumu yükle
    fprintf(out, "    mov eax, [rdi]    ; State\n");
    fprintf(out, "    mov [rbp - 28], eax\n");
    fprintf(out, "    mov eax, [rdi + 4]    ; Resume point\n");
    fprintf(out, "    mov [rbp - 24], eax\n");
    
    // Context'i geri yükle
    fprintf(out, "    call .gen_restore_context\n");
}

// Generator epilogue
void codegen_gen_epilogue(FILE* out, Generator* gen) {
    fprintf(out, ".gen_return:\n");
    
    // Context'i güncelle
    fprintf(out, "    mov rdi, [rbp - 32]    ; Context\n");
    fprintf(out, "    mov eax, [rbp - 28]    ; State\n");
    fprintf(out, "    mov [rdi], eax\n");
    fprintf(out, "    mov eax, [rbp - 24]    ; Resume point\n");
    fprintf(out, "    mov [rdi + 4], eax\n");
    
    fprintf(out, "    mov rsp, rbp\n");
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n\n");
}

// Yield noktası etiketi
void codegen_yield_point(FILE* out, int yield_id) {
    fprintf(out, ".yield_point_%d:\n", yield_id);
    fprintf(out, "    ; Resume noktası %d\n", yield_id);
}

// Generator için mangled isim
const char* get_gen_mangled_name(Generator* gen) {
    static char buffer[128];
    snprintf(buffer, sizeof(buffer), ".gen_%s", gen->name);
    return buffer;
}
