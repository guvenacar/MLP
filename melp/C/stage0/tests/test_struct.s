section .data
    str_person_name db "Alice", 0  ; SSO (≤23 bytes)

section .bss
    var_person_age resq 1 ; INT64 (small integer)
    var_person_name resq 1  ; STRING pointer

section .text
    global _start

_start:
    ; person_age = 25 (INT64)
    mov rax, 25
    mov [var_person_age], rax
    ; person_name = "Alice" (SSO)
    mov rax, str_person_name
    mov [var_person_name], rax

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
