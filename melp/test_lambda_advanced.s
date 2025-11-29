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


global func_makeCounter
func_makeCounter:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param start
    sub rsp, 8
    mov [rbp-8], rax   ; Store param start locally
    ; Declaration: numeric counter
    sub rsp, 8         ; Allocate space for counter
    jmp __lambda_skip_0     ; Skip lambda definition

; Lambda function: __lambda_1 (captures 1 variables)
__lambda_1:
    push rbp
    mov rbp, rsp
    ; Store closure environment pointer
    push rdi    ; closure environment
    ; Captured variable: start at environment[0]
    sub rsp, 8
    mov [rbp-16], rsi
    ; Lambda body
    ; Load captured variable start from environment[0]
    mov r13, [rbp-8]   ; Environment pointer
    mov rax, [r13+0]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    add rax, rbx
    leave
    ret
__lambda_skip_0:

    ; Allocate closure environment
    mov rdi, 8
    call malloc
    mov r12, rax    ; Save environment pointer
    ; Store start in environment[0]
    mov rax, [rbp-8]
    mov [r12+0], rax
    ; Create closure structure [magic, func_ptr, env_ptr]
    mov rdi, 24     ; 3 pointers (magic + func_ptr + env_ptr)
    call malloc
    mov qword [rax], 202182115   ; Magic number 0xC105UR3
    lea r13, [rel __lambda_1]
    mov [rax+8], r13     ; Store function pointer
    mov [rax+16], r12    ; Store environment pointer
    ; rax now holds closure pointer
    mov [rbp-16], rax   ; Initialize counter
    mov rax, [rbp-16]
    mov rsp, rbp
    pop rbp
    ret

global func_main
func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: numeric add
    sub rsp, 8         ; Allocate space for add
    jmp __lambda_skip_2     ; Skip lambda definition

; Lambda function: __lambda_3 (captures 0 variables)
__lambda_3:
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov [rbp-8], rdi
    ; Lambda body
    mov rax, [rbp-8]
    push rax
    mov rax, 10
    mov rbx, rax
    pop rax
    add rax, rbx
    leave
    ret
__lambda_skip_2:
    lea rax, [rel __lambda_3]
    mov [rbp-8], rax   ; Initialize add
    ; Declaration: numeric result1
    sub rsp, 8         ; Allocate space for result1
    ; Lambda/closure call with runtime detection
    mov r15, [rbp-8]   ; Load closure/lambda
    push r15
    mov rax, 5
    push rax
    mov r15, [rsp+8]
    ; Check if closure by testing magic number (0xC105UR3 = 202182115)
    mov r14, [r15]      ; Load first qword
    cmp r14, 202182115  ; Magic number for closures
    je .L_closure_4    ; Jump if closure
    ; Plain function pointer call
    pop rdi
    add rsp, 8
    call r15
    jmp .L_done_5
.L_closure_4:
    ; Closure call: [magic, func_ptr, env_ptr]
    mov r14, [r15+8]    ; Function pointer
    mov rdi, [r15+16]   ; Environment pointer (first arg)
    pop rsi
    add rsp, 8
    call r14
.L_done_5:
    mov [rbp-16], rax   ; Initialize result1

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_number
    ; Declaration: numeric multiplier
    sub rsp, 8         ; Allocate space for multiplier
    mov rax, 3
    mov [rbp-24], rax   ; Initialize multiplier
    ; Declaration: numeric multiply
    sub rsp, 8         ; Allocate space for multiply
    jmp __lambda_skip_6     ; Skip lambda definition

; Lambda function: __lambda_7 (captures 1 variables)
__lambda_7:
    push rbp
    mov rbp, rsp
    ; Store closure environment pointer
    push rdi    ; closure environment
    ; Captured variable: multiplier at environment[0]
    sub rsp, 8
    mov [rbp-16], rsi
    ; Lambda body
    mov rax, [rbp-16]
    push rax
    ; Load captured variable multiplier from environment[0]
    mov r13, [rbp-8]   ; Environment pointer
    mov rax, [r13+0]
    mov rbx, rax
    pop rax
    imul rax, rbx
    leave
    ret
