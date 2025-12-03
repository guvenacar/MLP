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


func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: numeric TOKEN_EOF
    sub rsp, 8         ; Allocate space for TOKEN_EOF
    mov rax, 0
    mov [rbp-8], rax   ; Initialize TOKEN_EOF
    ; Declaration: numeric TOKEN_NUMERIC_KW
    sub rsp, 8         ; Allocate space for TOKEN_NUMERIC_KW
    mov rax, 1
    mov [rbp-16], rax   ; Initialize TOKEN_NUMERIC_KW
    ; Declaration: numeric TOKEN_NUMBER
    sub rsp, 8         ; Allocate space for TOKEN_NUMBER
    mov rax, 3
    mov [rbp-24], rax   ; Initialize TOKEN_NUMBER
    ; Declaration: numeric TOKEN_IDENTIFIER
    sub rsp, 8         ; Allocate space for TOKEN_IDENTIFIER
    mov rax, 5
    mov [rbp-32], rax   ; Initialize TOKEN_IDENTIFIER
    ; Declaration: numeric TOKEN_ASSIGN
    sub rsp, 8         ; Allocate space for TOKEN_ASSIGN
    mov rax, 20
    mov [rbp-40], rax   ; Initialize TOKEN_ASSIGN
    ; Declaration: numeric[] token_types (dynamic array, pointer)
    sub rsp, 8         ; Allocate space for pointer token_types
    mov rax, 10
    mov rdi, rax
    call mlp_array_alloc
    mov [rbp-48], rax   ; Store array pointer to token_types
    ; Declaration: text[] token_values (dynamic array, pointer)
    sub rsp, 8         ; Allocate space for pointer token_values
    mov rax, 10
    mov rdi, rax
    call mlp_array_alloc
    mov [rbp-56], rax   ; Store array pointer to token_values

    ; Array assignment: token_types[...] = ...
    mov rax, 0
    push rax            ; Save index
    mov rax, [rbp-16]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-48]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, 0
    push rax            ; Save index
    mov rax, str_0
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-56]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_types[...] = ...
    mov rax, 1
    push rax            ; Save index
    mov rax, [rbp-32]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-48]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, 1
    push rax            ; Save index
    mov rax, str_1
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-56]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_types[...] = ...
    mov rax, 2
    push rax            ; Save index
    mov rax, [rbp-40]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-48]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, 2
    push rax            ; Save index
    mov rax, str_2
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-56]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_types[...] = ...
    mov rax, 3
    push rax            ; Save index
    mov rax, [rbp-24]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-48]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, 3
    push rax            ; Save index
    mov rax, str_3
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-56]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_types[...] = ...
    mov rax, 4
    push rax            ; Save index
    mov rax, [rbp-8]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-48]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, 4
    push rax            ; Save index
    mov rax, str_4
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-56]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]
    ; Declaration: numeric token_count
    sub rsp, 8         ; Allocate space for token_count
    mov rax, 5
    mov [rbp-64], rax   ; Initialize token_count

    ; Print statement
    mov rax, str_5
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_6
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, 0
    push rax            ; Save index
    mov rbx, [rbp-56]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_7
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_8
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, 1
    push rax            ; Save index
    mov rbx, [rbp-56]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_9
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_10
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, 2
    push rax            ; Save index
    mov rbx, [rbp-56]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_11
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_12
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, 3
    push rax            ; Save index
    mov rbx, [rbp-56]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_13
    mov rdi, rax
    call print_string
    ; Declaration: numeric tok_pos
    sub rsp, 8         ; Allocate space for tok_pos
    mov rax, 0
    mov [rbp-72], rax   ; Initialize tok_pos

    ; If statement

    ; Evaluate comparison

    ; Array indexing
    mov rax, [rbp-72]
    push rax            ; Save index
    mov rbx, [rbp-48]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L0
    ; Then body

    ; Print statement
    mov rax, str_14
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_15
    mov rdi, rax
    call print_string

    ; Assignment: tok_pos = ...
    mov rax, [rbp-72]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-72], rax   ; Store to tok_pos

    ; If statement

    ; Evaluate comparison

    ; Array indexing
    mov rax, [rbp-72]
    push rax            ; Save index
    mov rbx, [rbp-48]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    push rax
    mov rax, [rbp-32]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L2
    ; Then body
    ; Declaration: text var_name
    sub rsp, 8         ; Allocate space for var_name

    ; Array indexing
    mov rax, [rbp-72]
    push rax            ; Save index
    mov rbx, [rbp-56]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov [rbp-80], rax   ; Initialize var_name

    ; Print statement
    mov rax, str_16
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-80]
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_17
    mov rdi, rax
    call print_string

    ; Assignment: tok_pos = ...
    mov rax, [rbp-72]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-72], rax   ; Store to tok_pos

    ; If statement

    ; Evaluate comparison

    ; Array indexing
    mov rax, [rbp-72]
    push rax            ; Save index
    mov rbx, [rbp-48]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    push rax
    mov rax, [rbp-40]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L4
    ; Then body

    ; Print statement
    mov rax, str_18
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_19
    mov rdi, rax
    call print_string

    ; Assignment: tok_pos = ...
    mov rax, [rbp-72]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-72], rax   ; Store to tok_pos

    ; If statement

    ; Evaluate comparison

    ; Array indexing
    mov rax, [rbp-72]
    push rax            ; Save index
    mov rbx, [rbp-48]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    push rax
    mov rax, [rbp-24]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L6
    ; Then body
    ; Declaration: text var_value
    sub rsp, 8         ; Allocate space for var_value

    ; Array indexing
    mov rax, [rbp-72]
    push rax            ; Save index
    mov rbx, [rbp-56]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov [rbp-88], rax   ; Initialize var_value

    ; Print statement
    mov rax, str_20
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-88]
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_21
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_22
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_23
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-80]
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_24
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-88]
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_25
    mov rdi, rax
    call print_string
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret
.L6:
.L4:
.L2:
.L0:

    ; Print statement
    mov rax, str_26
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_27
    mov rdi, rax
    call print_string
    mov rax, 1
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

    ; Call main function
    call func_main

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_27: db " ", 0
str_26: db "Parse failed", 0
str_25: db " ", 0
str_24: db " = ", 0
str_23: db "Declaration: numeric ", 0
str_22: db "=== PARSE SUCCESS ===", 0
str_21: db " ", 0
str_20: db "✓ Found value: ", 0
str_19: db " ", 0
str_18: db "✓ Found assignment operator", 0
str_17: db " ", 0
str_16: db "✓ Found variable: ", 0
str_15: db " ", 0
str_14: db "✓ Found type declaration", 0
str_13: db " ", 0
str_12: db "Token 3: ", 0
str_11: db " ", 0
str_10: db "Token 2: ", 0
str_9: db " ", 0
str_8: db "Token 1: ", 0
str_7: db " ", 0
str_6: db "Token 0: ", 0
str_5: db "=== MANUAL TOKENS ===", 0
str_4: db "", 0
str_3: db "42", 0
str_2: db "=", 0
str_1: db "x", 0
str_0: db "numeric", 0
