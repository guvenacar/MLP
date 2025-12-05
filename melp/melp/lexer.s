section .data

section .bss
    var_TOKEN_NUMERIC resq 1
    var_TOKEN_TEXT resq 1
    var_TOKEN_BOOLEAN resq 1
    var_TOKEN_IDENTIFIER resq 1
    var_TOKEN_ASSIGN resq 1
    var_TOKEN_NUMBER resq 1
    var_TOKEN_STRING resq 1
    var_TOKEN_TRUE resq 1
    var_TOKEN_FALSE resq 1
    var_TOKEN_EOF resq 1
    var_test_token resq 1

section .text
    global _start

_start:
    mov rax, 0
    mov [var_TOKEN_NUMERIC], rax
    mov rax, 1
    mov [var_TOKEN_TEXT], rax
    mov rax, 2
    mov [var_TOKEN_BOOLEAN], rax
    mov rax, 3
    mov [var_TOKEN_IDENTIFIER], rax
    mov rax, 4
    mov [var_TOKEN_ASSIGN], rax
    mov rax, 5
    mov [var_TOKEN_NUMBER], rax
    mov rax, 6
    mov [var_TOKEN_STRING], rax
    mov rax, 7
    mov [var_TOKEN_TRUE], rax
    mov rax, 8
    mov [var_TOKEN_FALSE], rax
    mov rax, 9
    mov [var_TOKEN_EOF], rax
    mov rax, 42
    mov [var_test_token], rax

    mov rax, 60
    xor rdi, rdi
    syscall
