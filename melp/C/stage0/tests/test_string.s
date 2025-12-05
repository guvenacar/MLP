section .data
    str_hello db "Hello World", 0  ; SSO (≤23 bytes)
    str_name db "MLP Compiler", 0  ; SSO (≤23 bytes)
    str_empty db "", 0  ; SSO (≤23 bytes)

section .bss
    var_hello resq 1  ; STRING pointer
    var_name resq 1  ; STRING pointer
    var_empty resq 1  ; STRING pointer

section .text
    global _start

_start:
    ; hello = "Hello World" (SSO)
    mov rax, str_hello
    mov [var_hello], rax
    ; name = "MLP Compiler" (SSO)
    mov rax, str_name
    mov [var_name], rax
    ; empty = "" (SSO)
    mov rax, str_empty
    mov [var_empty], rax

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
