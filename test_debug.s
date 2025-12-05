; MLP Stage 0 - Debug Features Module Output

section .data
    msg_ok db 'Debug OK!', 10, 0
    msg_ok_len equ $ - msg_ok

section .text
    global _start

_debug_assert:
    ret

_debug_trace:
    ret

_start:
    mov rax, 1
    mov rdi, 1
    lea rsi, [msg_ok]
    mov rdx, msg_ok_len
    syscall
    mov rax, 60
    xor rdi, rdi
    syscall
