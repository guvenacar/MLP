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


global func_Circle_draw
func_Circle_draw:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param c
    sub rsp, 8
    mov [rbp-8], rax   ; Store param c locally

    ; Print statement
    mov rax, 5
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-8]   ; Load c.radius
    mov rdi, rax
    call print_number
    mov rsp, rbp
    pop rbp
    ret

global func_Circle_getArea
func_Circle_getArea:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param c
    sub rsp, 8
    mov [rbp-8], rax   ; Store param c locally
    mov rax, [rbp-8]   ; Load c.radius
    push rax
    mov rax, [rbp-8]   ; Load c.radius
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
    ; Declaration: Circle c (struct, size=8 bytes)
    sub rsp, 8         ; Allocate space for struct c
    mov qword [rbp-8], 0   ; Initialize c.radius

    ; Field assignment: c.radius = ...
    mov rax, 10
    mov [rbp-8], rax   ; Store to c.radius
    mov rax, [rbp-8]
    push rax
    call func_Circle_draw
    add rsp, 8
    ; Declaration: numeric area
    sub rsp, 8         ; Allocate space for area
    mov rax, [rbp-8]
    push rax
    call func_Circle_getArea
    add rsp, 8
    mov [rbp-16], rax   ; Initialize area

    ; Print statement
    mov rax, [rbp-16]
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
