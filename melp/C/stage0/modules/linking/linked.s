; MLP Linking Module - Merged Assembly
; Target: x86-64 Linux

section .data
    msg1: db "Hello"
    msg2: db "World"

section .text
    global _start
_start:
    mov rax, 1
    mov rbx, 2

    ; Ensure clean exit
    mov rax, 60
    xor rdi, rdi
    syscall
