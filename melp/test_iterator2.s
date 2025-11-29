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
    ; Declaration: numeric[] stack_arr (array, size=3, total=24 bytes)
    sub rsp, 24         ; Allocate space for array stack_arr
    mov rax, 10
    mov [rbp-24], rax   ; Initialize stack_arr[0]
    mov rax, 20
    mov [rbp-16], rax   ; Initialize stack_arr[1]
    mov rax, 30
    mov [rbp-8], rax   ; Initialize stack_arr[2]

    ; Print statement
    mov rax, str_0
    mov rdi, rax
    call print_string

    ; For-in loop
    sub rsp, 8         ; Allocate space for val
    sub rsp, 8         ; Allocate space for loop index
    sub rsp, 8         ; Allocate space for array base
    sub rsp, 8         ; Allocate space for array length
    lea rax, [rbp-24]   ; Get address of stack array
    mov [rbp-48], rax   ; Store array base address
    mov rax, 3   ; Array length (from declaration)
    mov [rbp-56], rax   ; Store array length
    mov qword [rbp-40], 0   ; Initialize index
.L0:  ; for_in_start
    mov rax, [rbp-40]
    cmp rax, [rbp-56]
    jge .L1
    mov rdi, [rbp-48]   ; Array base
    mov rsi, [rbp-40]   ; Index
    mov rax, [rdi + rsi*8]   ; Load element
    mov [rbp-32], rax   ; Store in loop variable

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_number
.L2:  ; for_in_continue
    mov rax, [rbp-40]
    add rax, 1
    mov [rbp-40], rax
    jmp .L0
.L1:  ; for_in_end
    ; Declaration: numeric total
    sub rsp, 8         ; Allocate space for total
    mov rax, 0
    mov [rbp-64], rax   ; Initialize total

    ; For-in loop
    sub rsp, 8         ; Allocate space for n
    sub rsp, 8         ; Allocate space for loop index
    sub rsp, 8         ; Allocate space for array base
    sub rsp, 8         ; Allocate space for array length
    lea rax, [rbp-24]   ; Get address of stack array
    mov [rbp-88], rax   ; Store array base address
    mov rax, 3   ; Array length (from declaration)
    mov [rbp-96], rax   ; Store array length
    mov qword [rbp-80], 0   ; Initialize index
.L3:  ; for_in_start
    mov rax, [rbp-80]
    cmp rax, [rbp-96]
    jge .L4
    mov rdi, [rbp-88]   ; Array base
    mov rsi, [rbp-80]   ; Index
    mov rax, [rdi + rsi*8]   ; Load element
    mov [rbp-72], rax   ; Store in loop variable

    ; Assignment: total = ...
    mov rax, [rbp-64]
    push rax
    mov rax, [rbp-72]
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-64], rax   ; Store to total
.L5:  ; for_in_continue
    mov rax, [rbp-80]
    add rax, 1
    mov [rbp-80], rax
    jmp .L3
.L4:  ; for_in_end

    ; Print statement
    mov rax, str_1
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-64]
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

section .data
str_1: db "Sum:", 0
str_0: db "Stack array iteration:", 0
