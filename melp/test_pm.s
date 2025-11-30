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

    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, 3
    mov [rbp-8], rax   ; Initialize x

    ; Pattern match statement
    mov rax, [rbp-8]
    push rax           ; Save match value
    ; case pattern(s)
    mov rax, 1
    mov rbx, rax        ; Pattern value
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    jne .L1           ; Skip if not equal

    ; Print statement
    mov rax, 100
    mov rdi, rax
    call print_number
    jmp .L0           ; End of match
.L1:
    ; case pattern(s)
    mov rax, 2
    mov rbx, rax        ; Pattern value
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    jne .L2           ; Skip if not equal

    ; Print statement
    mov rax, 200
    mov rdi, rax
    call print_number
    jmp .L0           ; End of match
.L2:
    ; case pattern(s)
    mov rax, 3
    mov rbx, rax        ; Pattern value
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    jne .L3           ; Skip if not equal

    ; Print statement
    mov rax, 300
    mov rdi, rax
    call print_number
    jmp .L0           ; End of match
.L3:
    ; case _ (wildcard/default)

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number
    jmp .L0           ; End of match
.L4:
.L0:
    add rsp, 8          ; Pop match value


    ; Print statement
    mov rax, 1000
    mov rdi, rax
    call print_number
    ; Declaration: numeric y
    sub rsp, 8         ; Allocate space for y
    mov rax, 5
    mov [rbp-16], rax   ; Initialize y

    ; Pattern match statement
    mov rax, [rbp-16]
    push rax           ; Save match value
    ; case pattern(s)
    mov rax, 1
    mov rbx, rax        ; Pattern value
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    je .L7            ; Match found
    mov rax, 2
    mov rbx, rax        ; Pattern value
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    je .L7            ; Match found
    jmp .L6
.L7:

    ; Print statement
    mov rax, 110
    mov rdi, rax
    call print_number
    jmp .L5           ; End of match
.L6:
    ; case pattern(s)
    mov rax, 3
    mov rbx, rax        ; Pattern value
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    je .L9            ; Match found
    mov rax, 4
    mov rbx, rax        ; Pattern value
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    je .L9            ; Match found
    mov rax, 5
    mov rbx, rax        ; Pattern value
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    je .L9            ; Match found
    jmp .L8
.L9:

    ; Print statement
    mov rax, 220
    mov rdi, rax
    call print_number
    jmp .L5           ; End of match
.L8:
    ; case pattern(s)
    mov rax, 6
    mov rbx, rax        ; Pattern value
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    je .L11            ; Match found
    mov rax, 7
    mov rbx, rax        ; Pattern value
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    je .L11            ; Match found
    jmp .L10
.L11:

    ; Print statement
    mov rax, 330
    mov rdi, rax
    call print_number
    jmp .L5           ; End of match
.L10:
    ; case _ (wildcard/default)

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number
    jmp .L5           ; End of match
.L12:
.L5:
    add rsp, 8          ; Pop match value


    ; Print statement
    mov rax, 2000
    mov rdi, rax
    call print_number
    ; Declaration: numeric z
    sub rsp, 8         ; Allocate space for z
    mov rax, 7
    mov [rbp-24], rax   ; Initialize z

    ; Pattern match statement
    mov rax, [rbp-24]
    push rax           ; Save match value
    ; case range pattern
    mov rax, [rsp]      ; Load match value
    mov rax, 1
    mov rbx, rax        ; Lower bound in rbx
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    jl .L14            ; Skip if value < lower
    mov rax, 5
    mov rbx, rax        ; Upper bound in rbx
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    jg .L14            ; Skip if value > upper

    ; Print statement
    mov rax, 111
    mov rdi, rax
    call print_number
    jmp .L13           ; End of match
.L14:
    ; case range pattern
    mov rax, [rsp]      ; Load match value
    mov rax, 6
    mov rbx, rax        ; Lower bound in rbx
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    jl .L15            ; Skip if value < lower
    mov rax, 10
    mov rbx, rax        ; Upper bound in rbx
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    jg .L15            ; Skip if value > upper

    ; Print statement
    mov rax, 222
    mov rdi, rax
    call print_number
    jmp .L13           ; End of match
.L15:
    ; case range pattern
    mov rax, [rsp]      ; Load match value
    mov rax, 11
    mov rbx, rax        ; Lower bound in rbx
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    jl .L16            ; Skip if value < lower
    mov rax, 20
    mov rbx, rax        ; Upper bound in rbx
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    jg .L16            ; Skip if value > upper

    ; Print statement
    mov rax, 333
    mov rdi, rax
    call print_number
    jmp .L13           ; End of match
.L16:
    ; case _ (wildcard/default)

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number
    jmp .L13           ; End of match
.L17:
.L13:
    add rsp, 8          ; Pop match value


    ; Print statement
    mov rax, 3000
    mov rdi, rax
    call print_number
    ; Declaration: numeric w
    sub rsp, 8         ; Allocate space for w
    mov rax, 100
    mov [rbp-32], rax   ; Initialize w

    ; Pattern match statement
    mov rax, [rbp-32]
    push rax           ; Save match value
    ; case pattern(s)
    mov rax, 1
    mov rbx, rax        ; Pattern value
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    jne .L19           ; Skip if not equal

    ; Print statement
    mov rax, 10
    mov rdi, rax
    call print_number
    jmp .L18           ; End of match
.L19:
    ; case pattern(s)
    mov rax, 2
    mov rbx, rax        ; Pattern value
    mov rax, [rsp]      ; Match value
    cmp rax, rbx
    jne .L20           ; Skip if not equal

    ; Print statement
    mov rax, 20
    mov rdi, rax
    call print_number
    jmp .L18           ; End of match
.L20:
    ; case _ (wildcard/default)

    ; Print statement
    mov rax, 444
    mov rdi, rax
    call print_number
    jmp .L18           ; End of match
.L21:
.L18:
    add rsp, 8          ; Pop match value


    ; Print statement
    mov rax, 4000
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
