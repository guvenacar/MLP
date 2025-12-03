; MLP Preprocessor Module - Generated Assembly
; File: test.mlp
; Macros defined: 10
; Includes: 1

section .data
    msg: db 'Preprocessor OK!', 10, 0
    msg_len equ $ - msg
    ; Macro PI = 3.14
    ; Macro VERSION = 1.0

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
