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


; Module: MathUtils

global MathUtils_add
MathUtils_add:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param a
    sub rsp, 8
    mov [rbp-8], rax   ; Store param a locally
    mov rax, [rbp+24]   ; Load param b
    sub rsp, 8
    mov [rbp-16], rax   ; Store param b locally
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    add rax, rbx
    mov rsp, rbp
    pop rbp
    ret

global MathUtils_multiply
MathUtils_multiply:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param x
    sub rsp, 8
    mov [rbp-8], rax   ; Store param x locally
    mov rax, [rbp+24]   ; Load param y
    sub rsp, 8
    mov [rbp-16], rax   ; Store param y locally
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov rsp, rbp
    pop rbp
    ret

global MathUtils_power
MathUtils_power:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param base
    sub rsp, 8
    mov [rbp-8], rax   ; Store param base locally
    mov rax, [rbp+24]   ; Load param exp
    sub rsp, 8
    mov [rbp-16], rax   ; Store param exp locally
    ; Declaration: numeric result
    sub rsp, 8         ; Allocate space for result
    mov rax, 1
    mov [rbp-24], rax   ; Initialize result
    ; Declaration: numeric i
    sub rsp, 8         ; Allocate space for i
    mov rax, 0
    mov [rbp-32], rax   ; Initialize i

    ; For loop
    sub rsp, 8         ; Allocate i
    mov rax, 0
    mov [rbp-40], rax   ; Initialize i
.L0:  ; loop_start
    mov rax, [rbp-40]   ; Load i
    push rax
    mov rax, [rbp-16]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jg .L2           ; Exit if i > end
    ; Loop body

    ; Assignment: result = ...
    mov rax, [rbp-24]
    push rax
    mov rax, [rbp-8]
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov [rbp-24], rax   ; Store to result
.L1:  ; loop_continue
    ; Increment loop variable
    mov rax, [rbp-40]
    add rax, 1
    mov [rbp-40], rax
    jmp .L0
.L2:  ; loop_end
    mov rax, [rbp-24]
    mov rsp, rbp
    pop rbp
    ret
; End of module


_start:
    ; Get argc and argv from stack
    pop rdi               ; argc (first item on stack)
    mov rsi, rsp          ; argv (pointer to argv[0])
    call mlp_get_argv     ; Convert to MLP string array

    push rbp
    mov rbp, rsp

; Import: MathUtils
    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, 10
    mov [rbp-8], rax   ; Initialize x
    ; Declaration: numeric y
    sub rsp, 8         ; Allocate space for y
    mov rax, 20
    mov [rbp-16], rax   ; Initialize y
    ; Declaration: numeric sum
    sub rsp, 8         ; Allocate space for sum
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-8]
    push rax
    call MathUtils_add
    add rsp, 16
    mov [rbp-24], rax   ; Initialize sum

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_number
    ; Declaration: numeric product
    sub rsp, 8         ; Allocate space for product
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-8]
    push rax
    call MathUtils_multiply
    add rsp, 16
    mov [rbp-32], rax   ; Initialize product

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_number
    ; Declaration: numeric diff
    sub rsp, 8         ; Allocate space for diff
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    push rax
    call MathUtils_subtract
    add rsp, 16
    mov [rbp-40], rax   ; Initialize diff

    ; Print statement
    mov rax, [rbp-40]
    mov rdi, rax
    call print_number
    ; Declaration: numeric quot
    sub rsp, 8         ; Allocate space for quot
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    push rax
    call MathUtils_divide
    add rsp, 16
    mov [rbp-48], rax   ; Initialize quot

    ; Print statement
    mov rax, [rbp-48]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
