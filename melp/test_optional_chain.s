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
    ; Declaration: Person p (struct, size=16 bytes)
    sub rsp, 16         ; Allocate space for struct p
    mov qword [rbp-16], 0   ; Initialize p.name
    mov qword [rbp-8], 0   ; Initialize p.age

    ; Field assignment: p.name = ...
    mov rax, str_1
    mov [rbp-16], rax   ; Store to p.name

    ; Field assignment: p.age = ...
    mov rax, 30
    mov [rbp-8], rax   ; Store to p.age

    ; Print statement
    mov rax, str_2
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-16]   ; Load p.name
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-8]   ; Load p.age
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_3
    mov rdi, rax
    call print_string
    ; Declaration: text name
    sub rsp, 8         ; Allocate space for name

    ; Optional chaining (?.)
    lea rax, [rbp-16]   ; Get struct address
    mov rbx, [rax]      ; Load first field for null check
    test rbx, rbx       ; Check if null
    jz .L_null_0       ; If null, skip field access
    mov rax, [rbp-16]   ; Load p.name
    jmp .L_end_1
.L_null_0:
    mov rax, 0          ; Return null
.L_end_1:
    mov [rbp-24], rax   ; Initialize name

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_4
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
str_4: db "=== Tests complete ===", 0
str_3: db "Optional chaining on valid struct:", 0
str_2: db "Normal access:", 0
str_1: db "Alice", 0
str_0: db "=== Optional Chaining Tests ===", 0
