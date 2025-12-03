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


global func_main
func_main:
    push rbp
    mov rbp, rsp

    ; Print statement
    mov rax, str_0
    mov rdi, rax
    call print_string

    ; For-in loop
    sub rsp, 8         ; Allocate space for i
    sub rsp, 8         ; Allocate space for loop index
    sub rsp, 8         ; Allocate space for array base
    sub rsp, 8         ; Allocate space for array length
    mov rax, 5
    push rax
    pop rdi
    call mlp_range1
    mov [rbp-24], rax   ; Store array pointer
    mov rdi, rax
    call mlp_array_length
    mov [rbp-32], rax   ; Store array length
    mov qword [rbp-16], 0   ; Initialize index
.L0:  ; for_in_start
    mov rax, [rbp-16]
    cmp rax, [rbp-32]
    jge .L1
    mov rdi, [rbp-24]   ; Array base
    mov rsi, [rbp-16]   ; Index
    mov rax, [rdi + rsi*8]   ; Load element
    mov [rbp-8], rax   ; Store in loop variable

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_number
.L2:  ; for_in_continue
    mov rax, [rbp-16]
    add rax, 1
    mov [rbp-16], rax
    jmp .L0
.L1:  ; for_in_end

    ; Print statement
    mov rax, str_1
    mov rdi, rax
    call print_string

    ; For-in loop
    sub rsp, 8         ; Allocate space for i
    sub rsp, 8         ; Allocate space for loop index
    sub rsp, 8         ; Allocate space for array base
    sub rsp, 8         ; Allocate space for array length
    mov rax, 3
    push rax
    mov rax, 8
    push rax
    pop rsi
    pop rdi
    call mlp_range2
    mov [rbp-56], rax   ; Store array pointer
    mov rdi, rax
    call mlp_array_length
    mov [rbp-64], rax   ; Store array length
    mov qword [rbp-48], 0   ; Initialize index
.L3:  ; for_in_start
    mov rax, [rbp-48]
    cmp rax, [rbp-64]
    jge .L4
    mov rdi, [rbp-56]   ; Array base
    mov rsi, [rbp-48]   ; Index
    mov rax, [rdi + rsi*8]   ; Load element
    mov [rbp-40], rax   ; Store in loop variable

    ; Print statement
    mov rax, [rbp-40]
    mov rdi, rax
    call print_number
.L5:  ; for_in_continue
    mov rax, [rbp-48]
    add rax, 1
    mov [rbp-48], rax
    jmp .L3
.L4:  ; for_in_end

    ; Print statement
    mov rax, str_2
    mov rdi, rax
    call print_string

    ; For-in loop
    sub rsp, 8         ; Allocate space for i
    sub rsp, 8         ; Allocate space for loop index
    sub rsp, 8         ; Allocate space for array base
    sub rsp, 8         ; Allocate space for array length
    mov rax, 0
    push rax
    mov rax, 10
    push rax
    mov rax, 2
    push rax
    pop rdx
    pop rsi
    pop rdi
    call mlp_range
    mov [rbp-88], rax   ; Store array pointer
    mov rdi, rax
    call mlp_array_length
    mov [rbp-96], rax   ; Store array length
    mov qword [rbp-80], 0   ; Initialize index
.L6:  ; for_in_start
    mov rax, [rbp-80]
    cmp rax, [rbp-96]
    jge .L7
    mov rdi, [rbp-88]   ; Array base
    mov rsi, [rbp-80]   ; Index
    mov rax, [rdi + rsi*8]   ; Load element
    mov [rbp-72], rax   ; Store in loop variable

    ; Print statement
    mov rax, [rbp-72]
    mov rdi, rax
    call print_number
.L8:  ; for_in_continue
    mov rax, [rbp-80]
    add rax, 1
    mov [rbp-80], rax
    jmp .L6
.L7:  ; for_in_end
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

section .data
str_2: db "Even numbers using range(0, 10, 2):", 0
str_1: db "Numbers using range(3, 8):", 0
str_0: db "Numbers using range(5):", 0
