section .data
    ; String literals will be added here

section .text
extern print_number
extern print_string
extern string_length
extern string_concat
extern string_equal
extern string_not_equal
extern int_to_string
extern mlp_array_alloc
extern mlp_array_free
extern mlp_array_length
extern mlp_array_resize
extern mlp_file_read
extern mlp_file_write
extern mlp_file_exists
extern mlp_file_append
global _start


func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: text path
    sub rsp, 8         ; Allocate space for path
    mov rax, str_0
    mov [rbp-8], rax   ; Initialize path
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, str_1
    mov rsi, rax
    call mlp_file_write
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, str_2
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, str_3
    mov rsi, rax
    call mlp_file_append

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    call mlp_file_exists
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L0
    ; Then body

    ; Print statement
    mov rax, str_4
    mov rdi, rax
    call print_string
    ; Declaration: text content
    sub rsp, 8         ; Allocate space for content
    mov rax, [rbp-8]
    mov rdi, rax
    call mlp_file_read
    mov [rbp-16], rax   ; Initialize content

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_string
.L0:
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

_start:
    push rbp
    mov rbp, rsp

    call func_main

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_4: db "Reading appended file...", 0
str_3: db "Line 3", 0
str_2: db "Line 2", 0
str_1: db "Line 1", 0
str_0: db "test_append.txt", 0
