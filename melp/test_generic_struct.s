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
    ; Declaration: Box b (struct, size=8 bytes)
    sub rsp, 8         ; Allocate space for struct b
    mov qword [rbp-8], 0   ; Initialize b.value

    ; Field assignment: b.value = ...
    mov rax, 42
    mov [rbp-8], rax   ; Store to b.value

    ; Print statement
    mov rax, [rbp-8]   ; Load b.value
    mov rdi, rax
    call print_number
    ; Declaration: Pair p (struct, size=16 bytes)
    sub rsp, 16         ; Allocate space for struct p
    mov qword [rbp-24], 0   ; Initialize p.first
    mov qword [rbp-16], 0   ; Initialize p.second

    ; Field assignment: p.first = ...
    mov rax, 100
    mov [rbp-24], rax   ; Store to p.first

    ; Field assignment: p.second = ...
    mov rax, 200
    mov [rbp-16], rax   ; Store to p.second

    ; Print statement
    mov rax, [rbp-24]   ; Load p.first
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-16]   ; Load p.second
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
