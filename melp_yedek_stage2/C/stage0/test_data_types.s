section .data

section .bss
    var_TYPE_NUMERIC resq 1 ; INT64 (small integer)
    var_TYPE_STRING resq 1 ; INT64 (small integer)
    var_TYPE_BOOLEAN resq 1 ; INT64 (small integer)

section .text
    global _start

_start:
    ; TYPE_NUMERIC = 0 (INT64)
    mov rax, 0
    mov [var_TYPE_NUMERIC], rax
    ; TYPE_STRING = 1 (INT64)
    mov rax, 1
    mov [var_TYPE_STRING], rax
    ; TYPE_BOOLEAN = 2 (INT64)
    mov rax, 2
    mov [var_TYPE_BOOLEAN], rax

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
