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
extern mlp_range
extern mlp_range1
extern mlp_range2
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


_start:
    ; Get argc and argv from stack
    pop rdi               ; argc (first item on stack)
    mov rsi, rsp          ; argv (pointer to argv[0])
    call mlp_get_argv     ; Convert to MLP string array

    push rbp
    mov rbp, rsp


    ; State declaration: shared global_counter
    mov rax, 1000
    mov [state_global_counter], rax

    ; Print statement
    mov rax, str_0
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [state_global_counter]
    mov rdi, rax
    call print_number

    ; Assignment: global_counter = ...
    mov rax, [state_global_counter]
    push rax
    mov rax, 100
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [state_global_counter], rax   ; Store to state global_counter

    ; Print statement
    mov rax, [state_global_counter]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_1
    mov rdi, rax
    call print_string

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_1: db "=== END ===", 0
str_0: db "=== Shared State Test ===", 0

section .bss
    ; Global state variables
state_global_counter: resq 1
