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
    ; Declaration: text source_code
    sub rsp, 8         ; Allocate space for source_code
    mov rax, str_0
    mov rdi, rax
    call mlp_file_read
    mov [rbp-8], rax   ; Initialize source_code
    ; Declaration: numeric source_len
    sub rsp, 8         ; Allocate space for source_len
    mov rax, [rbp-8]
    mov rdi, rax
    call mlp_string_length
    mov [rbp-16], rax   ; Initialize source_len
    ; Declaration: numeric pos
    sub rsp, 8         ; Allocate space for pos
    mov rax, 0
    mov [rbp-24], rax   ; Initialize pos
    ; Declaration: numeric token_count
    sub rsp, 8         ; Allocate space for token_count
    mov rax, 0
    mov [rbp-32], rax   ; Initialize token_count
    ; Declaration: numeric[] token_types (dynamic array, pointer)
    sub rsp, 8         ; Allocate space for pointer token_types
    mov rax, 100
    mov rdi, rax
    call mlp_array_alloc
    mov [rbp-40], rax   ; Store array pointer to token_types
    ; Declaration: text[] token_values (dynamic array, pointer)
    sub rsp, 8         ; Allocate space for pointer token_values
    mov rax, 100
    mov rdi, rax
    call mlp_array_alloc
    mov [rbp-48], rax   ; Store array pointer to token_values
    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, 42
    mov [rbp-56], rax   ; Initialize x

    ; Print statement
    mov rax, [rbp-56]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-16]
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
    ; Declaration: numeric TOKEN_NUMBER
    sub rsp, 8         ; Allocate space for TOKEN_NUMBER
    mov rax, 3
    mov [rbp-8], rax   ; Initialize TOKEN_NUMBER
    ; Declaration: numeric TOKEN_IDENTIFIER
    sub rsp, 8         ; Allocate space for TOKEN_IDENTIFIER
    mov rax, 5
    mov [rbp-16], rax   ; Initialize TOKEN_IDENTIFIER
    ; Declaration: numeric TOKEN_PRINT
    sub rsp, 8         ; Allocate space for TOKEN_PRINT
    mov rax, 6
    mov [rbp-24], rax   ; Initialize TOKEN_PRINT
    ; Declaration: numeric TOKEN_ASSIGN
    sub rsp, 8         ; Allocate space for TOKEN_ASSIGN
    mov rax, 20
    mov [rbp-32], rax   ; Initialize TOKEN_ASSIGN
    ; Declaration: numeric TOKEN_LPAREN
    sub rsp, 8         ; Allocate space for TOKEN_LPAREN
    mov rax, 25
    mov [rbp-40], rax   ; Initialize TOKEN_LPAREN
    ; Declaration: numeric TOKEN_RPAREN
    sub rsp, 8         ; Allocate space for TOKEN_RPAREN
    mov rax, 26
    mov [rbp-48], rax   ; Initialize TOKEN_RPAREN
    ; Declaration: numeric TOKEN_EOF
    sub rsp, 8         ; Allocate space for TOKEN_EOF
    mov rax, 0
    mov [rbp-56], rax   ; Initialize TOKEN_EOF

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_0: db "compiler/test_simple.mlp", 0
