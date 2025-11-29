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

    ; Declaration: text name
    sub rsp, 8         ; Allocate space for name
    mov rax, str_0
    mov [rbp-8], rax   ; Initialize name
    ; Declaration: numeric age
    sub rsp, 8         ; Allocate space for age
    mov rax, 25
    mov [rbp-16], rax   ; Initialize age
    ; Declaration: text greeting
    sub rsp, 8         ; Allocate space for greeting
    mov rax, str_1
    push rax
    mov rax, [rbp-8]
    mov rsi, rax
    pop rdi
    call string_concat
    push rax
    mov rax, str_2
    mov rsi, rax
    pop rdi
    call string_concat
    mov [rbp-24], rax   ; Initialize greeting

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_string
    ; Declaration: text info
    sub rsp, 8         ; Allocate space for info
    mov rax, str_3
    push rax
    mov rax, [rbp-8]
    mov rsi, rax
    pop rdi
    call string_concat
    push rax
    mov rax, str_4
    mov rsi, rax
    pop rdi
    call string_concat
    push rax
    mov rax, [rbp-16]
    mov rdi, rax
    call int_to_string
    mov rsi, rax
    pop rdi
    call string_concat
    mov [rbp-32], rax   ; Initialize info

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_string
    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, 10
    mov [rbp-40], rax   ; Initialize x
    ; Declaration: numeric y
    sub rsp, 8         ; Allocate space for y
    mov rax, 20
    mov [rbp-48], rax   ; Initialize y
    ; Declaration: text result
    sub rsp, 8         ; Allocate space for result
    mov rax, str_5
    push rax
    mov rax, [rbp-40]
    push rax
    mov rax, [rbp-48]
    mov rbx, rax
    pop rax
    add rax, rbx
    mov rdi, rax
    call int_to_string
    mov rsi, rax
    pop rdi
    call string_concat
    push rax
    mov rax, str_6
    mov rsi, rax
    pop rdi
    call string_concat
    push rax
    mov rax, [rbp-40]
    push rax
    mov rax, [rbp-48]
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov rdi, rax
    call int_to_string
    mov rsi, rax
    pop rdi
    call string_concat
    mov [rbp-56], rax   ; Initialize result

    ; Print statement
    mov rax, [rbp-56]
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_6: db ", Product: ", 0
str_5: db "Sum: ", 0
str_4: db ", Age: ", 0
str_3: db "Name: ", 0
str_2: db "!", 0
str_1: db "Hello, ", 0
str_0: db "Alice", 0
