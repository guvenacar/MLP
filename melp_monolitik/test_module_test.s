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
extern malloc
extern free
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
extern setjmp
extern strcmp
extern mlp_exception_push
extern mlp_exception_pop
extern mlp_throw
extern mlp_exception_type
extern mlp_exception_message
extern mlp_exception_code
extern mlp_exception_has_handler
extern mlp_exception_has_parent_handler
global _start


; Module: MathUtils

global MathUtils_add
MathUtils_add:
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

global MathUtils_multiply
MathUtils_multiply:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param x
    sub rsp, 8
    mov [rbp-8], rax   ; Store param x locally
    mov rax, [rbp+24]   ; Load param y
    sub rsp, 8
    mov [rbp-16], rax   ; Store param y locally
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov rsp, rbp
    pop rbp
    ret
; End of module

; Skipping duplicate module: MathUtils

_start:
    ; Get argc and argv from stack
    pop rdi               ; argc (first item on stack)
    mov rsi, rsp          ; argv (pointer to argv[0])
    call mlp_get_argv     ; Convert to MLP string array

    push rbp
    mov rbp, rsp

    ; Declaration: numeric result1
    sub rsp, 8         ; Allocate space for result1
    mov rax, 5
    push rax
    mov rax, 10
    push rax
    call MathUtils_add
    add rsp, 16
    mov [rbp-8], rax   ; Initialize result1

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_number
    ; Declaration: numeric result2
    sub rsp, 8         ; Allocate space for result2
    mov rax, 7
    push rax
    mov rax, 3
    push rax
    call MathUtils_multiply
    add rsp, 16
    mov [rbp-16], rax   ; Initialize result2

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
