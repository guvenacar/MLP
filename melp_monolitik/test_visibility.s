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

    ; module Utils (functions)

global func_public_add
func_public_add:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param a
    sub rsp, 8
    mov [rbp-8], rax   ; Store param a locally
    mov rax, [rbp+24]   ; Load param b
    sub rsp, 8
    mov [rbp-16], rax   ; Store param b locally
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    add rax, rbx
    mov rsp, rbp
    pop rbp
    ret

func_internal_helper:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param x
    sub rsp, 8
    mov [rbp-8], rax   ; Store param x locally
    mov rax, [rbp-8]
    push rax
    mov rax, 2
    mov rbx, rax
    pop rax
    imul rax, rbx
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

    ; module Utils (non-functions)
    ; Declaration: numeric public_var
    sub rsp, 8         ; Allocate space for public_var
    mov rax, 100
    mov [rbp-8], rax   ; Initialize public_var
    ; Declaration: numeric internal_var
    sub rsp, 8         ; Allocate space for internal_var
    mov rax, 42
    mov [rbp-16], rax   ; Initialize internal_var
    ; Declaration: numeric result
    sub rsp, 8         ; Allocate space for result
    mov rax, 50
    mov [rbp-24], rax   ; Initialize result

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
