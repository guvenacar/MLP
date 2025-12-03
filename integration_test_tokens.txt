.section .data
msg_lexer: .asciz "Lexer module compiled\n"

.section .text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp
    leaq msg_lexer(%rip), %rdi
    call printf@PLT
    xorq %rax, %rax
    popq %rbp
    ret
