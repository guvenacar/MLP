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
    mov rax, 2
    mov [rbp-8], rax   ; Initialize x

    ; Switch statement
    mov rax, [rbp-8]
    push rax         ; Save switch value
    ; Case comparison
    pop rax          ; Restore switch value
    push rax         ; Save again for next comparison
    mov rax, 1
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    je .L1          ; Jump to case 0
    ; Case comparison
    pop rax          ; Restore switch value
    push rax         ; Save again for next comparison
    mov rax, 2
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    je .L2          ; Jump to case 1
    ; Case comparison
    pop rax          ; Restore switch value
    push rax         ; Save again for next comparison
    mov rax, 3
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    je .L3          ; Jump to case 2
    pop rax          ; Clean up switch value
    jmp .L4         ; Jump to default/end
.L1:  ; case 0

    ; Print statement
    mov rax, 10
    mov rdi, rax
    call print_number
    jmp .L0         ; Fall through to end
.L2:  ; case 1

    ; Print statement
    mov rax, 20
    mov rdi, rax
    call print_number
    jmp .L0         ; Fall through to end
.L3:  ; case 2

    ; Print statement
    mov rax, 30
    mov rdi, rax
    call print_number
    jmp .L0         ; Fall through to end
.L4:  ; default

    ; Print statement
    mov rax, 0
    mov rdi, rax
    call print_number
.L0:  ; switch_end
    ; Declaration: numeric y
    sub rsp, 8         ; Allocate space for y
    mov rax, 5
    mov [rbp-16], rax   ; Initialize y

    ; Switch statement
    mov rax, [rbp-16]
    push rax         ; Save switch value
    ; Case comparison
    pop rax          ; Restore switch value
    push rax         ; Save again for next comparison
    mov rax, 1
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    je .L6          ; Jump to case 0
    ; Case comparison
    pop rax          ; Restore switch value
    push rax         ; Save again for next comparison
    mov rax, 2
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    je .L7          ; Jump to case 1
    pop rax          ; Clean up switch value
    jmp .L8         ; Jump to default/end
.L6:  ; case 0

    ; Print statement
    mov rax, 100
    mov rdi, rax
    call print_number
    jmp .L5         ; Fall through to end
.L7:  ; case 1

    ; Print statement
    mov rax, 200
    mov rdi, rax
    call print_number
    jmp .L5         ; Fall through to end
.L8:  ; default

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number
.L5:  ; switch_end

    ; Print statement
    mov rax, 777
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
