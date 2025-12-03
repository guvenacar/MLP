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


_start:
    ; Get argc and argv from stack
    pop rdi               ; argc (first item on stack)
    mov rsi, rsp          ; argv (pointer to argv[0])
    call mlp_get_argv     ; Convert to MLP string array

    push rbp
    mov rbp, rsp


    ; State declaration: counter
    mov rax, 100
    mov [state_counter], rax

    ; State declaration: total
    mov rax, 0
    mov [state_total], rax

    ; State declaration: multiplier
    mov rax, 5
    mov [state_multiplier], rax

    ; Print statement
    mov rax, str_0
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [state_counter]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [state_total]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [state_multiplier]
    mov rdi, rax
    call print_number

    ; Assignment: counter = ...
    mov rax, 200
    mov [state_counter], rax   ; Store to state counter

    ; Print statement
    mov rax, str_1
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [state_counter]
    mov rdi, rax
    call print_number

    ; Assignment: total = ...
    mov rax, [state_counter]
    push rax
    mov rax, [state_multiplier]
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [state_total], rax   ; Store to state total

    ; Print statement
    mov rax, str_2
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [state_total]
    mov rdi, rax
    call print_number

    ; Assignment: counter = ...
    mov rax, [state_counter]
    push rax
    mov rax, 50
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [state_counter], rax   ; Store to state counter

    ; Print statement
    mov rax, str_3
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [state_counter]
    mov rdi, rax
    call print_number

    ; Assignment: total = ...
    mov rax, [state_multiplier]
    push rax
    mov rax, 10
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov [state_total], rax   ; Store to state total

    ; Print statement
    mov rax, str_4
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [state_total]
    mov rdi, rax
    call print_number

    ; Assignment: total = ...
    mov rax, [state_total]
    push rax
    mov rax, 2
    mov rbx, rax
    pop rax
    cqo
    idiv rbx
    mov [state_total], rax   ; Store to state total

    ; Print statement
    mov rax, str_5
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [state_total]
    mov rdi, rax
    call print_number

    ; Assignment: total = ...
    mov rax, [state_counter]
    push rax
    mov rax, [state_multiplier]
    mov rbx, rax
    pop rax
    add rax, rbx
    push rax
    mov rax, 2
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov [state_total], rax   ; Store to state total

    ; Print statement
    mov rax, str_6
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [state_total]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_7
    mov rdi, rax
    call print_string

    ; If statement

    ; Evaluate comparison
    mov rax, [state_counter]
    push rax
    mov rax, 100
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jle .L0
    ; Then body

    ; Print statement
    mov rax, str_8
    mov rdi, rax
    call print_string
    jmp .L1
.L0:
    ; Else body

    ; Print statement
    mov rax, str_9
    mov rdi, rax
    call print_string
.L1:

    ; Print statement
    mov rax, str_10
    mov rdi, rax
    call print_string

    ; Assignment: counter = ...
    mov rax, 0
    mov [state_counter], rax   ; Store to state counter

    ; For-in loop
    sub rsp, 8         ; Allocate space for i
    sub rsp, 8         ; Allocate space for loop index
    sub rsp, 8         ; Allocate space for array base
    sub rsp, 8         ; Allocate space for array length
    mov rax, 1
    push rax
    mov rax, 6
    push rax
    pop rsi
    pop rdi
    call mlp_range2
    mov [rbp-24], rax   ; Store array pointer
    mov rdi, rax
    call mlp_array_length
    mov [rbp-32], rax   ; Store array length
    mov qword [rbp-16], 0   ; Initialize index
.L2:  ; for_in_start
    mov rax, [rbp-16]
    cmp rax, [rbp-32]
    jge .L3
    mov rdi, [rbp-24]   ; Array base
    mov rsi, [rbp-16]   ; Index
    mov rax, [rdi + rsi*8]   ; Load element
    mov [rbp-8], rax   ; Store in loop variable

    ; Assignment: counter = ...
    mov rax, [state_counter]
    push rax
    mov rax, [rbp-8]
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [state_counter], rax   ; Store to state counter

    ; Print statement
    mov rax, [state_counter]
    mov rdi, rax
    call print_number
.L4:  ; for_in_continue
    mov rax, [rbp-16]
    add rax, 1
    mov [rbp-16], rax
    jmp .L2
.L3:  ; for_in_end

    ; Print statement
    mov rax, str_11
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [state_counter]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [state_total]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [state_multiplier]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_12
    mov rdi, rax
    call print_string

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_12: db "=== END ===", 0
str_11: db "=== Final State Values ===", 0
str_10: db "=== Testing state in loop ===", 0
str_9: db "Counter is 100 or less", 0
str_8: db "Counter is greater than 100", 0
str_7: db "=== Testing state in condition ===", 0
str_6: db "=== total = counter + multiplier * 2 ===", 0
str_5: db "=== total = total / 2 ===", 0
str_4: db "=== total = multiplier * 10 ===", 0
str_3: db "=== counter = counter - 50 ===", 0
str_2: db "=== total = counter + multiplier ===", 0
str_1: db "=== After counter = 200 ===", 0
str_0: db "=== Initial Values ===", 0

section .bss
    ; Global state variables
state_counter: resq 1
state_total: resq 1
state_multiplier: resq 1
