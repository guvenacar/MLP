.section .data
msg_memory: .asciz "Memory module compiled\n"

.section .text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp
    leaq msg_memory(%rip), %rdi
    call printf@PLT
    xorq %rax, %rax
    popq %rbp
    ret
