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
global _start


func_swap:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param a
    sub rsp, 8
    mov [rbp-8], rax   ; Store param a locally
    mov rax, [rbp+24]   ; Load param b
    sub rsp, 8
    mov [rbp-16], rax   ; Store param b locally
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

func_main:
    push rbp
    mov rbp, rsp
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

_start:
    push rbp
    mov rbp, rsp

    call func_main

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
