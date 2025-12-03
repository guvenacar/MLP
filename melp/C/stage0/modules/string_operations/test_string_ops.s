section .data
    msg db "String Operations OK!", 10
    msg_len equ $ - msg

section .bss
    name resb 256  ; text
    greeting resb 256  ; text
    x resq 1    ; numeric

section .text
    global _start

_start:
    ; Variable: name (text)
    ; Variable: greeting (text)
    ; Variable: x (numeric)

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
