; MLP FFI Module Assembly

section .data
    msg_ok: db "FFI OK!", 10, 0
    msg_ok_len: equ $ - msg_ok

    ; Extern functions: 2
    ; Link directives: 0
    ; Unsafe blocks: 1
    ; Raw pointers: 2

section .text
    global _start

_start:
    ; Op 0: c_function
    ; Op 1: c_malloc
    ; Op 2: unsafe
    ; Op 3: *const
    ; Op 4: c_char
    ; Op 5: *mut
    ; Op 6: c_int

    mov rax, 1
    mov rdi, 1
    mov rsi, msg_ok
    mov rdx, msg_ok_len
    syscall

    mov rax, 60
    xor rdi, rdi
    syscall
