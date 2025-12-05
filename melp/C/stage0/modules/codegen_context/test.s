; Codegen Context Module
section .data
    _ctx_msg: db 'Codegen Context OK!',10,0
section .bss
section .text
    global _start
_start:
    mov rax, 1
    mov rdi, 1
    lea rsi, [_ctx_msg]
    mov rdx, 20
    syscall
    mov rax, 60
    xor rdi, rdi
    syscall
