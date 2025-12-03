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
extern mlp_substring
extern mlp_indexOf
extern mlp_charAt
extern mlp_string_length
extern mlp_get_argv
extern mlp_get_argc
global _start


func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: text source
    sub rsp, 8         ; Allocate space for source
    mov rax, str_0
    mov rdi, rax
    call mlp_file_read
    mov [rbp-8], rax   ; Initialize source
    ; Declaration: numeric len
    sub rsp, 8         ; Allocate space for len
    mov rax, [rbp-8]
    mov rdi, rax
    call mlp_string_length
    mov [rbp-16], rax   ; Initialize len

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_number

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-16]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jle .L0
    ; Then body
    ; Declaration: text first
    sub rsp, 8         ; Allocate space for first
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, 0
    mov rsi, rax
    call mlp_charAt
    mov [rbp-24], rax   ; Initialize first

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_string
.L0:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-16]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jle .L2
    ; Then body
    ; Declaration: text second
    sub rsp, 8         ; Allocate space for second
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, 1
    mov rsi, rax
    call mlp_charAt
    mov [rbp-32], rax   ; Initialize second

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_string
.L2:
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

_start:
    ; Get argc and argv from stack
    pop rdi               ; argc (first item on stack)
    mov rsi, rsp          ; argv (pointer to argv[0])
    call mlp_get_argv     ; Convert to MLP string array

    push rbp
    mov rbp, rsp

    ; Call main function
    call func_main

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_0: db "compiler/test_simple.mlp", 0
