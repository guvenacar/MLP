section .data
    msg: db "Generic Types OK!", 10, 0
    msg_len: equ $ - msg

section .bss
    var_T: resq 1
    var_count: resq 1
    var_strings: resq 1

section .text
    global _start

_start:
    push rbp
    mov rbp, rsp
    sub rsp, 64

    ; T: List [INSTANTIATION args=1]
    mov qword [var_T], 0    ; Generic/custom type

    ; count: numeric
    mov qword [var_count], 0

    ; strings: List [INSTANTIATION args=1]
    mov qword [var_strings], 0    ; Generic/custom type

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
