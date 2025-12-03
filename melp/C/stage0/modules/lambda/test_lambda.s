section .data
    msg: db "Lambda OK!", 10, 0
    msg_len: equ $ - msg
    str_result: db "lambda_text", 0

section .bss
    var_x: resq 1
    var_y: resq 1
    var_double: resq 1
    var_add: resq 1
    var_multiplier: resq 1
    var_counter: resq 1
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

; Lambda: add
lambda_add:
    push rbp
    mov rbp, rsp
    mov rax, rdi    ; Return first param
    pop rbp
    ret

; Lambda: multiplier
lambda_multiplier:
    push rbp
    mov rbp, rsp
    mov rax, rdi    ; Return first param
    pop rbp
    ret

; Lambda: counter
lambda_counter:
    push rbp
    mov rbp, rsp
    xor rax, rax    ; Return 0
    pop rbp
    ret

_start:
    push rbp
    mov rbp, rsp
    sub rsp, 64

    ; Variable: x (numeric)
    mov qword [var_x], 0

    ; Variable: y (numeric)
    mov qword [var_y], 0

    ; Variable: double (func) [LAMBDA]
    lea rax, [lambda_double]
    mov [var_double], rax
    ; Params: n

    ; Variable: add (func) [LAMBDA]
    lea rax, [lambda_add]
    mov [var_add], rax
    ; Params: a, b

    ; Variable: multiplier (func) [LAMBDA]
    lea rax, [lambda_multiplier]
    mov [var_multiplier], rax
    ; Captures: x
    ; Params: n

    ; Variable: counter (func) [LAMBDA]
    lea rax, [lambda_counter]
    mov [var_counter], rax
    ; Captures: &y

    ; Variable: result (text)
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
