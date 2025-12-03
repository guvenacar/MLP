#include "lambda_codegen.h"

// Lambda için kod üret
void codegen_lambda(FILE* out, Lambda* lambda) {
    static int lambda_counter = 0;
    int lambda_id = lambda_counter++;
    
    fprintf(out, "    ; Lambda tanımı #%d\n", lambda_id);
    
    if (lambda->type == LAMBDA_CLOSURE) {
        // Closure ise struct oluştur
        codegen_closure_struct(out, lambda);
        codegen_closure_capture(out, lambda);
    }
    
    // Lambda fonksiyonunu üret
    codegen_lambda_function(out, lambda, lambda_id);
    
    // Lambda pointer'ını stack'e koy
    const char* name = get_lambda_mangled_name(lambda, lambda_id);
    fprintf(out, "    lea rax, [%s]    ; Lambda adresi\n", name);
    fprintf(out, "    mov [rbp - 8], rax    ; Stack'e kaydet\n");
}

// Lambda çağrısı için kod üret
void codegen_lambda_call(FILE* out, LambdaCall* call) {
    fprintf(out, "    ; Lambda çağrısı\n");
    
    // Argümanları hazırla (System V AMD64 ABI)
    const char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    
    for (int i = 0; i < call->arg_count && i < 6; i++) {
        fprintf(out, "    mov %s, [rbp - %d]    ; Argüman %d\n",
                arg_regs[i], (i + 1) * 8, i);
    }
    
    // Stack'te kalan argümanlar (6'dan fazla ise)
    if (call->arg_count > 6) {
        for (int i = 6; i < call->arg_count; i++) {
            fprintf(out, "    push qword [rbp - %d]    ; Stack argüman %d\n",
                    (i + 1) * 8, i);
        }
    }
    
    // Lambda'yı çağır
    fprintf(out, "    mov rax, [rbp - 8]    ; Lambda pointer\n");
    fprintf(out, "    call rax    ; Indirect call\n");
    
    // Stack temizliği (6'dan fazla argüman varsa)
    if (call->arg_count > 6) {
        int stack_cleanup = (call->arg_count - 6) * 8;
        fprintf(out, "    add rsp, %d    ; Stack temizle\n", stack_cleanup);
    }
}

// Closure struct'ı için kod üret
void codegen_closure_struct(FILE* out, Lambda* lambda) {
    fprintf(out, "    ; Closure struct (%zu byte)\n", lambda->closure_size);
    
    // Heap'te yer ayır
    fprintf(out, "    mov rdi, %zu    ; Closure boyutu\n", lambda->closure_size);
    fprintf(out, "    call malloc\n");
    fprintf(out, "    mov [rbp - 16], rax    ; Closure pointer\n");
}

// Capture işlemi için kod üret
void codegen_closure_capture(FILE* out, Lambda* lambda) {
    fprintf(out, "    ; Değişkenleri capture et\n");
    
    CapturedVar* var = lambda->captures;
    while (var != NULL) {
        fprintf(out, "    ; Capture: %s (mod: %d, offset: %zu)\n",
                var->name, var->mode, var->offset);
        
        // Closure pointer'ı al
        fprintf(out, "    mov rbx, [rbp - 16]    ; Closure base\n");
        fprintf(out, "    add rbx, %zu    ; Offset\n", var->offset);
        
        if (var->mode == CAPTURE_BY_VALUE) {
            // Değer ile capture
            fprintf(out, "    mov rax, [rbp - 24]    ; %s değeri\n", var->name);
            fprintf(out, "    mov [rbx], rax    ; Kopyala\n");
        } else {
            // Referans ile capture
            fprintf(out, "    lea rax, [rbp - 24]    ; %s adresi\n", var->name);
            fprintf(out, "    mov [rbx], rax    ; Pointer kaydet\n");
        }
        
        var = var->next;
    }
}

// Lambda fonksiyon tanımı üret
void codegen_lambda_function(FILE* out, Lambda* lambda, int lambda_id) {
    const char* name = get_lambda_mangled_name(lambda, lambda_id);
    
    fprintf(out, "\n%s:    ; Lambda fonksiyonu\n", name);
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    
    // Closure pointer'ı al (eğer varsa)
    if (lambda->type == LAMBDA_CLOSURE) {
        fprintf(out, "    ; Closure pointer r10'da\n");
        fprintf(out, "    mov r10, [rbp + 16]    ; Closure erişimi\n");
    }
    
    fprintf(out, "    ; Lambda gövdesi\n");
    // Body burada üretilecek (parser'dan gelecek)
    
    fprintf(out, "    ; Dönüş değeri rax'te\n");
    fprintf(out, "    mov rsp, rbp\n");
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n\n");
}

// Higher-order function için kod üret
void codegen_higher_order(FILE* out, HigherOrder* ho) {
    fprintf(out, "    ; Higher-order: %s\n", ho->function_name);
    
    // map, filter, reduce gibi fonksiyonlar için
    if (strcmp(ho->function_name, "map") == 0) {
        fprintf(out, "    ; Map operasyonu\n");
        fprintf(out, "    mov rdi, [rbp - 8]     ; Koleksiyon\n");
        fprintf(out, "    mov rsi, [rbp - 16]    ; Lambda\n");
        fprintf(out, "    call mlp_map    ; Runtime fonksiyonu\n");
    } else if (strcmp(ho->function_name, "filter") == 0) {
        fprintf(out, "    ; Filter operasyonu\n");
        fprintf(out, "    mov rdi, [rbp - 8]     ; Koleksiyon\n");
        fprintf(out, "    mov rsi, [rbp - 16]    ; Predicate\n");
        fprintf(out, "    call mlp_filter\n");
    } else if (strcmp(ho->function_name, "reduce") == 0) {
        fprintf(out, "    ; Reduce operasyonu\n");
        fprintf(out, "    mov rdi, [rbp - 8]     ; Koleksiyon\n");
        fprintf(out, "    mov rsi, [rbp - 16]    ; Lambda\n");
        fprintf(out, "    mov rdx, [rbp - 24]    ; Initial değer\n");
        fprintf(out, "    call mlp_reduce\n");
    }
}

// Captured değişken erişimi için kod üret
void codegen_capture_access(FILE* out, CapturedVar* var) {
    fprintf(out, "    ; Capture erişimi: %s\n", var->name);
    
    // Closure pointer'dan değişkene eriş
    fprintf(out, "    mov rax, r10    ; Closure base\n");
    fprintf(out, "    add rax, %zu    ; Offset\n", var->offset);
    
    if (var->mode == CAPTURE_BY_VALUE) {
        fprintf(out, "    mov rax, [rax]    ; Değeri oku\n");
    } else {
        fprintf(out, "    mov rax, [rax]    ; Pointer oku\n");
        fprintf(out, "    mov rax, [rax]    ; Değeri oku\n");
    }
}

// Lambda için mangled isim üret
const char* get_lambda_mangled_name(Lambda* lambda, int lambda_id) {
    static char buffer[64];
    snprintf(buffer, sizeof(buffer), ".lambda_%d", lambda_id);
    return buffer;
}
