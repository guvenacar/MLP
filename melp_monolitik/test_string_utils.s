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
    ; Declaration: text str
    sub rsp, 8         ; Allocate space for str
    mov rax, str_0
    mov [rbp-8], rax   ; Initialize str

    ; Print statement
    mov rax, [rbp-8]
    push rax
    pop rdi
    call mlp_string_length
    mov rdi, rax
    call print_number
    ; Declaration: text sub
    sub rsp, 8         ; Allocate space for sub
    mov rax, [rbp-8]
    push rax
    mov rax, 0
    push rax
    mov rax, 5
    push rax
    pop rdx
    pop rsi
    pop rdi
    call mlp_substring
    mov [rbp-16], rax   ; Initialize sub

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_string
    ; Declaration: text sub2
    sub rsp, 8         ; Allocate space for sub2
    mov rax, [rbp-8]
    push rax
    mov rax, 6
    push rax
    mov rax, 5
    push rax
    pop rdx
    pop rsi
    pop rdi
    call mlp_substring
    mov [rbp-24], rax   ; Initialize sub2

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_string
    ; Declaration: numeric idx
    sub rsp, 8         ; Allocate space for idx
    mov rax, [rbp-8]
    push rax
    mov rax, str_1
    push rax
    pop rsi
    pop rdi
    call mlp_indexOf
    mov [rbp-32], rax   ; Initialize idx

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_number
    ; Declaration: numeric idx2
    sub rsp, 8         ; Allocate space for idx2
    mov rax, [rbp-8]
    push rax
    mov rax, str_2
    push rax
    pop rsi
    pop rdi
    call mlp_indexOf
    mov [rbp-40], rax   ; Initialize idx2

    ; Print statement
    mov rax, [rbp-40]
    mov rdi, rax
    call print_number
    ; Declaration: text ch
    sub rsp, 8         ; Allocate space for ch
    mov rax, [rbp-8]
    push rax
    mov rax, 0
    push rax
    pop rsi
    pop rdi
    call mlp_charAt
    mov [rbp-48], rax   ; Initialize ch

    ; Print statement
    mov rax, [rbp-48]
    mov rdi, rax
    call print_string
    ; Declaration: text ch2
    sub rsp, 8         ; Allocate space for ch2
    mov rax, [rbp-8]
    push rax
    mov rax, 6
    push rax
    pop rsi
    pop rdi
    call mlp_charAt
    mov [rbp-56], rax   ; Initialize ch2

    ; Print statement
    mov rax, [rbp-56]
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
str_2: db "xyz", 0
str_1: db "World", 0
str_0: db "Hello World", 0
