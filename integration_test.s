.section .data
msg_codegen: .asciz "Codegen module compiled\n"

.section .text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp
    leaq msg_codegen(%rip), %rdi
    call printf@PLT
    xorq %rax, %rax
    popq %rbp
    ret
