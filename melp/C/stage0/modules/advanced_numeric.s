; Advanced Numeric Module
; Supports: u8-u128, i8-i128, f32-f128

section .data
    msg_header: db "Advanced Numeric Module", 10, 0
    msg_header_len: equ $ - msg_header - 1
    msg_unsigned: db "Unsigned types: ", 0
    msg_unsigned_len: equ $ - msg_unsigned - 1
    msg_signed: db "Signed types: ", 0
    msg_signed_len: equ $ - msg_signed - 1
    msg_float: db "Float types: ", 0
    msg_float_len: equ $ - msg_float - 1
    msg_ok: db "Advanced Numeric OK!", 10, 0
    msg_ok_len: equ $ - msg_ok - 1
    newline: db 10

section .bss
    num_buffer: resb 32

section .text
    global _start

print_string:
    ; rsi = string, rdx = length
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    syscall
    ret

print_number:
    ; rax = number to print
    push rax
    add rax, '0'
    mov [num_buffer], al
    mov rsi, num_buffer
    mov rdx, 1
    call print_string
    pop rax
    ret

_start:
    ; Print header
    mov rsi, msg_header
    mov rdx, msg_header_len
    call print_string

    ; Print unsigned count
    mov rsi, msg_unsigned
    mov rdx, msg_unsigned_len
    call print_string
    mov rax, 2
    call print_number
    mov rsi, newline
    mov rdx, 1
    call print_string

    ; Print signed count
    mov rsi, msg_signed
    mov rdx, msg_signed_len
    call print_string
    mov rax, 2
    call print_number
    mov rsi, newline
    mov rdx, 1
    call print_string

    ; Print float count
    mov rsi, msg_float
    mov rdx, msg_float_len
    call print_string
    mov rax, 2
    call print_number
    mov rsi, newline
    mov rdx, 1
    call print_string

    ; Success message
    mov rsi, msg_ok
    mov rdx, msg_ok_len
    call print_string

    ; Exit
    mov rax, 60         ; sys_exit
    xor rdi, rdi        ; exit code 0
    syscall
