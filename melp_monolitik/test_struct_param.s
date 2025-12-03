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


global func_addVectors
func_addVectors:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param a
    sub rsp, 8
    mov [rbp-8], rax   ; Store param a locally
    mov rax, [rbp+24]   ; Load param b
    sub rsp, 8
    mov [rbp-16], rax   ; Store param b locally
    ; Declaration: Vector2D result (struct, size=16 bytes)
    sub rsp, 16         ; Allocate space for struct result
    mov qword [rbp-32], 0   ; Initialize result.x
    mov qword [rbp-24], 0   ; Initialize result.y

    ; Field assignment: result.x = ...
    mov rax, [rbp-8]   ; Load a.x
    push rax
    mov rax, [rbp-16]   ; Load b.x
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-32], rax   ; Store to result.x

    ; Field assignment: result.y = ...
    mov rax, [rbp-0]   ; Load a.y
    push rax
    mov rax, [rbp-8]   ; Load b.y
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-24], rax   ; Store to result.y
    mov rax, [rbp-32]
    mov rsp, rbp
    pop rbp
    ret

global func_main
func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: Vector2D v1 (struct, size=16 bytes)
    sub rsp, 16         ; Allocate space for struct v1
    mov qword [rbp-16], 0   ; Initialize v1.x
    mov qword [rbp-8], 0   ; Initialize v1.y

    ; Field assignment: v1.x = ...
    mov rax, 10
    mov [rbp-16], rax   ; Store to v1.x

    ; Field assignment: v1.y = ...
    mov rax, 20
    mov [rbp-8], rax   ; Store to v1.y
    ; Declaration: Vector2D v2 (struct, size=16 bytes)
    sub rsp, 16         ; Allocate space for struct v2
    mov qword [rbp-32], 0   ; Initialize v2.x
    mov qword [rbp-24], 0   ; Initialize v2.y

    ; Field assignment: v2.x = ...
    mov rax, 5
    mov [rbp-32], rax   ; Store to v2.x

    ; Field assignment: v2.y = ...
    mov rax, 15
    mov [rbp-24], rax   ; Store to v2.y
    ; Declaration: Vector2D v3 (struct, size=16 bytes)
    sub rsp, 16         ; Allocate space for struct v3
    mov qword [rbp-48], 0   ; Initialize v3.x
    mov qword [rbp-40], 0   ; Initialize v3.y

    ; Assignment: v3 = ...
    mov rax, [rbp-32]
    push rax
    mov rax, [rbp-16]
    push rax
    call func_addVectors
    add rsp, 16
    mov [rbp-48], rax   ; Store to v3

    ; Print statement
    mov rax, [rbp-48]   ; Load v3.x
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-40]   ; Load v3.y
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, 999
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
