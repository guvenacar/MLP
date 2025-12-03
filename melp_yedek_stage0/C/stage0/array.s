section .data

section .bss
    var_COLL_ARRAY resq 1 ; INT64 (small integer)
    var_COLL_LIST resq 1 ; INT64 (small integer)
    var_COLL_TUPLE resq 1 ; INT64 (small integer)

section .text
    global _start

_start:
    ; COLL_ARRAY = 0 (INT64)
    mov rax, 0
    mov [var_COLL_ARRAY], rax
    ; COLL_LIST = 1 (INT64)
    mov rax, 1
    mov [var_COLL_LIST], rax
    ; COLL_TUPLE = 2 (INT64)
    mov rax, 2
    mov [var_COLL_TUPLE], rax

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
