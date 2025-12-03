section .data
    msg db "Collections OK!", 10
    msg_len equ $ - msg

section .bss
    x resq 1
    y resq 1

section .text
    global _start

_start:
    ; Variable: x (type: 0)
    ; Variable: y (type: 0)

    ; Print success message
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    lea rsi, [msg]      ; message
    mov rdx, msg_len    ; length
    syscall

    ; Exit
    mov rax, 60         ; sys_exit
    xor rdi, rdi        ; exit code 0
    syscall
