.section .data
msg_parser: .asciz "Parser module compiled\n"

.section .text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp
    leaq msg_parser(%rip), %rdi
    call printf@PLT
    xorq %rax, %rax
    popq %rbp
    ret
