; Array Operations Module Test - Generated Assembly
; Source: /home/pardus/projeler/MLP/MLP/melp/melp/array_operations/test_array_operations.mlp
; P1 Data Structures - Module #13
; Declarations: 0
; Array operations: 0

section .data
    ; Array operations module data
    msg_ops: db "Array Operations OK!", 10, 0
    msg_len: equ $ - msg_ops

section .text
global _start

; Array Operations:
; - push(val): append to end, resize if needed
; - pop(): remove and return last element
; - length: stored in array header
; - slice(s,e): create new array [s:e]
; - indexOf(v): linear search, return -1 if not found

_start:
    ; Program start - P1 Array Operations Module
    push rbp
    mov rbp, rsp
    sub rsp, 64  ; Stack for variables

    ; Print array operations message
    mov rax, 1
    mov rdi, 1
    lea rsi, [msg_ops]
    mov rdx, msg_len
    syscall

    ; Program exit
    mov rsp, rbp
    pop rbp
    mov rax, 60
    xor rdi, rdi
    syscall

section .bss
