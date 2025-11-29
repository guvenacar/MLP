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
extern malloc
extern free
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
extern setjmp
extern strcmp
extern mlp_exception_push
extern mlp_exception_pop
extern mlp_throw
extern mlp_exception_type
extern mlp_exception_message
extern mlp_exception_code
extern mlp_exception_has_handler
extern mlp_exception_has_parent_handler
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

    ; Print statement
    mov rax, [rbp-8]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0      ; Default false
    mov rbx, 1      ; True value
    cmove rax, rbx  ; If equal, set rax=1
    mov rdi, rax
    call print_number
    ; Declaration: numeric y
    sub rsp, 8         ; Allocate space for y
    mov rax, 42
    mov [rbp-16], rax   ; Initialize y

    ; Print statement
    mov rax, [rbp-16]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0      ; Default false
    mov rbx, 1      ; True value
    cmove rax, rbx  ; If equal, set rax=1
    mov rdi, rax
    call print_number

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
    je .L2
    ; Then body

    ; Print statement
    mov rax, 100
    mov rdi, rax
    call print_number
.L2:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L4
    ; Then body

    ; Print statement
    mov rax, 200
    mov rdi, rax
    call print_number
.L4:

    ; Assignment: x = ...
    mov rax, 99
    mov [rbp-8], rax   ; Store to x

    ; Print statement
    mov rax, [rbp-8]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0      ; Default false
    mov rbx, 1      ; True value
    cmove rax, rbx  ; If equal, set rax=1
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-8]
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
