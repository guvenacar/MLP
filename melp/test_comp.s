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

    ; Declaration: numeric i
    sub rsp, 8         ; Allocate space for i
    mov rax, 0
    mov [rbp-8], rax   ; Initialize i

    ; Do-while loop
.L0:  ; do_while_start
    ; Do-while body

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_number

    ; Assignment: i = ...
    mov rax, [rbp-8]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-8], rax   ; Store to i
.L1:  ; do_while_continue
    ; Check condition

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, 3
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L2
    jmp .L0         ; Condition true, loop
.L2:  ; do_while_end

    ; Print statement
    mov rax, 111
    mov rdi, rax
    call print_number
    ; Declaration: numeric j
    sub rsp, 8         ; Allocate space for j
    mov rax, 10
    mov [rbp-16], rax   ; Initialize j

    ; Do-while loop
.L3:  ; do_while_start
    ; Do-while body

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_number

    ; Assignment: j = ...
    mov rax, [rbp-16]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-16], rax   ; Store to j
.L4:  ; do_while_continue
    ; Check condition

    ; Evaluate comparison
    mov rax, [rbp-16]
    push rax
    mov rax, 5
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L5
    jmp .L3         ; Condition true, loop
.L5:  ; do_while_end

    ; Print statement
    mov rax, 222
    mov rdi, rax
    call print_number
    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, 0
    mov [rbp-24], rax   ; Initialize x

    ; Do-while loop
.L6:  ; do_while_start
    ; Do-while body
    ; Declaration: numeric y
    sub rsp, 8         ; Allocate space for y
    mov rax, 0
    mov [rbp-32], rax   ; Initialize y

    ; Do-while loop
.L9:  ; do_while_start
    ; Do-while body

    ; Print statement
    mov rax, [rbp-24]
    push rax
    mov rax, 10
    mov rbx, rax
    pop rax
    imul rax, rbx
    push rax
    mov rax, [rbp-32]
    mov rbx, rax
    pop rax
    add rax, rbx
    mov rdi, rax
    call print_number

    ; Assignment: y = ...
    mov rax, [rbp-32]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-32], rax   ; Store to y
.L10:  ; do_while_continue
    ; Check condition

    ; Evaluate comparison
    mov rax, [rbp-32]
    push rax
    mov rax, 2
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L11
    jmp .L9         ; Condition true, loop
.L11:  ; do_while_end

    ; Assignment: x = ...
    mov rax, [rbp-24]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-24], rax   ; Store to x
.L7:  ; do_while_continue
    ; Check condition

    ; Evaluate comparison
    mov rax, [rbp-24]
    push rax
    mov rax, 2
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L8
    jmp .L6         ; Condition true, loop
.L8:  ; do_while_end

    ; Print statement
    mov rax, 333
    mov rdi, rax
    call print_number
    ; Declaration: numeric sum
    sub rsp, 8         ; Allocate space for sum
    mov rax, 0
    mov [rbp-40], rax   ; Initialize sum
    ; Declaration: numeric n
    sub rsp, 8         ; Allocate space for n
    mov rax, 1
    mov [rbp-48], rax   ; Initialize n

    ; Do-while loop
.L12:  ; do_while_start
    ; Do-while body

    ; Assignment: sum = ...
    mov rax, [rbp-40]
    push rax
    mov rax, [rbp-48]
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-40], rax   ; Store to sum

    ; Assignment: n = ...
    mov rax, [rbp-48]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-48], rax   ; Store to n
.L13:  ; do_while_continue
    ; Check condition

    ; Evaluate comparison
    mov rax, [rbp-48]
    push rax
    mov rax, 5
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jg .L14
    jmp .L12         ; Condition true, loop
.L14:  ; do_while_end

    ; Print statement
    mov rax, [rbp-40]
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
