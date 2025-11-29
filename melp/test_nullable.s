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


_start:
    ; Get argc and argv from stack
    pop rdi               ; argc (first item on stack)
    mov rsi, rsp          ; argv (pointer to argv[0])
    call mlp_get_argv     ; Convert to MLP string array

    push rbp
    mov rbp, rsp

    ; Declaration: numeric maybe_num
    sub rsp, 8         ; Allocate space for maybe_num
    mov rax, 0
    mov [rbp-8], rax   ; Initialize maybe_num

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L0
    ; Then body

    ; Print statement
    mov rax, 1
    mov rdi, rax
    call print_number
.L0:

    ; Assignment: maybe_num = ...
    mov rax, 42
    mov [rbp-8], rax   ; Store to maybe_num

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    je .L2
    ; Then body

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_number
.L2:
    ; Declaration: numeric another
    sub rsp, 8         ; Allocate space for another
    mov rax, 100
    mov [rbp-16], rax   ; Initialize another

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-16]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    je .L4
    ; Then body

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_number
.L4:

    ; Assignment: another = ...
    mov rax, 0
    mov [rbp-16], rax   ; Store to another

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-16]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L6
    ; Then body

    ; Print statement
    mov rax, 2
    mov rdi, rax
    call print_number
.L6:

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
