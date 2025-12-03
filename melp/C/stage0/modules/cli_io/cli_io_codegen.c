#include "cli_io_codegen.h"
#include <string.h>

// Forward declarations
static void generate_print_int_function(FILE* f);
static void generate_print_string_function(FILE* f);

// Global helper function generation
void cli_io_generate_helpers(FILE* f) {
    generate_print_int_function(f);
    generate_print_string_function(f);
}

// Generate print_int helper function (prints integer in rax to stdout)
static void generate_print_int_function(FILE* f) {
    fprintf(f, "\n; Print integer in rax to stdout\n");
    fprintf(f, "print_int:\n");
    fprintf(f, "    push rbx\n");
    fprintf(f, "    push rcx\n");
    fprintf(f, "    push rdx\n");
    fprintf(f, "    push rsi\n");
    fprintf(f, "    push rdi\n");
    fprintf(f, "    mov rsi, rsp\n");
    fprintf(f, "    sub rsp, 32          ; Buffer for digits\n");
    fprintf(f, "    mov rdi, rsp\n");
    fprintf(f, "    add rdi, 31          ; End of buffer\n");
    fprintf(f, "    mov byte [rdi], 10   ; Newline\n");
    fprintf(f, "    dec rdi\n");
    fprintf(f, "    mov rbx, 10\n");
    fprintf(f, "    test rax, rax\n");
    fprintf(f, "    jns .print_positive\n");
    fprintf(f, "    neg rax\n");
    fprintf(f, "    mov rcx, 1           ; Negative flag\n");
    fprintf(f, "    jmp .print_loop\n");
    fprintf(f, ".print_positive:\n");
    fprintf(f, "    xor rcx, rcx         ; Positive\n");
    fprintf(f, ".print_loop:\n");
    fprintf(f, "    xor rdx, rdx\n");
    fprintf(f, "    div rbx\n");
    fprintf(f, "    add dl, '0'\n");
    fprintf(f, "    mov [rdi], dl\n");
    fprintf(f, "    dec rdi\n");
    fprintf(f, "    test rax, rax\n");
    fprintf(f, "    jnz .print_loop\n");
    fprintf(f, "    test rcx, rcx\n");
    fprintf(f, "    jz .print_write\n");
    fprintf(f, "    mov byte [rdi], '-'\n");
    fprintf(f, "    dec rdi\n");
    fprintf(f, ".print_write:\n");
    fprintf(f, "    inc rdi\n");
    fprintf(f, "    mov rax, 1           ; sys_write\n");
    fprintf(f, "    mov rsi, rdi\n");
    fprintf(f, "    lea rdx, [rsp + 32]\n");
    fprintf(f, "    sub rdx, rdi\n");
    fprintf(f, "    mov rdi, 1           ; stdout\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "    add rsp, 32\n");
    fprintf(f, "    pop rdi\n");
    fprintf(f, "    pop rsi\n");
    fprintf(f, "    pop rdx\n");
    fprintf(f, "    pop rcx\n");
    fprintf(f, "    pop rbx\n");
    fprintf(f, "    ret\n");
}

// Generate print_string helper function (prints null-terminated string in rax)
static void generate_print_string_function(FILE* f) {
    fprintf(f, "\n; Print null-terminated string (pointer in rax) to stdout\n");
    fprintf(f, "print_string:\n");
    fprintf(f, "    push rbx\n");
    fprintf(f, "    push rcx\n");
    fprintf(f, "    push rdx\n");
    fprintf(f, "    push rsi\n");
    fprintf(f, "    push rdi\n");
    fprintf(f, "    mov rsi, rax         ; String pointer\n");
    fprintf(f, "    mov rdi, rsi         ; Save start\n");
    fprintf(f, "    ; Calculate string length\n");
    fprintf(f, "    xor rcx, rcx\n");
    fprintf(f, ".strlen_loop:\n");
    fprintf(f, "    mov al, [rsi + rcx]\n");
    fprintf(f, "    test al, al\n");
    fprintf(f, "    jz .strlen_done\n");
    fprintf(f, "    inc rcx\n");
    fprintf(f, "    jmp .strlen_loop\n");
    fprintf(f, ".strlen_done:\n");
    fprintf(f, "    ; rcx = length, rdi = string pointer\n");
    fprintf(f, "    mov rdx, rcx         ; Length\n");
    fprintf(f, "    mov rsi, rdi         ; String pointer\n");
    fprintf(f, "    mov rax, 1           ; sys_write\n");
    fprintf(f, "    mov rdi, 1           ; stdout\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "    ; Print newline\n");
    fprintf(f, "    push 10              ; Newline character\n");
    fprintf(f, "    mov rax, 1\n");
    fprintf(f, "    mov rdi, 1\n");
    fprintf(f, "    mov rsi, rsp\n");
    fprintf(f, "    mov rdx, 1\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "    add rsp, 8\n");
    fprintf(f, "    pop rdi\n");
    fprintf(f, "    pop rsi\n");
    fprintf(f, "    pop rdx\n");
    fprintf(f, "    pop rcx\n");
    fprintf(f, "    pop rbx\n");
    fprintf(f, "    ret\n");
}

// Print için kod üret (newline yok)
void codegen_print(FILE* out, PrintOp* print) {
    fprintf(out, "    ; Print çıktısı (newline yok)\n");
    
    switch (print->type) {
        case OUT_STRING:
            // String yazdırma - printf ile
            fprintf(out, "    lea rdi, [.str_fmt_s]    ; \"%%s\"\n");
            fprintf(out, "    mov rsi, [rbp - 8]    ; String değeri\n");
            fprintf(out, "    xor rax, rax    ; Variadic flag\n");
            fprintf(out, "    call printf\n");
            break;
            
        case OUT_NUMBER:
            // Sayı yazdırma
            fprintf(out, "    lea rdi, [.str_fmt_d]    ; \"%%ld\"\n");
            fprintf(out, "    mov rsi, [rbp - 8]    ; Sayı değeri\n");
            fprintf(out, "    xor rax, rax\n");
            fprintf(out, "    call printf\n");
            break;
            
        case OUT_BOOLEAN:
            // Boolean yazdırma (true/false)
            fprintf(out, "    mov rax, [rbp - 8]    ; Boolean değeri\n");
            fprintf(out, "    test rax, rax\n");
            fprintf(out, "    jz .print_false\n");
            fprintf(out, "    lea rdi, [.str_true]\n");
            fprintf(out, "    jmp .print_bool_done\n");
            fprintf(out, ".print_false:\n");
            fprintf(out, "    lea rdi, [.str_false]\n");
            fprintf(out, ".print_bool_done:\n");
            fprintf(out, "    xor rax, rax\n");
            fprintf(out, "    call printf\n");
            break;
            
        case OUT_FORMATTED:
            // Formatlanmış çıktı
            if (print->format) {
                fprintf(out, "    lea rdi, [.str_custom_%p]    ; Custom format\n", 
                        (void*)print);
                fprintf(out, "    ; Parametreler stack'te hazır olmalı\n");
                fprintf(out, "    call printf\n");
            }
            break;
    }
}

// Println için kod üret (newline ile)
void codegen_println(FILE* out, PrintOp* print) {
    // Önce normal print
    codegen_print(out, print);
    
    // Sonra newline
    fprintf(out, "    ; Newline ekle\n");
    fprintf(out, "    mov rdi, 10    ; '\\n'\n");
    fprintf(out, "    call putchar\n");
}

// Printf için kod üret (format string ile)
void codegen_printf(FILE* out, PrintOp* print) {
    if (!print->format) {
        codegen_print(out, print);
        return;
    }
    
    fprintf(out, "    ; Printf - format: %s\n", print->format);
    fprintf(out, "    lea rdi, [.str_fmt_%p]\n", (void*)print);
    fprintf(out, "    ; Parametreler rsi, rdx, rcx, r8, r9, stack\n");
    fprintf(out, "    xor rax, rax    ; Variadic flag\n");
    fprintf(out, "    call printf\n");
    
    // Format string'i data section'a ekle
    fprintf(out, "section .data\n");
    fprintf(out, "    .str_fmt_%p: db \"%s\", 0\n", (void*)print, print->format);
    fprintf(out, "section .text\n");
}

// Input için kod üret
void codegen_input(FILE* out, InputOp* input) {
    fprintf(out, "    ; Kullanıcı girdisi\n");
    
    // Prompt göster (varsa)
    if (input->prompt) {
        fprintf(out, "    lea rdi, [.str_prompt_%p]\n", (void*)input);
        fprintf(out, "    xor rax, rax\n");
        fprintf(out, "    call printf\n");
        
        fprintf(out, "section .data\n");
        fprintf(out, "    .str_prompt_%p: db \"%s\", 0\n", 
                (void*)input, input->prompt);
        fprintf(out, "section .text\n");
    }
    
    // Input oku
    switch (input->expected) {
        case OUT_STRING:
            // String okuma - fgets
            fprintf(out, "    lea rdi, [rbp - %zu]    ; Buffer\n", 
                    input->max_size);
            fprintf(out, "    mov rsi, %zu    ; Max boyut\n", input->max_size);
            fprintf(out, "    mov rdx, [stdin]    ; stdin\n");
            fprintf(out, "    call fgets\n");
            break;
            
        case OUT_NUMBER:
            // Sayı okuma - scanf
            fprintf(out, "    lea rdi, [.str_scanf_d]    ; \"%%ld\"\n");
            fprintf(out, "    lea rsi, [rbp - 8]    ; Hedef adres\n");
            fprintf(out, "    xor rax, rax\n");
            fprintf(out, "    call scanf\n");
            break;
            
        default:
            // Genel okuma
            fprintf(out, "    ; Generic input - scanf\n");
            break;
    }
}

// CLI argüman erişimi için kod üret
void codegen_arg_access(FILE* out, ArgAccess* access) {
    fprintf(out, "    ; CLI argüman erişimi: args[%d]\n", access->index);
    
    // argc kontrolü
    fprintf(out, "    mov rax, [rbp + 16]    ; argc (main parametresi)\n");
    fprintf(out, "    cmp rax, %d\n", access->index + 1);
    fprintf(out, "    jle .arg_out_of_bounds    ; Index geçersiz\n");
    
    // argv[index] erişimi
    fprintf(out, "    mov rax, [rbp + 24]    ; argv (main parametresi)\n");
    fprintf(out, "    mov rbx, %d    ; Index\n", access->index);
    fprintf(out, "    shl rbx, 3    ; x8 (pointer boyutu)\n");
    fprintf(out, "    add rax, rbx    ; argv[index] adresi\n");
    fprintf(out, "    mov rax, [rax]    ; argv[index] değeri\n");
}

// Renkli çıktı için kod üret
void codegen_color_output(FILE* out, ColorOutput* color) {
    fprintf(out, "    ; Renkli çıktı: %d (bold: %d)\n", color->color, color->bold);
    
    // ANSI escape code yazdır
    const char* ansi_code = get_ansi_color_code(color->color, color->bold);
    fprintf(out, "    lea rdi, [.str_ansi_%p]\n", (void*)color);
    fprintf(out, "    xor rax, rax\n");
    fprintf(out, "    call printf\n");
    
    // Metni yazdır
    fprintf(out, "    lea rdi, [.str_text_%p]\n", (void*)color);
    fprintf(out, "    xor rax, rax\n");
    fprintf(out, "    call printf\n");
    
    // Reset
    fprintf(out, "    lea rdi, [.str_reset]\n");
    fprintf(out, "    xor rax, rax\n");
    fprintf(out, "    call printf\n");
    
    // String'leri ekle
    fprintf(out, "section .data\n");
    fprintf(out, "    .str_ansi_%p: db \"%s\", 0\n", (void*)color, ansi_code);
    fprintf(out, "    .str_text_%p: db \"%s\", 0\n", (void*)color, color->text);
    fprintf(out, "    .str_reset: db \"\\033[0m\", 0\n");
    fprintf(out, "section .text\n");
}

// puts çağrısı için yardımcı
void codegen_puts_call(FILE* out, const char* str) {
    fprintf(out, "    lea rdi, [.str_%p]\n", (void*)str);
    fprintf(out, "    call puts\n");
}

// printf çağrısı için yardımcı
void codegen_printf_call(FILE* out, const char* format) {
    fprintf(out, "    lea rdi, [.str_fmt_%p]\n", (void*)format);
    fprintf(out, "    xor rax, rax\n");
    fprintf(out, "    call printf\n");
}

// ANSI renk kodunu al
const char* get_ansi_color_code(AnsiColor color, int bold) {
    const char* base = bold ? "\\033[1;" : "\\033[0;";
    
    switch (color) {
        case COLOR_RESET: return "\\033[0m";
        case COLOR_BLACK: return bold ? "\\033[1;30m" : "\\033[0;30m";
        case COLOR_RED: return bold ? "\\033[1;31m" : "\\033[0;31m";
        case COLOR_GREEN: return bold ? "\\033[1;32m" : "\\033[0;32m";
        case COLOR_YELLOW: return bold ? "\\033[1;33m" : "\\033[0;33m";
        case COLOR_BLUE: return bold ? "\\033[1;34m" : "\\033[0;34m";
        case COLOR_MAGENTA: return bold ? "\\033[1;35m" : "\\033[0;35m";
        case COLOR_CYAN: return bold ? "\\033[1;36m" : "\\033[0;36m";
        case COLOR_WHITE: return bold ? "\\033[1;37m" : "\\033[0;37m";
    }
    return "\\033[0m";
}
