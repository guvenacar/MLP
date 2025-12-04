#include "print_codegen.h"
#include "print.h"
#include <stdio.h>
#include <string.h>

static int string_counter = 0;
static int is_first_call = 1;

void codegen_print_statement(FILE* f, PrintStatement* stmt) {
    if (!stmt || !stmt->value) return;
    
    // Get string content (temporarily stored as Expression* pointer)
    const char* string_content = (const char*)stmt->value;
    
    // On first call, emit program header
    if (is_first_call) {
        fprintf(f, "; MLP Print Module - Generated Assembly\n");
        fprintf(f, "; Target: x86-64 Linux\n\n");
        is_first_call = 0;
    }
    
    // Generate unique label for this string
    fprintf(f, "section .data\n");
    fprintf(f, "    str_%d: db \"%s\", 10, 0  ; string with newline\n", string_counter, string_content);
    fprintf(f, "    str_%d_len: equ $-str_%d-2  ; length without null terminator and newline\n\n", 
            string_counter, string_counter);
    
    // If this is the first statement, emit _start label
    if (string_counter == 0) {
        fprintf(f, "section .text\n");
        fprintf(f, "    global _start\n");
        fprintf(f, "_start:\n");
    } else {
        fprintf(f, "section .text\n");
    }
    
    fprintf(f, "    ; Print: %s\n", string_content);
    
    // sys_write(stdout=1, buffer=str_N, length=str_N_len+1)
    fprintf(f, "    mov rax, 1              ; sys_write\n");
    fprintf(f, "    mov rdi, 1              ; stdout\n");
    fprintf(f, "    lea rsi, [rel str_%d]   ; string address\n", string_counter);
    fprintf(f, "    mov rdx, str_%d_len     ; string length\n", string_counter);
    fprintf(f, "    add rdx, 1              ; include newline\n");
    fprintf(f, "    syscall\n\n");
    
    string_counter++;
}

void codegen_print_finalize(FILE* f) {
    // Emit program exit code
    fprintf(f, "    ; Program exit\n");
    fprintf(f, "    mov rax, 60             ; sys_exit\n");
    fprintf(f, "    xor rdi, rdi            ; exit code 0\n");
    fprintf(f, "    syscall\n");
}


