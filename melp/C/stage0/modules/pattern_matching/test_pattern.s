section .data
    msg: db "Pattern Matching OK!", 10, 0
    msg_len: equ $ - msg
    str_result: db "unknown", 0

section .bss
    value: resq 1
    result: resq 1
    x: resq 1

section .text
    global _start

_start:
    mov qword [value], 42
    lea rax, [str_result]
    mov [result], rax
    mov qword [x], 100

    ; Print success message
    mov rax, 1
    mov rdi, 1
    lea rsi, [msg]
    mov rdx, msg_len
    syscall

    ; Exit
    mov rax, 60
    xor rdi, rdi
    syscall
