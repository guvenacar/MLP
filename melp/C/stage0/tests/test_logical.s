section .data

section .bss
    var_a resb 1  ; BOOLEAN
    var_b resb 1  ; BOOLEAN
    var_and_result resb 1  ; BOOLEAN
    var_or_result resb 1  ; BOOLEAN

section .text
    global _start

_start:
    ; a = true
    mov byte [var_a], 1
    ; b = false
    mov byte [var_b], 0
    ; and_result = false
    mov byte [var_and_result], 0
    ; or_result = true
    mov byte [var_or_result], 1

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
