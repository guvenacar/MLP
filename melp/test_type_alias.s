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


_start:
    ; Get argc and argv from stack
    pop rdi               ; argc (first item on stack)
    mov rsi, rsp          ; argv (pointer to argv[0])
    call mlp_get_argv     ; Convert to MLP string array

    push rbp
    mov rbp, rsp

    ; Declaration: numeric user_id
    sub rsp, 8         ; Allocate space for user_id
    mov rax, 42
    mov [rbp-8], rax   ; Initialize user_id
    ; Declaration: numeric final_score
    sub rsp, 8         ; Allocate space for final_score
    mov rax, 95
    mov [rbp-16], rax   ; Initialize final_score
    ; Declaration: boolean active
    sub rsp, 8         ; Allocate space for active
    mov rax, 1
    mov [rbp-24], rax   ; Initialize active

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_number
    ; Declaration: numeric another_id
    sub rsp, 8         ; Allocate space for another_id
    mov rax, [rbp-8]
    push rax
    mov rax, 10
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-32], rax   ; Initialize another_id

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
