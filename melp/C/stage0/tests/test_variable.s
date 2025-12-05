section .data
    str_name db "MLP", 0  ; SSO (≤23 bytes)

section .bss
    var_x resq 1 ; INT64 (small integer)
    var_y resq 1 ; INT64 (small integer)
    var_name resq 1  ; STRING pointer
    var_flag resb 1  ; BOOLEAN

section .text
    global _start

_start:
    ; x = 42 (INT64)
    mov rax, 42
    mov [var_x], rax
    ; y = 100 (INT64)
    mov rax, 100
    mov [var_y], rax
    ; name = "MLP" (SSO)
    mov rax, str_name
    mov [var_name], rax
    ; flag = true
    mov byte [var_flag], 1

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
