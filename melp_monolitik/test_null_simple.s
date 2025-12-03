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
    ; Declaration: numeric a
    sub rsp, 8         ; Allocate space for a
    mov rax, 50
    mov [rbp-8], rax   ; Initialize a
    ; Declaration: numeric result1
    sub rsp, 8         ; Allocate space for result1

    ; Null coalescing (??)
    mov rax, [rbp-8]
    test rax, rax       ; Check if null
    jz .L_default_0    ; If null, use default
    jmp .L_end_1
.L_default_0:
    mov rax, 100
.L_end_1:
    mov [rbp-16], rax   ; Initialize result1

    ; Print statement
    mov rax, str_1
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_number
    ; Declaration: numeric b
    sub rsp, 8         ; Allocate space for b
    mov rax, 0
    mov [rbp-24], rax   ; Initialize b
    ; Declaration: numeric result2
    sub rsp, 8         ; Allocate space for result2

    ; Null coalescing (??)
    mov rax, [rbp-24]
    test rax, rax       ; Check if null
    jz .L_default_2    ; If null, use default
    jmp .L_end_3
.L_default_2:
    mov rax, 200
.L_end_3:
    mov [rbp-32], rax   ; Initialize result2

    ; Print statement
    mov rax, str_2
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_number
    ; Declaration: numeric c
    sub rsp, 8         ; Allocate space for c
    mov rax, 0
    mov [rbp-40], rax   ; Initialize c
    ; Declaration: numeric d
    sub rsp, 8         ; Allocate space for d
    mov rax, 0
    mov [rbp-48], rax   ; Initialize d
    ; Declaration: numeric e
    sub rsp, 8         ; Allocate space for e
    mov rax, 30
    mov [rbp-56], rax   ; Initialize e
    ; Declaration: numeric result3
    sub rsp, 8         ; Allocate space for result3

    ; Null coalescing (??)

    ; Null coalescing (??)

    ; Null coalescing (??)
    mov rax, [rbp-40]
    test rax, rax       ; Check if null
    jz .L_default_8    ; If null, use default
    jmp .L_end_9
.L_default_8:
    mov rax, [rbp-48]
.L_end_9:
    test rax, rax       ; Check if null
    jz .L_default_6    ; If null, use default
    jmp .L_end_7
.L_default_6:
    mov rax, [rbp-56]
.L_end_7:
    test rax, rax       ; Check if null
    jz .L_default_4    ; If null, use default
    jmp .L_end_5
.L_default_4:
    mov rax, 999
.L_end_5:
    mov [rbp-64], rax   ; Initialize result3

    ; Print statement
    mov rax, str_3
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-64]
    mov rdi, rax
    call print_number
    ; Declaration: numeric f
    sub rsp, 8         ; Allocate space for f
    mov rax, 0
    mov [rbp-72], rax   ; Initialize f
    ; Declaration: numeric g
    sub rsp, 8         ; Allocate space for g
    mov rax, 42
    mov [rbp-80], rax   ; Initialize g
    ; Declaration: numeric result4
    sub rsp, 8         ; Allocate space for result4

    ; Null coalescing (??)

    ; Null coalescing (??)
    mov rax, [rbp-72]
    test rax, rax       ; Check if null
    jz .L_default_12    ; If null, use default
    jmp .L_end_13
.L_default_12:
    mov rax, [rbp-80]
.L_end_13:
    test rax, rax       ; Check if null
    jz .L_default_10    ; If null, use default
    jmp .L_end_11
.L_default_10:
    mov rax, 999
.L_end_11:
    mov [rbp-88], rax   ; Initialize result4

    ; Print statement
    mov rax, str_4
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-88]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_5
    mov rdi, rax
    call print_string
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
    call func_main

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_5: db "=== Tests complete ===", 0
str_4: db "0 ?? 42 ?? 999 = ", 0
str_3: db "0 ?? 0 ?? 30 ?? 999 = ", 0
str_2: db "0 ?? 200 = ", 0
str_1: db "50 ?? 100 = ", 0
str_0: db "=== Null Coalescing Tests ===", 0
