section .data
    msg: db "Memory Management OK!", 10, 0
    msg_len: equ $ - msg

section .bss
    ptr: resq 1
    size: resq 1
    buffer: resq 1

section .text
    global _start

_start:
    ; Memory allocation: alloc_expr
    ; Memory allocation: new_expr
    ; Defer statement
    ; Memory free: free_exprptr
    ; Memory free: free_exprptr
    ; Memory free: delete_exprmyobj
    ; GC hint

    ; Print success message
    mov rax, 1
    mov rdi, 1
    lea rsi, [msg]
    mov rdx, msg_len
    syscall

    ; Exit
    mov rax, 60
    xor rdi, rdi
    syscall
