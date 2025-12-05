section .data

section .bss
    var_a resq 1 ; INT64 (small integer)
    var_b resq 1 ; INT64 (small integer)
    var_sum resq 1 ; INT64 (small integer)
    var_diff resq 1 ; INT64 (small integer)
    var_prod resq 1 ; INT64 (small integer)
    var_quot resq 1 ; INT64 (small integer)

section .text
    global _start

_start:
    ; a = 10 (INT64)
    mov rax, 10
    mov [var_a], rax
    ; b = 20 (INT64)
    mov rax, 20
    mov [var_b], rax
    ; sum = 30 (INT64)
    mov rax, 30
    mov [var_sum], rax
    ; diff = 10 (INT64)
    mov rax, 10
    mov [var_diff], rax
    ; prod = 200 (INT64)
    mov rax, 200
    mov [var_prod], rax
    ; quot = 2 (INT64)
    mov rax, 2
    mov [var_quot], rax

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
