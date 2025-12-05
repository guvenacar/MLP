; Result Type Module
section .data
    _res_msg: db 'Result Type OK!',10,0
; Result type definitions
; Result<sayi, metin>
; Result<sayi, metin>
section .text
    global _start
_start:
    mov rax, 1
    mov rdi, 1
    lea rsi, [_res_msg]
    mov rdx, 16
    syscall
    mov rax, 60
    xor rdi, rdi
    syscall
