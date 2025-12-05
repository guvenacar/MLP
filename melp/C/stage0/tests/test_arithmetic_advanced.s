section .data

section .bss
    var_a resq 1 ; INT64 (small integer)
    var_b resq 1 ; INT64 (small integer)
    var_sum resq 1 ; INT64 (small integer)
    var_diff resq 1 ; INT64 (small integer)
    var_product resq 1 ; INT64 (small integer)
    var_quotient resq 1 ; INT64 (small integer)
    var_expr1 resq 1 ; INT64 (small integer)
    var_expr2 resq 1 ; INT64 (small integer)
    var_expr3 resq 1 ; INT64 (small integer)

section .text
    global _start

_start:
    ; a = 10 (INT64)
    mov rax, 10
    mov [var_a], rax
    ; b = 5 (INT64)
    mov rax, 5
    mov [var_b], rax
    ; sum = 15 (INT64)
    mov rax, 15
    mov [var_sum], rax
    ; diff = 5 (INT64)
    mov rax, 5
    mov [var_diff], rax
    ; product = 50 (INT64)
    mov rax, 50
    mov [var_product], rax
    ; quotient = 2 (INT64)
    mov rax, 2
    mov [var_quotient], rax
    ; expr1 = 100 (INT64)
    mov rax, 100
    mov [var_expr1], rax
    ; expr2 = 25 (INT64)
    mov rax, 25
    mov [var_expr2], rax
    ; expr3 = 200 (INT64)
    mov rax, 200
    mov [var_expr3], rax

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
