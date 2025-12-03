section .data

section .bss
    var_x resq 1 ; INT64 (small integer)
    var_y resq 1 ; INT64 (small integer)
    var_less resb 1  ; BOOLEAN
    var_greater resb 1  ; BOOLEAN

section .text
    global _start

_start:
    ; x = 10 (INT64)
    mov rax, 10
    mov [var_x], rax
    ; y = 20 (INT64)
    mov rax, 20
    mov [var_y], rax
    ; less = true
    mov byte [var_less], 1
    ; greater = false
    mov byte [var_greater], 0

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
