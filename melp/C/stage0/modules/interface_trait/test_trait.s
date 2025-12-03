section .data
    msg: db "Interface/Trait OK!", 10, 0
    msg_len: equ $ - msg

section .bss

section .text
    global _start

; Trait: Drawable (0 methods)
Drawable_vtable:
    dq 0  ; vtable placeholder

_start:
    ; Initialize variables

    ; Print result
    mov rax, 1
    mov rdi, 1
    lea rsi, [msg]
    mov rdx, msg_len
    syscall

    ; Exit
    mov rax, 60
    xor rdi, rdi
    syscall
