; MLP Preprocessor Module - Generated Assembly
; File: /home/pardus/projeler/MLP/MLP/test_preprocessor.mlp
; Macros defined: 8
; Includes: 0

section .data
    msg: db 'Preprocessor OK!', 10, 0
    msg_len equ $ - msg
    ; Macro PI = 3.14

section .bss
    ; Reserved space for macro expansions

section .text
    global _start

_start:
    ; Print success message
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    lea rsi, [rel msg]  ; message
    mov rdx, msg_len    ; length
    syscall

    ; Exit
    mov rax, 60         ; sys_exit
    xor rdi, rdi        ; status 0
    syscall
