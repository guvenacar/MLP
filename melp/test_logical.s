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


global func_test_logical
func_test_logical:
    push rbp
    mov rbp, rsp
    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, 5
    mov [rbp-8], rax   ; Initialize x
    ; Declaration: numeric y
    sub rsp, 8         ; Allocate space for y
    mov rax, 10
    mov [rbp-16], rax   ; Initialize y

    ; If statement
    ; Then body

    ; Print statement
    mov rax, 1
    mov rdi, rax
    call print_number
.L0:

    ; If statement
    ; Then body

    ; Print statement
    mov rax, 2
    mov rdi, rax
    call print_number
.L2:
    ; Declaration: numeric flag
    sub rsp, 8         ; Allocate space for flag
    mov rax, 1
    mov [rbp-24], rax   ; Initialize flag

    ; If statement

    ; Evaluate comparison

    ; Logical NOT
    mov rax, [rbp-24]
    test rax, rax
    setz al             ; Set AL to 1 if zero, 0 otherwise
    movzx rax, al       ; Zero-extend AL to RAX
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L4
    ; Then body

    ; Print statement
    mov rax, 3
    mov rdi, rax
    call print_number
.L4:
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

global func_main
func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: numeric result
    sub rsp, 8         ; Allocate space for result
    call func_test_logical
    mov [rbp-8], rax   ; Initialize result
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
