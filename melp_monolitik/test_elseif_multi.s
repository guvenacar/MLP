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


_start:
    ; Get argc and argv from stack
    pop rdi               ; argc (first item on stack)
    mov rsi, rsp          ; argv (pointer to argv[0])
    call mlp_get_argv     ; Convert to MLP string array

    push rbp
    mov rbp, rsp

    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, 15
    mov [rbp-8], rax   ; Initialize x

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, 10
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jle .L0
    ; Then body

    ; Print statement
    mov rax, 1
    mov rdi, rax
    call print_number
    jmp .L1
.L0:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, 5
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jle .L2
    ; Then body

    ; Print statement
    mov rax, 2
    mov rdi, rax
    call print_number
    jmp .L3
.L2:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, 2
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jle .L4
    ; Then body

    ; Print statement
    mov rax, 3
    mov rdi, rax
    call print_number
    jmp .L5
.L4:
    ; Else body

    ; Print statement
    mov rax, 0
    mov rdi, rax
    call print_number
.L5:
.L3:
.L1:
    ; Declaration: numeric y
    sub rsp, 8         ; Allocate space for y
    mov rax, 4
    mov [rbp-16], rax   ; Initialize y

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-16]
    push rax
    mov rax, 10
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jle .L6
    ; Then body

    ; Print statement
    mov rax, 10
    mov rdi, rax
    call print_number
    jmp .L7
.L6:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-16]
    push rax
    mov rax, 5
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jle .L8
    ; Then body

    ; Print statement
    mov rax, 20
    mov rdi, rax
    call print_number
    jmp .L9
.L8:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-16]
    push rax
    mov rax, 2
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jle .L10
    ; Then body

    ; Print statement
    mov rax, 30
    mov rdi, rax
    call print_number
    jmp .L11
.L10:
    ; Else body

    ; Print statement
    mov rax, 40
    mov rdi, rax
    call print_number
.L11:
.L9:
.L7:
    ; Declaration: numeric z
    sub rsp, 8         ; Allocate space for z
    mov rax, 1
    mov [rbp-24], rax   ; Initialize z

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    push rax
    mov rax, 10
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jle .L12
    ; Then body

    ; Print statement
    mov rax, 100
    mov rdi, rax
    call print_number
    jmp .L13
.L12:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    push rax
    mov rax, 5
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jle .L14
    ; Then body

    ; Print statement
    mov rax, 200
    mov rdi, rax
    call print_number
    jmp .L15
.L14:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    push rax
    mov rax, 2
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jle .L16
    ; Then body

    ; Print statement
    mov rax, 300
    mov rdi, rax
    call print_number
    jmp .L17
.L16:
    ; Else body

    ; Print statement
    mov rax, 400
    mov rdi, rax
    call print_number
.L17:
.L15:
.L13:

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
