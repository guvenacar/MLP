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


func_math_ops:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param x
    sub rsp, 8
    mov [rbp-8], rax   ; Store param x locally
    mov rax, [rbp+24]   ; Load param y
    sub rsp, 8
    mov [rbp-16], rax   ; Store param y locally
    ; Declaration: numeric sum
    sub rsp, 8         ; Allocate space for sum
    ; Declaration: numeric diff
    sub rsp, 8         ; Allocate space for diff

    ; Assignment: sum = ...
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-24], rax   ; Store to sum

    ; Assignment: diff = ...
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-32], rax   ; Store to diff
    mov rax, [rbp-24]
    push rax           ; Save first return value
    mov rax, [rbp-32]
    mov rdx, rax       ; Second return in rdx
    pop rax            ; Restore first return in rax
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

    ; Declaration: numeric a
    sub rsp, 8         ; Allocate space for a
    ; Declaration: numeric b
    sub rsp, 8         ; Allocate space for b
    ; Declaration: numeric sum_result
    sub rsp, 8         ; Allocate space for sum_result
    ; Declaration: numeric diff_result
    sub rsp, 8         ; Allocate space for diff_result

    ; Assignment: a = ...
    mov rax, 100
    mov [rbp-8], rax   ; Store to a

    ; Assignment: b = ...
    mov rax, 30
    mov [rbp-16], rax   ; Store to b
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-8]
    push rax
    call func_math_ops
    add rsp, 16
    mov [rbp-24], rax   ; sum_result = first return value
    mov [rbp-32], rdx   ; diff_result = second return value

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_number
    ; Declaration: numeric result1
    sub rsp, 8         ; Allocate space for result1
    ; Declaration: numeric result2
    sub rsp, 8         ; Allocate space for result2
    mov rax, 20
    push rax
    mov rax, 50
    push rax
    call func_math_ops
    add rsp, 16
    mov [rbp-40], rax   ; result1 = first return value
    mov [rbp-48], rdx   ; result2 = second return value

    ; Print statement
    mov rax, [rbp-40]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-48]
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
