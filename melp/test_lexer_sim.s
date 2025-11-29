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
global _start


func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: text source
    sub rsp, 8         ; Allocate space for source
    mov rax, str_0
    mov [rbp-8], rax   ; Initialize source

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call mlp_string_length
    mov rdi, rax
    call print_number
    ; Declaration: numeric pos
    sub rsp, 8         ; Allocate space for pos
    mov rax, 0
    mov [rbp-16], rax   ; Initialize pos
    ; Declaration: numeric len
    sub rsp, 8         ; Allocate space for len
    mov rax, [rbp-8]
    mov rdi, rax
    call mlp_string_length
    mov [rbp-24], rax   ; Initialize len
    ; Declaration: numeric space1
    sub rsp, 8         ; Allocate space for space1
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, str_1
    mov rsi, rax
    call mlp_indexOf
    mov [rbp-32], rax   ; Initialize space1

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_number
    ; Declaration: text token1
    sub rsp, 8         ; Allocate space for token1
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, 0
    mov rsi, rax
    mov rax, [rbp-32]
    mov rdx, rax
    call mlp_substring
    mov [rbp-40], rax   ; Initialize token1

    ; Print statement
    mov rax, [rbp-40]
    mov rdi, rax
    call print_string
    ; Declaration: text remaining
    sub rsp, 8         ; Allocate space for remaining
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, [rbp-32]
    mov rsi, rax
    mov rax, [rbp-24]
    mov rdx, rax
    call mlp_substring
    mov [rbp-48], rax   ; Initialize remaining

    ; Print statement
    mov rax, [rbp-48]
    mov rdi, rax
    call print_string
    ; Declaration: text first_char
    sub rsp, 8         ; Allocate space for first_char
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, 0
    mov rsi, rax
    call mlp_charAt
    mov [rbp-56], rax   ; Initialize first_char

    ; Print statement
    mov rax, [rbp-56]
    mov rdi, rax
    call print_string
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

section .data
str_1: db " ", 0
str_0: db "func main() return 42 end func", 0
