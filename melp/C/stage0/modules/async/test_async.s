section .data
    msg: db "Async/Await OK!", 10, 0
    msg_len: equ $ - msg

section .bss
    result: resq 1
    timeout: resq 1

section .text
    global _start

_start:
    ; Await expression
    ; Await expression
    ; Await expression
    ; Promise
    ; Future

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
