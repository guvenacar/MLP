section .data

section .bss
    var_TOKEN_NUMERIC resq 1 ; INT64 (small integer)
    var_TOKEN_TEXT resq 1 ; INT64 (small integer)
    var_TOKEN_BOOLEAN resq 1 ; INT64 (small integer)
    var_TOKEN_IDENTIFIER resq 1 ; INT64 (small integer)
    var_TOKEN_ASSIGN resq 1 ; INT64 (small integer)
    var_TOKEN_NUMBER resq 1 ; INT64 (small integer)
    var_TOKEN_STRING resq 1 ; INT64 (small integer)
    var_TOKEN_TRUE resq 1 ; INT64 (small integer)
    var_TOKEN_FALSE resq 1 ; INT64 (small integer)
    var_TOKEN_EOF resq 1 ; INT64 (small integer)
    var_test_token resq 1 ; INT64 (small integer)

section .text
    global _start

_start:
    ; TOKEN_NUMERIC = 0 (INT64)
    mov rax, 0
    mov [var_TOKEN_NUMERIC], rax
    ; TOKEN_TEXT = 1 (INT64)
    mov rax, 1
    mov [var_TOKEN_TEXT], rax
    ; TOKEN_BOOLEAN = 2 (INT64)
    mov rax, 2
    mov [var_TOKEN_BOOLEAN], rax
    ; TOKEN_IDENTIFIER = 3 (INT64)
    mov rax, 3
    mov [var_TOKEN_IDENTIFIER], rax
    ; TOKEN_ASSIGN = 4 (INT64)
    mov rax, 4
    mov [var_TOKEN_ASSIGN], rax
    ; TOKEN_NUMBER = 5 (INT64)
    mov rax, 5
    mov [var_TOKEN_NUMBER], rax
    ; TOKEN_STRING = 6 (INT64)
    mov rax, 6
    mov [var_TOKEN_STRING], rax
    ; TOKEN_TRUE = 7 (INT64)
    mov rax, 7
    mov [var_TOKEN_TRUE], rax
    ; TOKEN_FALSE = 8 (INT64)
    mov rax, 8
    mov [var_TOKEN_FALSE], rax
    ; TOKEN_EOF = 9 (INT64)
    mov rax, 9
    mov [var_TOKEN_EOF], rax
    ; test_token = 42 (INT64)
    mov rax, 42
    mov [var_test_token], rax

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
