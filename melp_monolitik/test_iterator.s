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
    ; Declaration: numeric[] arr (array, size=5, total=40 bytes)
    sub rsp, 40         ; Allocate space for array arr
    mov rax, 1
    mov [rbp-40], rax   ; Initialize arr[0]
    mov rax, 2
    mov [rbp-32], rax   ; Initialize arr[1]
    mov rax, 3
    mov [rbp-24], rax   ; Initialize arr[2]
    mov rax, 4
    mov [rbp-16], rax   ; Initialize arr[3]
    mov rax, 5
    mov [rbp-8], rax   ; Initialize arr[4]

    ; For-in loop
    sub rsp, 8         ; Allocate space for x
    sub rsp, 8         ; Allocate space for loop index
    sub rsp, 8         ; Allocate space for array base
    sub rsp, 8         ; Allocate space for array length
    lea rax, [rbp-40]   ; Get address of stack array
    mov [rbp-64], rax   ; Store array base address
    mov rax, 5   ; Array length (from declaration)
    mov [rbp-72], rax   ; Store array length
    mov qword [rbp-56], 0   ; Initialize index
.L0:  ; for_in_start
    mov rax, [rbp-56]
    cmp rax, [rbp-72]
    jge .L1
    mov rdi, [rbp-64]   ; Array base
    mov rsi, [rbp-56]   ; Index
    mov rax, [rdi + rsi*8]   ; Load element
    mov [rbp-48], rax   ; Store in loop variable

    ; Print statement
    mov rax, [rbp-48]
    mov rdi, rax
    call print_number
.L2:  ; for_in_continue
    mov rax, [rbp-56]
    add rax, 1
    mov [rbp-56], rax
    jmp .L0
.L1:  ; for_in_end
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
