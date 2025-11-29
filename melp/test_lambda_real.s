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
    ; Declaration: numeric add5
    sub rsp, 8         ; Allocate space for add5
    jmp __lambda_skip_0     ; Skip lambda definition

; Lambda function: __lambda_1 (captures 0 variables)
__lambda_1:
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov [rbp-8], rdi
    ; Lambda body
    mov rax, [rbp-8]
    push rax
    mov rax, 5
    mov rbx, rax
    pop rax
    add rax, rbx
    leave
    ret
__lambda_skip_0:
    lea rax, [rel __lambda_1]
    mov [rbp-8], rax   ; Initialize add5
    ; Declaration: numeric result
    sub rsp, 8         ; Allocate space for result
    ; Lambda/closure call with runtime detection
    mov r15, [rbp-8]   ; Load closure/lambda
    push r15
    mov rax, 10
    push rax
    mov r15, [rsp+8]
    ; Check if closure by testing magic number (0xC105UR3 = 202182115)
    mov r14, [r15]      ; Load first qword
    cmp r14, 202182115  ; Magic number for closures
    je .L_closure_2    ; Jump if closure
    ; Plain function pointer call
    pop rdi
    add rsp, 8
    call r15
    jmp .L_done_3
.L_closure_2:
    ; Closure call: [magic, func_ptr, env_ptr]
    mov r14, [r15+8]    ; Function pointer
    mov rdi, [r15+16]   ; Environment pointer (first arg)
    pop rsi
    add rsp, 8
    call r14
.L_done_3:
    mov [rbp-16], rax   ; Initialize result

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_number
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
