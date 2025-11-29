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


    ; Try-catch block
    ; Note: Simplified error handling (no full exception stack unwinding)
    ; Try body

    ; Print statement
    mov rax, 100
    mov rdi, rax
    call print_number
    jmp .L1           ; Skip catch if no error
.L0:  ; Catch block
    ; Catch body

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number
.L1:  ; End try-catch

    ; Try-catch block
    ; Note: Simplified error handling (no full exception stack unwinding)
    ; Try body

    ; Print statement
    mov rax, 200
    mov rdi, rax
    call print_number

    ; Throw statement (simplified)
    mov rax, 777
    mov rdi, rax
    call print_number  ; Print error code
    mov rax, 60        ; sys_exit
    mov rdi, 1         ; exit code 1 (error)
    syscall

    ; Print statement
    mov rax, 201
    mov rdi, rax
    call print_number
    jmp .L3           ; Skip catch if no error
.L2:  ; Catch block
    ; Catch body

    ; Print statement
    mov rax, 888
    mov rdi, rax
    call print_number
.L3:  ; End try-catch

    ; Print statement
    mov rax, 300
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
