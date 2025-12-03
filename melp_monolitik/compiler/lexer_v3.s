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


func_is_digit:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param c
    sub rsp, 8
    mov [rbp-8], rax   ; Store param c locally

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_0
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L0
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L0:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_1
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L2
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L2:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_2
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L4
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L4:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_3
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L6
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L6:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_4
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L8
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L8:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_5
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L10
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L10:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_6
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L12
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L12:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_7
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L14
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L14:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_8
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L16
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L16:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_9
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L18
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L18:
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

func_is_alpha:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param c
    sub rsp, 8
    mov [rbp-8], rax   ; Store param c locally
    ; Declaration: numeric idx
    sub rsp, 8         ; Allocate space for idx
    mov rax, str_10
    mov rdi, rax
    mov rax, [rbp-8]
    mov rsi, rax
    call mlp_indexOf
    mov [rbp-16], rax   ; Initialize idx

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-16]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L20
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L20:
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

func_is_alnum:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param c
    sub rsp, 8
    mov [rbp-8], rax   ; Store param c locally
    ; Declaration: numeric d
    sub rsp, 8         ; Allocate space for d
    mov rax, [rbp-8]
    push rax
    call func_is_digit
    add rsp, 8
    mov [rbp-16], rax   ; Initialize d

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-16]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L22
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L22:
    mov rax, [rbp-8]
    push rax
    call func_is_alpha
    add rsp, 8
    mov rsp, rbp
    pop rbp
    ret

func_is_whitespace:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param c
    sub rsp, 8
    mov [rbp-8], rax   ; Store param c locally

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_11
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L24
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L24:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_12
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L26
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L26:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_13
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L28
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L28:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_14
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L30
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L30:
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

func_keyword_type:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param word
    sub rsp, 8
    mov [rbp-8], rax   ; Store param word locally

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_15
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L32
    ; Then body
