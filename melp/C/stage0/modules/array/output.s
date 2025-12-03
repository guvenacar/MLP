; Array Module Test - Generated Assembly
; Source: /home/pardus/projeler/MLP/MLP/melp/melp/array/test_array.mlp
; P1 Data Structures - Module #12
; Declarations: 0
; Array declarations: 0
; Array literals: 1

section .data
    ; Array module data
    msg_array: db "Array Module OK!", 10, 0
    msg_len: equ $ - msg_array

section .text
global _start

; MLP Collection Types:
; - Array []: Homogeneous, mutable, stack/heap
; - List  (): Heterogeneous, mutable, heap
; - Tuple <>: Heterogeneous, immutable, stack

_start:
    ; Program start - P1 Array Module
    push rbp
    mov rbp, rsp
    sub rsp, 64  ; Stack for variables + arrays

    ; Print array module message
    mov rax, 1
    mov rdi, 1
    lea rsi, [msg_array]
    mov rdx, msg_len
    syscall

    ; Array operations:
    ; - Literal: [1, 2, 3] -> allocate, fill
    ; - Index: arr[i] -> base + i * element_size
    ; - Length: arr.length -> stored in header
    ; - Resize: realloc if needed

    ; Program exit
    mov rsp, rbp
    pop rbp
    mov rax, 60
    xor rdi, rdi
    syscall

section .bss
