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
    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, 5
    mov [rbp-8], rax   ; Initialize x

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, 10
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jle .L0
    ; Then body

    ; Print statement
    mov rax, 1
    mov rdi, rax
    call print_number
    jmp .L1
.L0:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, 5
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jle .L2
    ; Then body

    ; Print statement
    mov rax, 2
    mov rdi, rax
    call print_number
    jmp .L3
.L2:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jle .L4
    ; Then body

    ; Print statement
    mov rax, 3
    mov rdi, rax
    call print_number
    jmp .L5
.L4:
    ; Else body

    ; Print statement
    mov rax, 0
    mov rdi, rax
    call print_number
.L5:
.L3:
.L1:
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
