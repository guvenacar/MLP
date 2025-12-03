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

    ; Declaration: Person emp (struct, size=16 bytes)
    sub rsp, 16         ; Allocate space for struct emp
    mov qword [rbp-16], 0   ; Initialize emp.age
    mov qword [rbp-8], 0   ; Initialize emp.score

    ; Field assignment: emp.age = ...
    mov rax, 30
    mov [rbp-16], rax   ; Store to emp.age

    ; Field assignment: emp.score = ...
    mov rax, 85
    mov [rbp-8], rax   ; Store to emp.score
    ; Declaration: Person stu (struct, size=16 bytes)
    sub rsp, 16         ; Allocate space for struct stu
    mov qword [rbp-32], 0   ; Initialize stu.age
    mov qword [rbp-24], 0   ; Initialize stu.score

    ; Field assignment: stu.age = ...
    mov rax, 20
    mov [rbp-32], rax   ; Store to stu.age

    ; Field assignment: stu.score = ...
    mov rax, 95
    mov [rbp-24], rax   ; Store to stu.score

    ; Print statement
    mov rax, [rbp-16]   ; Load emp.age
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-8]   ; Load emp.score
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-32]   ; Load stu.age
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-24]   ; Load stu.score
    mov rdi, rax
    call print_number
    ; Declaration: numeric uid
    sub rsp, 8         ; Allocate space for uid
    mov rax, 100
    mov [rbp-40], rax   ; Initialize uid

    ; Print statement
    mov rax, [rbp-40]
    mov rdi, rax
    call print_number
    ; Declaration: numeric items
    sub rsp, 8         ; Allocate space for items
    mov rax, 5
    mov [rbp-48], rax   ; Initialize items
    ; Declaration: numeric total
    sub rsp, 8         ; Allocate space for total
    mov rax, [rbp-48]
    push rax
    mov rax, 10
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-56], rax   ; Initialize total

    ; Print statement
    mov rax, [rbp-56]
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
