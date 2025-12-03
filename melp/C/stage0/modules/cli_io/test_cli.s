; MLP Stage 0 - CLI I/O Module Output
; Auto-generated x86-64 assembly

section .data
    msg_ok db 'CLI I/O OK!', 10, 0
    msg_ok_len equ $ - msg_ok

section .bss
    input_buffer resb 1024

section .text
    global _start

; sys_read wrapper
_cli_read:
    mov rax, 0
    syscall
    ret

; sys_write wrapper
_cli_write:
    mov rax, 1
    syscall
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
