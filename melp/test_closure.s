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


global func_makeAdder
func_makeAdder:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param x
    sub rsp, 8
    mov [rbp-8], rax   ; Store param x locally
    jmp __lambda_skip_0     ; Skip lambda definition

; Lambda function: __lambda_1 (captures 1 variables)
__lambda_1:
    push rbp
    mov rbp, rsp
    ; Store closure environment pointer
    push rdi    ; closure environment
    ; Captured variable: x at environment[0]
    sub rsp, 8
    mov [rbp-16], rsi
    ; Lambda body
    ; Load captured variable x from environment[0]
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
    ; Store x in environment[0]
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

    ; Declaration: numeric add5
    sub rsp, 8         ; Allocate space for add5
    mov rax, 5
    push rax
    call func_makeAdder
    add rsp, 8
    mov [rbp-8], rax   ; Initialize add5

    ; Print statement
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
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
