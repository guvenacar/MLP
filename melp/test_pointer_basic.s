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
    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, 42
    mov [rbp-8], rax   ; Initialize x
    ; Declaration: numeric* ptr (pointer)
    sub rsp, 8         ; Allocate space for pointer ptr

    ; Address-of operator
    lea rax, [rbp-8]   ; &x
    mov [rbp-16], rax   ; Initialize ptr

    ; Print statement

    ; Dereference operator
    mov rax, [rbp-16]
    mov rax, [rax]      ; Dereference pointer
    mov rdi, rax
    call print_number

    ; Pointer dereference assignment: *ptr = ...
    mov rax, 100
    push rax            ; Save value
    mov rax, [rbp-16]
    mov rbx, rax        ; Move address to rbx
    pop rax             ; Restore value
    mov [rbx], rax      ; Store value at address

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_number
    ; Declaration: numeric y
    sub rsp, 8         ; Allocate space for y
    mov rax, 7
    mov [rbp-24], rax   ; Initialize y

    ; Assignment: ptr = ...

    ; Address-of operator
    lea rax, [rbp-24]   ; &y
    mov [rbp-16], rax   ; Store to ptr

    ; Print statement

    ; Dereference operator
    mov rax, [rbp-16]
    mov rax, [rax]      ; Dereference pointer
    mov rdi, rax
    call print_number
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
