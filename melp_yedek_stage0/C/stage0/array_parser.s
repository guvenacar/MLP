section .data

section .bss
    var_EXPR_ARRAY_LITERAL resq 1 ; INT64 (small integer)
    var_EXPR_LIST_LITERAL resq 1 ; INT64 (small integer)
    var_EXPR_TUPLE_LITERAL resq 1 ; INT64 (small integer)
    var_EXPR_INDEX_ACCESS resq 1 ; INT64 (small integer)

section .text
    global _start

_start:
    ; EXPR_ARRAY_LITERAL = 18 (INT64)
    mov rax, 18
    mov [var_EXPR_ARRAY_LITERAL], rax
    ; EXPR_LIST_LITERAL = 19 (INT64)
    mov rax, 19
    mov [var_EXPR_LIST_LITERAL], rax
    ; EXPR_TUPLE_LITERAL = 20 (INT64)
    mov rax, 20
    mov [var_EXPR_TUPLE_LITERAL], rax
    ; EXPR_INDEX_ACCESS = 21 (INT64)
    mov rax, 21
    mov [var_EXPR_INDEX_ACCESS], rax

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
