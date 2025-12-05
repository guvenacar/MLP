section .data
    msg: db "Module System OK!", 10, 0
    msg_len: equ $ - msg

section .bss

section .text
    global _start

_start:
    ; Import: std
    ; Import: std

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
