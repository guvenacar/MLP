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


global func_make_squares
func_make_squares:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param n
    sub rsp, 8
    mov [rbp-8], rax   ; Store param n locally
    ; Generator function setup
    sub rsp, 8
    sub rsp, 8
    mov rdi, 16
    call mlp_array_alloc
    mov [rbp-16], rax      ; Store result array pointer
    mov qword [rbp-24], 0 ; Initialize index to 0

    ; For loop
    sub rsp, 8         ; Allocate i
    mov rax, 0
    mov [rbp-32], rax   ; Initialize i
.L0:  ; loop_start
    mov rax, [rbp-32]   ; Load i
    push rax
    mov rax, [rbp-8]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jg .L2           ; Exit if i > end
    ; Loop body
    ; Yield statement
    mov rax, [rbp-32]
    push rax
    mov rax, [rbp-32]
    mov rbx, rax
    pop rax
    imul rax, rbx
    push rax           ; Save yield value
    mov rdi, [rbp-16]   ; Load result array pointer
    mov rsi, [rbp-24]   ; Load current index
    pop rax             ; Restore yield value
    mov [rdi + rsi*8], rax   ; Store yielded value
    inc qword [rbp-24]   ; Increment index
.L1:  ; loop_continue
    ; Increment loop variable
    mov rax, [rbp-32]
    add rax, 1
    mov [rbp-32], rax
    jmp .L0
.L2:  ; loop_end
    ; Generator: resize and return result array
    mov rdi, [rbp-16]     ; Array pointer
    mov rsi, [rbp-24]    ; Actual size used
    call mlp_array_resize  ; Resize to actual size
    mov rsp, rbp
    pop rbp
    ret

global func_main
func_main:
    push rbp
    mov rbp, rsp

    ; Print statement
    mov rax, str_0
    mov rdi, rax
    call print_string

    ; For-in loop
    sub rsp, 8         ; Allocate space for sq
    sub rsp, 8         ; Allocate space for loop index
    sub rsp, 8         ; Allocate space for array base
    sub rsp, 8         ; Allocate space for array length
    mov rax, 5
    push rax
    call func_make_squares
    add rsp, 8
    mov [rbp-24], rax   ; Store array pointer
    mov rdi, rax
    call mlp_array_length
    mov [rbp-32], rax   ; Store array length
    mov qword [rbp-16], 0   ; Initialize index
.L3:  ; for_in_start
    mov rax, [rbp-16]
    cmp rax, [rbp-32]
    jge .L4
    mov rdi, [rbp-24]   ; Array base
    mov rsi, [rbp-16]   ; Index
    mov rax, [rdi + rsi*8]   ; Load element
    mov [rbp-8], rax   ; Store in loop variable

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_number
.L5:  ; for_in_continue
    mov rax, [rbp-16]
    add rax, 1
    mov [rbp-16], rax
    jmp .L3
.L4:  ; for_in_end
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
str_0: db "Squares of 0 to 4:", 0
