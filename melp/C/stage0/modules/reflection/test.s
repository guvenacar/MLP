; MLP Reflection Module - Generated Assembly
; Types: 3, Fields: 3, Methods: 3

section .data
    msg: db 'Reflection OK!', 10, 0
    msg_len equ $ - msg
    ; Type: Person (category: 1)
    ; Type: Color (category: 2)
    ; Type: Drawable (category: 6)

section .bss
    ; Runtime type info storage

section .text
    global _start

_start:
    mov rax, 1
    mov rdi, 1
    lea rsi, [rel msg]
    mov rdx, msg_len
    syscall

    mov rax, 60
    xor rdi, rdi
    syscall
