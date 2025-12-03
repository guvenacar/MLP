section .data

section .bss
    var_x resq 1 ; INT64 (small integer)
    var_y resq 1 ; INT64 (small integer)

section .text
    global _start

_start:
    ; x = 42 (INT64)
    mov rax, 42
    mov [var_x], rax
    ; y = 10 (INT64)
    mov rax, 10
    mov [var_y], rax

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
