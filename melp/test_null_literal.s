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
extern mlp_range
extern mlp_range1
extern mlp_range2
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


global func_main
func_main:
    push rbp
    mov rbp, rsp

    ; Print statement
    mov rax, str_0
    mov rdi, rax
    call print_string
    ; Declaration: numeric result1
    sub rsp, 8         ; Allocate space for result1

    ; Null coalescing (??)
    mov rax, 0
    test rax, rax       ; Check if null
    jz .L_default_0    ; If null, use default
    jmp .L_end_1
.L_default_0:
    mov rax, 100
.L_end_1:
    mov [rbp-8], rax   ; Initialize result1

    ; Print statement
    mov rax, str_1
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_number
    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, 0
    mov [rbp-16], rax   ; Initialize x

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
    mov rax, str_2
    mov rdi, rax
    call print_string
.L2:

    ; Print statement
    mov rax, str_3
    mov rdi, rax
    call print_string
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
    call func_main

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_3: db "=== Tests complete ===", 0
str_2: db "PASS: x is zero (null-like)", 0
str_1: db "null ?? 100 = ", 0
str_0: db "=== Null Literal Tests ===", 0
