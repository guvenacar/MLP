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


global func_main
func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: text path
    sub rsp, 8         ; Allocate space for path
    mov rax, str_0
    mov [rbp-8], rax   ; Initialize path
    mov rax, [rbp-8]
    push rax
    mov rax, str_1
    push rax
    pop rsi
    pop rdi
    call mlp_file_write
    mov rax, [rbp-8]
    push rax
    mov rax, str_2
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-8]
    push rax
    mov rax, str_3
    push rax
    pop rsi
    pop rdi
    call mlp_file_append

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    pop rdi
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
    push rax
    pop rdi
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
str_4: db "Reading appended file...", 0
str_3: db "Line 3", 0
str_2: db "Line 2", 0
str_1: db "Line 1", 0
str_0: db "test_append.txt", 0
