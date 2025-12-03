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

    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, 0
    mov [rbp-8], rax   ; Initialize x

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
    mov rax, 10
    mov rdi, rax
    call print_number
.L0:

    ; Assignment: x = ...
    mov rax, 5
    mov [rbp-8], rax   ; Store to x

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_number
    ; Declaration: decimal y
    sub rsp, 8         ; Allocate space for y
    mov rax, 0
    mov [rbp-16], rax   ; Initialize y

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-16]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L2
    ; Then body

    ; Print statement
    mov rax, 20
    mov rdi, rax
    call print_number
.L2:

    ; Assignment: y = ...
    mov rax, 3
    mov [rbp-16], rax   ; Store to y

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_number
    ; Declaration: boolean flag
    sub rsp, 8         ; Allocate space for flag
    mov rax, 0
    mov [rbp-24], rax   ; Initialize flag

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L4
    ; Then body

    ; Print statement
    mov rax, 30
    mov rdi, rax
    call print_number
.L4:

    ; Assignment: flag = ...
    mov rax, 1
    mov [rbp-24], rax   ; Store to flag

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_number
    ; Declaration: text msg
    sub rsp, 8         ; Allocate space for msg
    mov rax, 0
    mov [rbp-32], rax   ; Initialize msg

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-32]
    mov rdi, rax
    push rdi
    mov rax, 0
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L6
    ; Then body

    ; Print statement
    mov rax, 40
    mov rdi, rax
    call print_number
.L6:
    ; Declaration: numeric z
    sub rsp, 8         ; Allocate space for z
    mov rax, 7
    mov [rbp-40], rax   ; Initialize z

    ; Print statement
    mov rax, [rbp-40]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
