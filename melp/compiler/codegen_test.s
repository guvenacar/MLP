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
global _start


func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: text var_name
    sub rsp, 8         ; Allocate space for var_name
    mov rax, str_0
    mov [rbp-8], rax   ; Initialize var_name
    ; Declaration: text var_value
    sub rsp, 8         ; Allocate space for var_value
    mov rax, str_1
    mov [rbp-16], rax   ; Initialize var_value

    ; Print statement
    mov rax, str_2
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_3
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_4
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_5
    mov rdi, rax
    call print_string
    ; Declaration: text output_file
    sub rsp, 8         ; Allocate space for output_file
    mov rax, str_6
    mov [rbp-24], rax   ; Initialize output_file
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_7
    mov rsi, rax
    call mlp_file_write
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_8
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_9
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_10
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_11
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_12
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_13
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_14
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_15
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_16
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_17
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_18
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_19
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_20
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_21
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_22
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_23
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_24
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_25
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_26
    mov rsi, rax
    call mlp_file_append
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, str_27
    mov rsi, rax
    call mlp_file_append

    ; Print statement
    mov rax, str_28
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_29
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_30
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_31
    mov rdi, rax
    call print_string
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
str_31: db " ", 0
str_30: db "=== CODEGEN SUCCESS ===", 0
str_29: db " ", 0
str_28: db "✓ Assembly written to: ", 0
str_27: db "    syscall
", 0
str_26: db "    xor rdi, rdi
", 0
str_25: db "    mov rax, 60
", 0
str_24: db "    pop rbp
", 0
str_23: db "    mov rsp, rbp
", 0
str_22: db "    ; Exit
", 0
str_21: db "
", 0
str_20: db "    mov [rbp-8], rax
", 0
str_19: db "    mov rax, 42
", 0
str_18: db "    ; Declaration: numeric x = 42
", 0
str_17: db "
", 0
str_16: db "    sub rsp, 16
", 0
str_15: db "    mov rbp, rsp
", 0
str_14: db "    push rbp
", 0
str_13: db "_start:
", 0
str_12: db "
", 0
str_11: db "    extern mlp_print_num
", 0
str_10: db "    global _start
", 0
str_9: db "section .text
", 0
str_8: db "
", 0
str_7: db "section .data
", 0
str_6: db "compiler/generated.s", 0
str_5: db " ", 0
str_4: db " = ", 0
str_3: db "Generating assembly for: numeric ", 0
str_2: db "=== CODEGEN START ===", 0
str_1: db "42", 0
str_0: db "x", 0
