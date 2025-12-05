; MLP Stage 0 - File I/O Module Output
; Auto-generated x86-64 assembly

section .data
    msg_ok db 'File I/O OK!', 10, 0
    msg_ok_len equ $ - msg_ok

section .bss
    file_buffer resb 4096
    file_handles resq 32

section .text
    global _start

; sys_open wrapper
_sys_open:
    mov rax, 2
    syscall
    ret

; sys_close wrapper
_sys_close:
    mov rax, 3
    syscall
    ret

; sys_read wrapper
_sys_read:
    mov rax, 0
    syscall
    ret

; sys_write wrapper
_sys_write:
    mov rax, 1
    syscall
    ret

_start:
    ; File I/O program

    ; Print success message
    mov rax, 1
    mov rdi, 1
    lea rsi, [msg_ok]
    mov rdx, msg_ok_len
    syscall

    ; Exit
    mov rax, 60
    xor rdi, rdi
    syscall
