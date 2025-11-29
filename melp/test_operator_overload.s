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

    ; Field assignment: v3.x = ...
    mov rax, [rbp-16]   ; Load v1.x
    push rax
    mov rax, [rbp-32]   ; Load v2.x
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-48], rax   ; Store to v3.x

    ; Field assignment: v3.y = ...
    mov rax, [rbp-8]   ; Load v1.y
    push rax
    mov rax, [rbp-24]   ; Load v2.y
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-40], rax   ; Store to v3.y

    ; Print statement
    mov rax, [rbp-48]   ; Load v3.x
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-40]   ; Load v3.y
    mov rdi, rax
    call print_number
    ; Declaration: Vector2D v4 (struct, size=16 bytes)
    sub rsp, 16         ; Allocate space for struct v4
    mov qword [rbp-64], 0   ; Initialize v4.x
    mov qword [rbp-56], 0   ; Initialize v4.y

    ; Field assignment: v4.x = ...
    mov rax, [rbp-16]   ; Load v1.x
    push rax
    mov rax, [rbp-32]   ; Load v2.x
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-64], rax   ; Store to v4.x

    ; Field assignment: v4.y = ...
    mov rax, [rbp-8]   ; Load v1.y
    push rax
    mov rax, [rbp-24]   ; Load v2.y
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-56], rax   ; Store to v4.y

    ; Print statement
    mov rax, [rbp-64]   ; Load v4.x
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-56]   ; Load v4.y
    mov rdi, rax
    call print_number
    ; Declaration: Vector2D v5 (struct, size=16 bytes)
    sub rsp, 16         ; Allocate space for struct v5
    mov qword [rbp-80], 0   ; Initialize v5.x
    mov qword [rbp-72], 0   ; Initialize v5.y

    ; Field assignment: v5.x = ...
    mov rax, [rbp-16]   ; Load v1.x
    push rax
    mov rax, 2
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov [rbp-80], rax   ; Store to v5.x

    ; Field assignment: v5.y = ...
    mov rax, [rbp-8]   ; Load v1.y
    push rax
    mov rax, 2
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov [rbp-72], rax   ; Store to v5.y

    ; Print statement
    mov rax, [rbp-80]   ; Load v5.x
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-72]   ; Load v5.y
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
