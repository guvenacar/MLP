section .data
    print_str_0 db "=== Complete MLP Test ===", 10, 0
    print_str_0_len equ $ - print_str_0
    print_str_1 db "1. Variable declaration:", 10, 0
    print_str_1_len equ $ - print_str_1
    print_str_2 db "2. For loop (sum 1 to 5):", 10, 0
    print_str_2_len equ $ - print_str_2
    print_str_3 db "3. While loop (count to 3):", 10, 0
    print_str_3_len equ $ - print_str_3
    print_str_4 db "4. Function call:", 10, 0
    print_str_4_len equ $ - print_str_4
    print_str_5 db "factorial called", 10, 0
    print_str_5_len equ $ - print_str_5
    print_str_6 db "=== All Tests Passed ===", 10, 0
    print_str_6_len equ $ - print_str_6

section .bss
    var_x resq 1 ; INT64
    var_sum resq 1 ; INT64
    var_count resq 1 ; INT64
    var_i resq 1 ; FOR loop variable
    var_n resq 1 ; PARAM (function factorial)

section .text
    global _start

; Print integer in rax to stdout
print_int:
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    mov rsi, rsp
    sub rsp, 32          ; Buffer for digits
    mov rdi, rsp
    add rdi, 31          ; End of buffer
    mov byte [rdi], 10   ; Newline
    dec rdi
    mov rbx, 10
    test rax, rax
    jns .print_positive
    neg rax
    mov rcx, 1           ; Negative flag
    jmp .print_loop
.print_positive:
    xor rcx, rcx         ; Positive
.print_loop:
    xor rdx, rdx
    div rbx
    add dl, '0'
    mov [rdi], dl
    dec rdi
    test rax, rax
    jnz .print_loop
    test rcx, rcx
    jz .print_write
    mov byte [rdi], '-'
    dec rdi
.print_write:
    inc rdi
    mov rax, 1           ; sys_write
    mov rsi, rdi
    lea rdx, [rsp + 32]
    sub rdx, rdi
    mov rdi, 1           ; stdout
    syscall
    add rsp, 32
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    ret

; ============ Function: factorial ============
factorial:
    push rbp
    mov rbp, rsp
    sub rsp, 128
    mov [rbp-8], rdi  ; param n
    mov [var_n], rdi  ; copy to global
    ; if statement
    mov rax, [var_n]
    push rax
    mov rax, 2
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L1
.L1:
    ; return
    mov rax, [var_n]
    mov rsp, rbp
    pop rbp
    ret
    ; default return
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

_start:
    ; x = 10 (INT64)
    mov rax, 10
    mov [var_x], rax
    ; sum = 0 (INT64)
    mov rax, 0
    mov [var_sum], rax
    ; count = 0 (INT64)
    mov rax, 0
    mov [var_count], rax
    ; print string
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    mov rsi, print_str_0
    mov rdx, print_str_0_len
    syscall
    ; print string
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    mov rsi, print_str_1
    mov rdx, print_str_1_len
    syscall
    ; print expression
    mov rax, [var_x]
    call print_int
    ; print string
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    mov rsi, print_str_2
    mov rdx, print_str_2_len
    syscall
    ; for loop
    mov rax, 1
    mov [var_i], rax
.L2:
    mov rax, [var_i]
    push rax
    mov rax, 5
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jg .L3
    ; assignment: sum = expr
    mov rax, [var_sum]
    push rax
    mov rax, [var_i]
    pop rbx
    add rax, rbx
    mov [var_sum], rax
    mov rax, [var_i]
    inc rax
    mov [var_i], rax
    jmp .L2
.L3:
    ; print expression
    mov rax, [var_sum]
    call print_int
    ; print string
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    mov rsi, print_str_3
    mov rdx, print_str_3_len
    syscall
    ; while loop
.L4:
    mov rax, [var_count]
    push rax
    mov rax, 3
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L5
    ; assignment: count = expr
    mov rax, [var_count]
    push rax
    mov rax, 1
    pop rbx
    add rax, rbx
    mov [var_count], rax
    jmp .L4
.L5:
    ; print expression
    mov rax, [var_count]
    call print_int
    ; print string
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    mov rsi, print_str_4
    mov rdx, print_str_4_len
    syscall
    ; Call function: factorial
    mov rax, 5
    push rax
    pop rdi
    call factorial
    ; print string
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    mov rsi, print_str_5
    mov rdx, print_str_5_len
    syscall
    ; print string
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    mov rsi, print_str_6
    mov rdx, print_str_6_len
    syscall

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
