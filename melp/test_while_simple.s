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

    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, 0
    mov [rbp-8], rax   ; Initialize x

    ; While loop
.L0:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, 3
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L1
    ; While body

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_number

    ; Assignment: x = ...
    mov rax, [rbp-8]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-8], rax   ; Store to x
    jmp .L0
.L1:  ; while_end

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