__lambda_skip_6:

    ; Allocate closure environment
    mov rdi, 8
    call malloc
    mov r12, rax    ; Save environment pointer
    ; Store multiplier in environment[0]
    mov rax, [rbp-24]
    mov [r12+0], rax
    ; Create closure structure [magic, func_ptr, env_ptr]
    mov rdi, 24     ; 3 pointers (magic + func_ptr + env_ptr)
    call malloc
    mov qword [rax], 202182115   ; Magic number 0xC105UR3
    lea r13, [rel __lambda_7]
    mov [rax+8], r13     ; Store function pointer
    mov [rax+16], r12    ; Store environment pointer
    ; rax now holds closure pointer
    mov [rbp-32], rax   ; Initialize multiply
    ; Declaration: numeric result2
    sub rsp, 8         ; Allocate space for result2
    ; Lambda/closure call with runtime detection
    mov r15, [rbp-32]   ; Load closure/lambda
    push r15
    mov rax, 4
    push rax
    mov r15, [rsp+8]
    ; Check if closure by testing magic number (0xC105UR3 = 202182115)
    mov r14, [r15]      ; Load first qword
    cmp r14, 202182115  ; Magic number for closures
    je .L_closure_8    ; Jump if closure
    ; Plain function pointer call
    pop rdi
    add rsp, 8
    call r15
    jmp .L_done_9
.L_closure_8:
    ; Closure call: [magic, func_ptr, env_ptr]
    mov r14, [r15+8]    ; Function pointer
    mov rdi, [r15+16]   ; Environment pointer (first arg)
    pop rsi
    add rsp, 8
    call r14
.L_done_9:
    mov [rbp-40], rax   ; Initialize result2

    ; Print statement
    mov rax, [rbp-40]
    mov rdi, rax
    call print_number
    ; Declaration: numeric counter5
    sub rsp, 8         ; Allocate space for counter5
    mov rax, 5
    push rax
    call func_makeCounter
    add rsp, 8
    mov [rbp-48], rax   ; Initialize counter5
    ; Declaration: numeric result3
    sub rsp, 8         ; Allocate space for result3
    ; Lambda/closure call with runtime detection
    mov r15, [rbp-48]   ; Load closure/lambda
    push r15
    mov rax, 2
    push rax
    mov r15, [rsp+8]
    ; Check if closure by testing magic number (0xC105UR3 = 202182115)
    mov r14, [r15]      ; Load first qword
    cmp r14, 202182115  ; Magic number for closures
    je .L_closure_10    ; Jump if closure
    ; Plain function pointer call
    pop rdi
    add rsp, 8
    call r15
    jmp .L_done_11
.L_closure_10:
    ; Closure call: [magic, func_ptr, env_ptr]
    mov r14, [r15+8]    ; Function pointer
    mov rdi, [r15+16]   ; Environment pointer (first arg)
    pop rsi
    add rsp, 8
    call r14
.L_done_11:
    mov [rbp-56], rax   ; Initialize result3

    ; Print statement
    mov rax, [rbp-56]
    mov rdi, rax
    call print_number
    ; Declaration: numeric counter10
    sub rsp, 8         ; Allocate space for counter10
    mov rax, 10
    push rax
    call func_makeCounter
    add rsp, 8
    mov [rbp-64], rax   ; Initialize counter10
    ; Declaration: numeric result4
    sub rsp, 8         ; Allocate space for result4
    ; Lambda/closure call with runtime detection
    mov r15, [rbp-64]   ; Load closure/lambda
    push r15
    mov rax, 3
    push rax
    mov r15, [rsp+8]
    ; Check if closure by testing magic number (0xC105UR3 = 202182115)
    mov r14, [r15]      ; Load first qword
    cmp r14, 202182115  ; Magic number for closures
    je .L_closure_12    ; Jump if closure
    ; Plain function pointer call
    pop rdi
    add rsp, 8
    call r15
    jmp .L_done_13
.L_closure_12:
    ; Closure call: [magic, func_ptr, env_ptr]
    mov r14, [r15+8]    ; Function pointer
    mov rdi, [r15+16]   ; Environment pointer (first arg)
    pop rsi
    add rsp, 8
    call r14
.L_done_13:
    mov [rbp-72], rax   ; Initialize result4

    ; Print statement
    mov rax, [rbp-72]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, 999
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
