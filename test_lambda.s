section .data
    msg: db "Lambda OK!", 10, 0
    msg_len: equ $ - msg

section .bss
    var_double: resq 1
    var_result: resq 1

section .text
    global _start

; Lambda: double
lambda_double:
    push rbp
    mov rbp, rsp
    mov rax, rdi    ; Return first param
    pop rbp
    ret

_start:
    push rbp
    mov rbp, rsp
    sub rsp, 64

    ; Variable: double (numeric) [LAMBDA]
    lea rax, [lambda_double]
    mov [var_double], rax
    ; Params: x

    ; Variable: result (numeric)
    mov qword [var_result], 0

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
