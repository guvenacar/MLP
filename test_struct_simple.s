; Struct Module Test - Generated Assembly
; Source: test_struct_simple.mlp
; P1 Data Structures - Module #11
; Variables: 1
; Struct definitions: 2
; Field accesses: 0

section .data
    ; Struct module data
    msg_struct: db "Struct Module OK!", 10, 0
    msg_len: equ $ - msg_struct

section .text
global _start

; Struct memory layout:
; - Each struct aligned to 8 bytes
; - Fields stored sequentially
; - Access via base + offset

_start:
    ; Program start - P1 Struct Module
    push rbp
    mov rbp, rsp
    sub rsp, 40  ; Stack for variables + structs

    ; Print struct module message
    mov rax, 1
    mov rdi, 1
    lea rsi, [msg_struct]
    mov rdx, msg_len
    syscall

    ; Struct operations:
    ; - Define struct: allocate template
    ; - Instantiate: allocate memory, copy template
    ; - Field access: base + offset
    ; - Nested: recursive offset calculation

    ; Program exit
    mov rsp, rbp
    pop rbp
    mov rax, 60
    xor rdi, rdi
    syscall

section .bss
    x: resq 1
    ; Struct instances would be allocated here
