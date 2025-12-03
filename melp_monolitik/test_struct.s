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
    ; Declaration: Point p (struct, size=16 bytes)
    sub rsp, 16         ; Allocate space for struct p
    mov qword [rbp-16], 0   ; Initialize p.x
    mov qword [rbp-8], 0   ; Initialize p.y

    ; Field assignment: p.x = ...
    mov rax, 10
    mov [rbp-16], rax   ; Store to p.x

    ; Field assignment: p.y = ...
    mov rax, 20
    mov [rbp-8], rax   ; Store to p.y

    ; Print statement
    mov rax, [rbp-16]   ; Load p.x
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-8]   ; Load p.y
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
