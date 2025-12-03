section .data

section .bss
    var_CODEGEN_ARRAY_SUCCESS resq 1 ; INT64 (small integer)
    var_CODEGEN_ARRAY_ERROR resq 1 ; INT64 (small integer)
    var_ARRAY_STORAGE_STACK resq 1 ; INT64 (small integer)
    var_ARRAY_STORAGE_HEAP resq 1 ; INT64 (small integer)

section .text
    global _start

_start:
    ; CODEGEN_ARRAY_SUCCESS = 0 (INT64)
    mov rax, 0
    mov [var_CODEGEN_ARRAY_SUCCESS], rax
    ; CODEGEN_ARRAY_ERROR = 1 (INT64)
    mov rax, 1
    mov [var_CODEGEN_ARRAY_ERROR], rax
    ; ARRAY_STORAGE_STACK = 0 (INT64)
    mov rax, 0
    mov [var_ARRAY_STORAGE_STACK], rax
    ; ARRAY_STORAGE_HEAP = 1 (INT64)
    mov rax, 1
    mov [var_ARRAY_STORAGE_HEAP], rax

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
