section .data
    msg db "Pointer Module OK!", 10
    msg_len equ $ - msg

section .bss

section .text
    global _start

_start:

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
