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
    ; Declaration: text code
    sub rsp, 8         ; Allocate space for code
    mov rax, str_0
    mov [rbp-8], rax   ; Initialize code
    ; Declaration: numeric len
    sub rsp, 8         ; Allocate space for len
    mov rax, [rbp-8]
    mov rdi, rax
    call mlp_string_length
    mov [rbp-16], rax   ; Initialize len

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_number
    ; Declaration: text first
    sub rsp, 8         ; Allocate space for first
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, 0
    mov rsi, rax
    mov rax, 1
    mov rdx, rax
    call mlp_substring
    mov [rbp-24], rax   ; Initialize first

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_string
    ; Declaration: numeric eq_pos
    sub rsp, 8         ; Allocate space for eq_pos
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, str_1
    mov rsi, rax
    call mlp_indexOf
    mov [rbp-32], rax   ; Initialize eq_pos

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_number
    ; Declaration: text num
    sub rsp, 8         ; Allocate space for num
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, 2
    mov rsi, rax
    mov rax, 2
    mov rdx, rax
    call mlp_substring
    mov [rbp-40], rax   ; Initialize num

    ; Print statement
    mov rax, [rbp-40]
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
    ; Declaration: numeric TOKEN_NUMERIC
    sub rsp, 8         ; Allocate space for TOKEN_NUMERIC
    mov rax, 1
    mov [rbp-8], rax   ; Initialize TOKEN_NUMERIC
    ; Declaration: numeric TOKEN_IDENTIFIER
    sub rsp, 8         ; Allocate space for TOKEN_IDENTIFIER
    mov rax, 2
    mov [rbp-16], rax   ; Initialize TOKEN_IDENTIFIER
    ; Declaration: numeric TOKEN_NUMBER
    sub rsp, 8         ; Allocate space for TOKEN_NUMBER
    mov rax, 3
    mov [rbp-24], rax   ; Initialize TOKEN_NUMBER
    ; Declaration: numeric TOKEN_ASSIGN
    sub rsp, 8         ; Allocate space for TOKEN_ASSIGN
    mov rax, 5
    mov [rbp-32], rax   ; Initialize TOKEN_ASSIGN
    ; Declaration: numeric TOKEN_PLUS
    sub rsp, 8         ; Allocate space for TOKEN_PLUS
    mov rax, 8
    mov [rbp-40], rax   ; Initialize TOKEN_PLUS
    ; Declaration: numeric TOKEN_PRINT
    sub rsp, 8         ; Allocate space for TOKEN_PRINT
    mov rax, 12
    mov [rbp-48], rax   ; Initialize TOKEN_PRINT
    ; Declaration: numeric TOKEN_EOF
    sub rsp, 8         ; Allocate space for TOKEN_EOF
    mov rax, 99
    mov [rbp-56], rax   ; Initialize TOKEN_EOF

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_1: db "=", 0
str_0: db "x=42", 0
