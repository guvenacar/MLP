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


; Operator overload: Vector +
__op_Vector_add:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    ; Copy first parameter struct to stack
    mov rax, [rdi+0]
    mov [rbp-16], rax
    mov rax, [rdi+8]
    mov [rbp-8], rax
    ; Copy second parameter struct to stack
    mov rax, [rsi+0]
    mov [rbp-32], rax
    mov rax, [rsi+8]
    mov [rbp-24], rax
    ; Declaration: numeric result
    sub rsp, 8         ; Allocate space for result

    ; Assignment: result = ...
    mov rax, [rbp-16]   ; Load a.x
    push rax
    mov rax, [rbp-32]   ; Load b.x
    mov rbx, rax
    pop rax
    add rax, rbx
    push rax
    mov rax, [rbp-8]   ; Load a.y
    mov rbx, rax
    pop rax
    add rax, rbx
    push rax
    mov rax, [rbp-24]   ; Load b.y
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-40], rax   ; Store to result
    mov rax, [rbp-40]
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret


; Operator overload: Vector -
__op_Vector_sub:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    ; Copy first parameter struct to stack
    mov rax, [rdi+0]
    mov [rbp-16], rax
    mov rax, [rdi+8]
    mov [rbp-8], rax
    ; Copy second parameter struct to stack
    mov rax, [rsi+0]
    mov [rbp-32], rax
    mov rax, [rsi+8]
    mov [rbp-24], rax
    ; Declaration: numeric result
    sub rsp, 8         ; Allocate space for result

    ; Assignment: result = ...
    mov rax, [rbp-16]   ; Load a.x
    push rax
    mov rax, [rbp-32]   ; Load b.x
    mov rbx, rax
    pop rax
    sub rax, rbx
    push rax
    mov rax, [rbp-8]   ; Load a.y
    mov rbx, rax
    pop rax
    add rax, rbx
    push rax
    mov rax, [rbp-24]   ; Load b.y
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-40], rax   ; Store to result
    mov rax, [rbp-40]
    mov rsp, rbp
    pop rbp
    ret
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

    ; Declaration: Vector v1 (struct, size=16 bytes)
    sub rsp, 16         ; Allocate space for struct v1
    mov qword [rbp-16], 0   ; Initialize v1.x
    mov qword [rbp-8], 0   ; Initialize v1.y

    ; Field assignment: v1.x = ...
    mov rax, 10
    mov [rbp-16], rax   ; Store to v1.x

    ; Field assignment: v1.y = ...
    mov rax, 20
    mov [rbp-8], rax   ; Store to v1.y
    ; Declaration: Vector v2 (struct, size=16 bytes)
    sub rsp, 16         ; Allocate space for struct v2
    mov qword [rbp-32], 0   ; Initialize v2.x
    mov qword [rbp-24], 0   ; Initialize v2.y

    ; Field assignment: v2.x = ...
    mov rax, 5
    mov [rbp-32], rax   ; Store to v2.x

    ; Field assignment: v2.y = ...
    mov rax, 15
    mov [rbp-24], rax   ; Store to v2.y

    ; Print statement
    mov rax, str_0
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_1
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-16]   ; Load v1.x
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-8]   ; Load v1.y
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_2
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-32]   ; Load v2.x
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-24]   ; Load v2.y
    mov rdi, rax
    call print_number
    ; Declaration: numeric sum
    sub rsp, 8         ; Allocate space for sum

    ; Assignment: sum = ...
    ; Struct operator overload call
    lea rdi, [rbp-16]   ; Address of left operand
    push rdi
    lea rsi, [rbp-32]   ; Address of right operand
    pop rdi
    call __op_Vector_add
    mov [rbp-40], rax   ; Store to sum

    ; Print statement
    mov rax, str_3
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-40]
    mov rdi, rax
    call print_number
    ; Declaration: numeric diff
    sub rsp, 8         ; Allocate space for diff

    ; Assignment: diff = ...
    ; Struct operator overload call
    lea rdi, [rbp-16]   ; Address of left operand
    push rdi
    lea rsi, [rbp-32]   ; Address of right operand
    pop rdi
    call __op_Vector_sub
    mov [rbp-48], rax   ; Store to diff

    ; Print statement
    mov rax, str_4
    mov rdi, rax
    call print_string

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

section .data
str_4: db "v1 - v2 (diff of all fields) =", 0
str_3: db "v1 + v2 (sum of all fields) =", 0
str_2: db "Vector v2 (x, y):", 0
str_1: db "Vector v1 (x, y):", 0
str_0: db "=== Operator Overloading Test ===", 0
