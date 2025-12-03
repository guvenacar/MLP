section .data

section .bss
    var_arr_size resq 1 ; INT64 (small integer)
    var_index resq 1 ; INT64 (small integer)

section .text
    global _start

_start:
    ; arr_size = 10 (INT64)
    mov rax, 10
    mov [var_arr_size], rax
    ; index = 5 (INT64)
    mov rax, 5
    mov [var_index], rax

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
