section .data

section .bss

section .text
    global _start

_start:

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
