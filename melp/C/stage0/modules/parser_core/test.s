; Parser Core Output
section .data
    msg db 'Parser Core OK!', 10, 0
    msg_len equ $ - msg
section .text
    global _start
_start:
    mov rax, 1
    mov rdi, 1
    lea rsi, [msg]
    mov rdx, msg_len
    syscall
    mov rax, 60
    xor rdi, rdi
    syscall
