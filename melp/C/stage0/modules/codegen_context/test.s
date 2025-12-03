; Codegen Context Module
section .data
    _ctx_msg: db 'Codegen Context OK!',10,0
section .bss
    _var_a: resq 1
    _var_b: resq 1
    _var_x: resq 1
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
