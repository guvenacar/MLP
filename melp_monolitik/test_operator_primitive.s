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


global func_numeric_operator_add
func_numeric_operator_add:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param a
    sub rsp, 8
    mov [rbp-8], rax   ; Store param a locally
    mov rax, [rbp+24]   ; Load param b
    sub rsp, 8
    mov [rbp-16], rax   ; Store param b locally
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    add rax, rbx
    push rax
    mov rax, 100
    mov rbx, rax
    pop rax
    add rax, rbx
    mov rsp, rbp
    pop rbp
    ret

global func_numeric_operator_mul
func_numeric_operator_mul:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param a
    sub rsp, 8
    mov [rbp-8], rax   ; Store param a locally
    mov rax, [rbp+24]   ; Load param b
    sub rsp, 8
    mov [rbp-16], rax   ; Store param b locally
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    imul rax, rbx
    push rax
    mov rax, 2
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov rsp, rbp
    pop rbp
    ret

global func_main
func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x

    ; Assignment: x = ...
    mov rax, 10
    mov [rbp-8], rax   ; Store to x
    ; Declaration: numeric y
    sub rsp, 8         ; Allocate space for y

    ; Assignment: y = ...
    mov rax, 5
    mov [rbp-16], rax   ; Store to y
    ; Declaration: numeric result1
    sub rsp, 8         ; Allocate space for result1

    ; Assignment: result1 = ...
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-8]
    push rax
    call func_numeric_operator_add
    add rsp, 16
    mov [rbp-24], rax   ; Store to result1

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_number
    ; Declaration: numeric result2
    sub rsp, 8         ; Allocate space for result2

    ; Assignment: result2 = ...
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-8]
    push rax
    call func_numeric_operator_mul
    add rsp, 16
    mov [rbp-32], rax   ; Store to result2

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number
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
