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
    ; Declaration: text a
    sub rsp, 8         ; Allocate space for a
    mov rax, str_0
    mov [rbp-8], rax   ; Initialize a
    ; Declaration: text b
    sub rsp, 8         ; Allocate space for b
    mov rax, str_1
    mov [rbp-16], rax   ; Initialize b
    ; Declaration: text c
    sub rsp, 8         ; Allocate space for c
    mov rax, [rbp-8]
    push rax
    mov rax, str_2
    mov rsi, rax
    pop rdi
    call string_concat
    push rax
    mov rax, [rbp-16]
    mov rsi, rax
    pop rdi
    call string_concat
    mov [rbp-24], rax   ; Initialize c

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_string
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
str_2: db " ", 0
str_1: db "World", 0
str_0: db "Hello", 0
