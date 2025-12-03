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


func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: text source_file
    sub rsp, 8         ; Allocate space for source_file
    mov rax, str_0
    mov [rbp-8], rax   ; Initialize source_file

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    call mlp_file_exists
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L0
    ; Then body

    ; Print statement
    mov rax, str_1
    mov rdi, rax
    call print_string
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L0:

    ; Print statement
    mov rax, str_2
    mov rdi, rax
    call print_string
    ; Declaration: text source
    sub rsp, 8         ; Allocate space for source
    mov rax, [rbp-8]
    mov rdi, rax
    call mlp_file_read
    mov [rbp-16], rax   ; Initialize source

    ; Print statement
    mov rax, str_3
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_string
    ; Declaration: text output_file
    sub rsp, 8         ; Allocate space for output_file
    mov rax, str_4
    mov [rbp-24], rax   ; Initialize output_file

    ; Print statement
    mov rax, str_5
    mov rdi, rax
    call print_string
    ; Declaration: numeric result
    sub rsp, 8         ; Allocate space for result
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, [rbp-16]
    mov rsi, rax
    call mlp_file_write
    mov [rbp-32], rax   ; Initialize result

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-32]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L2
    ; Then body

    ; Print statement
    mov rax, str_6
    mov rdi, rax
    call print_string

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    call mlp_file_exists
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L4
    ; Then body

    ; Print statement
    mov rax, str_7
    mov rdi, rax
    call print_string
    ; Declaration: text copy
    sub rsp, 8         ; Allocate space for copy
    mov rax, [rbp-24]
    mov rdi, rax
    call mlp_file_read
    mov [rbp-40], rax   ; Initialize copy

    ; Print statement
    mov rax, [rbp-40]
    mov rdi, rax
    call print_string
.L4:
    jmp .L3
.L2:
    ; Else body

    ; Print statement
    mov rax, str_8
    mov rdi, rax
    call print_string
.L3:
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
str_8: db "Failed to create copy!", 0
str_7: db "Verifying copy...", 0
str_6: db "Copy created successfully!", 0
str_5: db "Writing copy to test_copy.mlp...", 0
str_4: db "test_copy.mlp", 0
str_3: db "Source code loaded:", 0
str_2: db "Reading source file...", 0
str_1: db "Source file not found!", 0
str_0: db "test_file_read.mlp", 0
