section .data
    msg: db "Null Safety OK!", 10, 0
    msg_len: equ $ - msg
    str_name: db "Test", 0

section .bss
    x: resq 1
    name: resq 1
    result: resq 1

section .text
    global _start

_start:
    mov qword [x], 42
    lea rax, [str_name]
    mov [name], rax
    mov qword [result], 100

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
