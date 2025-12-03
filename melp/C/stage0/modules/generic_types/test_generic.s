section .data
    msg: db "Generic Types OK!", 10, 0
    msg_len: equ $ - msg
    str_result: db "generic_text", 0

section .bss
    var_List: resq 1
    var_identity: resq 1
    var_numbers: resq 1
    var_x: resq 1
    var_result: resq 1

section .text
    global _start

_start:
    push rbp
    mov rbp, rsp
    sub rsp, 64

    ; List: struct [GENERIC STRUCT params=1]
    mov qword [var_List], 0    ; Generic/custom type

    ; identity: function [GENERIC FUNC params=1]
    mov qword [var_identity], 0    ; Generic/custom type

    ; numbers: List [INSTANTIATION args=1]
    mov qword [var_numbers], 0    ; Generic/custom type

    ; x: numeric
    mov qword [var_x], 0

    ; result: text
    lea rax, [str_result]
    mov [var_result], rax

    ; Print result
    mov rax, 1
    mov rdi, 1
    lea rsi, [msg]
    mov rdx, msg_len
    syscall

    ; Exit
    mov rsp, rbp
    pop rbp
    mov rax, 60
    xor rdi, rdi
    syscall
