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

    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, 3
    mov [rbp-8], rax   ; Initialize x

    ; Switch statement
    mov rax, [rbp-8]
    push rax         ; Save switch value
    ; Case comparison
    pop rax          ; Restore switch value
    push rax         ; Save again for next comparison
    ; Range match
    mov rax, 1
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    jl .L4          ; Skip if value < start
    mov rax, 5
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    jle .L1         ; Range matched, execute case
.L4:  ; Skip label
    ; Case comparison
    pop rax          ; Restore switch value
    push rax         ; Save again for next comparison
    ; Range match
    mov rax, 6
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    jl .L5          ; Skip if value < start
    mov rax, 10
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    jle .L2         ; Range matched, execute case
.L5:  ; Skip label
    pop rax          ; Clean up switch value
    jmp .L3         ; Jump to default/end
.L1:  ; case 0

    ; Print statement
    mov rax, 100
    mov rdi, rax
    call print_number
    jmp .L0         ; Fall through to end
.L2:  ; case 1

    ; Print statement
    mov rax, 200
    mov rdi, rax
    call print_number
    jmp .L0         ; Fall through to end
.L3:  ; default

    ; Print statement
    mov rax, 0
    mov rdi, rax
    call print_number
.L0:  ; switch_end
    ; Declaration: numeric y
    sub rsp, 8         ; Allocate space for y
    mov rax, 15
    mov [rbp-16], rax   ; Initialize y

    ; Switch statement
    mov rax, [rbp-16]
    push rax         ; Save switch value
    ; Case comparison
    pop rax          ; Restore switch value
    push rax         ; Save again for next comparison
    mov rax, 10
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    je .L10          ; Value matched, check guard
.L10:  ; Guard check
    ; Evaluate guard condition
    mov rax, [rbp-16]
    push rax
    mov rax, 12
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0
    mov rbx, 1
    cmovg rax, rbx
    cmp rax, 0
    jne .L7         ; Guard passed, execute case
    ; Case comparison
    pop rax          ; Restore switch value
    push rax         ; Save again for next comparison
    mov rax, 20
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    je .L8          ; Value matched, execute case
    pop rax          ; Clean up switch value
    jmp .L9         ; Jump to default/end
.L7:  ; case 0

    ; Print statement
    mov rax, 300
    mov rdi, rax
    call print_number
    jmp .L6         ; Fall through to end
.L8:  ; case 1

    ; Print statement
    mov rax, 400
    mov rdi, rax
    call print_number
    jmp .L6         ; Fall through to end
.L9:  ; default

    ; Print statement
    mov rax, 0
    mov rdi, rax
    call print_number
.L6:  ; switch_end
    ; Declaration: numeric z
    sub rsp, 8         ; Allocate space for z
    mov rax, 0
    mov [rbp-24], rax   ; Initialize z

    ; Switch statement
    mov rax, [rbp-24]
    push rax         ; Save switch value
    ; Case comparison
    pop rax          ; Restore switch value
    push rax         ; Save again for next comparison
    ; Type check (null)
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, 0
    je .L13          ; Null matched, execute case
    ; Case comparison
    pop rax          ; Restore switch value
    push rax         ; Save again for next comparison
    mov rax, 1
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    je .L14          ; Value matched, execute case
    pop rax          ; Clean up switch value
    jmp .L15         ; Jump to default/end
.L13:  ; case 0

    ; Print statement
    mov rax, 500
    mov rdi, rax
    call print_number
    jmp .L12         ; Fall through to end
.L14:  ; case 1

    ; Print statement
    mov rax, 600
    mov rdi, rax
    call print_number
    jmp .L12         ; Fall through to end
.L15:  ; default

    ; Print statement
    mov rax, 0
    mov rdi, rax
    call print_number
.L12:  ; switch_end
    ; Declaration: numeric w
    sub rsp, 8         ; Allocate space for w
    mov rax, 7
    mov [rbp-32], rax   ; Initialize w

    ; Switch statement
    mov rax, [rbp-32]
    push rax         ; Save switch value
    ; Case comparison
    pop rax          ; Restore switch value
    push rax         ; Save again for next comparison
    ; Range match
    mov rax, 1
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    jl .L21          ; Skip if value < start
    mov rax, 5
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    jle .L17         ; Range matched, execute case
.L21:  ; Skip label
    ; Case comparison
    pop rax          ; Restore switch value
    push rax         ; Save again for next comparison
    ; Range match
    mov rax, 6
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    jl .L23          ; Skip if value < start
    mov rax, 10
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    jle .L22         ; Range matched, check guard
.L23:  ; Skip label
.L22:  ; Guard check
    ; Evaluate guard condition
    mov rax, [rbp-32]
    push rax
    mov rax, 8
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0
    mov rbx, 1
    cmovg rax, rbx
    cmp rax, 0
    jne .L18         ; Guard passed, execute case
    ; Case comparison
    pop rax          ; Restore switch value
    push rax         ; Save again for next comparison
    ; Range match
    mov rax, 6
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    jl .L26          ; Skip if value < start
    mov rax, 10
    mov rbx, rax
    pop rax          ; Get switch value
    push rax         ; Save again
    cmp rax, rbx
    jle .L25         ; Range matched, check guard
.L26:  ; Skip label
.L25:  ; Guard check
    ; Evaluate guard condition
    mov rax, [rbp-32]
    push rax
    mov rax, 8
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0
    mov rbx, 1
    cmovle rax, rbx
    cmp rax, 0
    jne .L19         ; Guard passed, execute case
    pop rax          ; Clean up switch value
    jmp .L20         ; Jump to default/end
.L17:  ; case 0

    ; Print statement
    mov rax, 700
    mov rdi, rax
    call print_number
    jmp .L16         ; Fall through to end
.L18:  ; case 1

    ; Print statement
    mov rax, 800
    mov rdi, rax
    call print_number
    jmp .L16         ; Fall through to end
.L19:  ; case 2

    ; Print statement
    mov rax, 900
    mov rdi, rax
    call print_number
    jmp .L16         ; Fall through to end
.L20:  ; default

    ; Print statement
    mov rax, 0
    mov rdi, rax
    call print_number
.L16:  ; switch_end

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
