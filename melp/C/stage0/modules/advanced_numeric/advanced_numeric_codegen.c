#include "advanced_numeric.h"

void num_generate_asm(const char* output_file, NumCtx* ctx) {
    FILE* f = fopen(output_file, "w");
    if (!f) return;
    
    // Generate assembly header
    fprintf(f, "; Advanced Numeric Module\n");
    fprintf(f, "; Supports: u8-u128, i8-i128, f32-f128\n\n");
    
    // Data section
    fprintf(f, "section .data\n");
    fprintf(f, "    msg_header: db \"Advanced Numeric Module\", 10, 0\n");
    fprintf(f, "    msg_header_len: equ $ - msg_header - 1\n");
    fprintf(f, "    msg_unsigned: db \"Unsigned types: \", 0\n");
    fprintf(f, "    msg_unsigned_len: equ $ - msg_unsigned - 1\n");
    fprintf(f, "    msg_signed: db \"Signed types: \", 0\n");
    fprintf(f, "    msg_signed_len: equ $ - msg_signed - 1\n");
    fprintf(f, "    msg_float: db \"Float types: \", 0\n");
    fprintf(f, "    msg_float_len: equ $ - msg_float - 1\n");
    fprintf(f, "    msg_ok: db \"Advanced Numeric OK!\", 10, 0\n");
    fprintf(f, "    msg_ok_len: equ $ - msg_ok - 1\n");
    fprintf(f, "    newline: db 10\n\n");
    
    // BSS section for numeric operations
    fprintf(f, "section .bss\n");
    fprintf(f, "    num_buffer: resb 32\n\n");
    
    // Text section
    fprintf(f, "section .text\n");
    fprintf(f, "    global _start\n\n");
    
    // Helper function: print string
    fprintf(f, "print_string:\n");
    fprintf(f, "    ; rsi = string, rdx = length\n");
    fprintf(f, "    mov rax, 1          ; sys_write\n");
    fprintf(f, "    mov rdi, 1          ; stdout\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "    ret\n\n");
    
    // Helper function: print number
    fprintf(f, "print_number:\n");
    fprintf(f, "    ; rax = number to print\n");
    fprintf(f, "    push rax\n");
    fprintf(f, "    add rax, '0'\n");
    fprintf(f, "    mov [num_buffer], al\n");
    fprintf(f, "    mov rsi, num_buffer\n");
    fprintf(f, "    mov rdx, 1\n");
    fprintf(f, "    call print_string\n");
    fprintf(f, "    pop rax\n");
    fprintf(f, "    ret\n\n");
    
    // Main entry point
    fprintf(f, "_start:\n");
    fprintf(f, "    ; Print header\n");
    fprintf(f, "    mov rsi, msg_header\n");
    fprintf(f, "    mov rdx, msg_header_len\n");
    fprintf(f, "    call print_string\n\n");
    
    // Print statistics
    fprintf(f, "    ; Print unsigned count\n");
    fprintf(f, "    mov rsi, msg_unsigned\n");
    fprintf(f, "    mov rdx, msg_unsigned_len\n");
    fprintf(f, "    call print_string\n");
    fprintf(f, "    mov rax, %d\n", ctx->unsigned_count);
    fprintf(f, "    call print_number\n");
    fprintf(f, "    mov rsi, newline\n");
    fprintf(f, "    mov rdx, 1\n");
    fprintf(f, "    call print_string\n\n");
    
    fprintf(f, "    ; Print signed count\n");
    fprintf(f, "    mov rsi, msg_signed\n");
    fprintf(f, "    mov rdx, msg_signed_len\n");
    fprintf(f, "    call print_string\n");
    fprintf(f, "    mov rax, %d\n", ctx->signed_count);
    fprintf(f, "    call print_number\n");
    fprintf(f, "    mov rsi, newline\n");
    fprintf(f, "    mov rdx, 1\n");
    fprintf(f, "    call print_string\n\n");
    
    fprintf(f, "    ; Print float count\n");
    fprintf(f, "    mov rsi, msg_float\n");
    fprintf(f, "    mov rdx, msg_float_len\n");
    fprintf(f, "    call print_string\n");
    fprintf(f, "    mov rax, %d\n", ctx->float_count);
    fprintf(f, "    call print_number\n");
    fprintf(f, "    mov rsi, newline\n");
    fprintf(f, "    mov rdx, 1\n");
    fprintf(f, "    call print_string\n\n");
    
    // Print success message
    fprintf(f, "    ; Success message\n");
    fprintf(f, "    mov rsi, msg_ok\n");
    fprintf(f, "    mov rdx, msg_ok_len\n");
    fprintf(f, "    call print_string\n\n");
    
    // Exit
    fprintf(f, "    ; Exit\n");
    fprintf(f, "    mov rax, 60         ; sys_exit\n");
    fprintf(f, "    xor rdi, rdi        ; exit code 0\n");
    fprintf(f, "    syscall\n");
    
    fclose(f);
}
