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
    call mlp_string_length
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L28
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L28:
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: text source_code
    sub rsp, 8         ; Allocate space for source_code
    mov rax, str_13
    mov rdi, rax
    call mlp_file_read
    mov [rbp-8], rax   ; Initialize source_code
    ; Declaration: numeric source_len
    sub rsp, 8         ; Allocate space for source_len
    mov rax, [rbp-8]
    mov rdi, rax
    call mlp_string_length
    mov [rbp-16], rax   ; Initialize source_len

    ; Print statement
    mov rax, str_14
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_15
    mov rdi, rax
    call print_string
    ; Declaration: numeric pos
    sub rsp, 8         ; Allocate space for pos
    mov rax, 0
    mov [rbp-24], rax   ; Initialize pos
    ; Declaration: numeric token_count
    sub rsp, 8         ; Allocate space for token_count
    mov rax, 0
    mov [rbp-32], rax   ; Initialize token_count

    ; While loop
.L30:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-24]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L31
    ; While body
    ; Declaration: text ch
    sub rsp, 8         ; Allocate space for ch
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, [rbp-24]
    mov rsi, rax
    call mlp_charAt
    mov [rbp-40], rax   ; Initialize ch
    ; Declaration: numeric is_ws
    sub rsp, 8         ; Allocate space for is_ws
    mov rax, [rbp-40]
    push rax
    call func_is_whitespace
    add rsp, 8
    mov [rbp-48], rax   ; Initialize is_ws

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-48]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L32
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-24]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-24], rax   ; Store to pos
    jmp .L33
.L32:
    ; Else body
    ; Declaration: numeric is_d
    sub rsp, 8         ; Allocate space for is_d
    mov rax, [rbp-40]
    push rax
    call func_is_digit
    add rsp, 8
    mov [rbp-56], rax   ; Initialize is_d

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-56]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L34
    ; Then body

    ; Assignment: token_count = ...
    mov rax, [rbp-32]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-32], rax   ; Store to token_count
    ; Declaration: numeric done
    sub rsp, 8         ; Allocate space for done
    mov rax, 0
    mov [rbp-64], rax   ; Initialize done

    ; While loop
.L36:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-64]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L37
    ; While body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L38
    ; Then body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-64], rax   ; Store to done
    jmp .L39
.L38:
    ; Else body
    ; Declaration: text numch
    sub rsp, 8         ; Allocate space for numch
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, [rbp-24]
    mov rsi, rax
    call mlp_charAt
    mov [rbp-72], rax   ; Initialize numch
    ; Declaration: numeric is_numchar
    sub rsp, 8         ; Allocate space for is_numchar
    mov rax, [rbp-72]
    push rax
    call func_is_digit
    add rsp, 8
    mov [rbp-80], rax   ; Initialize is_numchar

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-80]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L40
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-24]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-24], rax   ; Store to pos
    jmp .L41
.L40:
    ; Else body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-64], rax   ; Store to done
.L41:
.L39:
    jmp .L36
.L37:  ; while_end
    jmp .L35
.L34:
    ; Else body
    ; Declaration: numeric is_a
    sub rsp, 8         ; Allocate space for is_a
    mov rax, [rbp-40]
    push rax
    call func_is_alpha
    add rsp, 8
    mov [rbp-88], rax   ; Initialize is_a

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-88]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L42
    ; Then body

    ; Assignment: token_count = ...
    mov rax, [rbp-32]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-32], rax   ; Store to token_count
    ; Declaration: numeric done
    sub rsp, 8         ; Allocate space for done
    mov rax, 0
    mov [rbp-96], rax   ; Initialize done

    ; While loop
.L44:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-96]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L45
    ; While body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L46
    ; Then body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-96], rax   ; Store to done
    jmp .L47
.L46:
    ; Else body
    ; Declaration: text idch
    sub rsp, 8         ; Allocate space for idch
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, [rbp-24]
    mov rsi, rax
    call mlp_charAt
    mov [rbp-104], rax   ; Initialize idch
    ; Declaration: numeric is_idchar
    sub rsp, 8         ; Allocate space for is_idchar
    mov rax, [rbp-104]
    push rax
    call func_is_alnum
    add rsp, 8
    mov [rbp-112], rax   ; Initialize is_idchar

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L48
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-24]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-24], rax   ; Store to pos
    jmp .L49
.L48:
    ; Else body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-96], rax   ; Store to done
.L49:
.L47:
    jmp .L44
.L45:  ; while_end
    jmp .L43
.L42:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-40]
    mov rdi, rax
    push rdi
    mov rax, str_16
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L50
    ; Then body

    ; Assignment: token_count = ...
    mov rax, [rbp-32]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-32], rax   ; Store to token_count

    ; Assignment: pos = ...
    mov rax, [rbp-24]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-24], rax   ; Store to pos
    jmp .L51
.L50:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-40]
    mov rdi, rax
    push rdi
    mov rax, str_17
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L52
    ; Then body

    ; Assignment: token_count = ...
    mov rax, [rbp-32]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-32], rax   ; Store to token_count

    ; Assignment: pos = ...
    mov rax, [rbp-24]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-24], rax   ; Store to pos
    jmp .L53
.L52:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-40]
    mov rdi, rax
    push rdi
    mov rax, str_18
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L54
    ; Then body

    ; Assignment: token_count = ...
    mov rax, [rbp-32]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-32], rax   ; Store to token_count

    ; Assignment: pos = ...
    mov rax, [rbp-24]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-24], rax   ; Store to pos
    jmp .L55
.L54:
    ; Else body

    ; Assignment: pos = ...
    mov rax, [rbp-24]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-24], rax   ; Store to pos
.L55:
.L53:
.L51:
.L43:
.L35:
.L33:
    jmp .L30
.L31:  ; while_end

    ; Assignment: token_count = ...
    mov rax, [rbp-32]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-32], rax   ; Store to token_count

    ; Print statement
    mov rax, str_19
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_20
    mov rdi, rax
    call print_string
    mov rax, 0
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
str_20: db " ", 0
str_19: db "Token count: ", 0
str_18: db ")", 0
str_17: db "(", 0
str_16: db "=", 0
str_15: db " ", 0
str_14: db "Source length: ", 0
str_13: db "compiler/test_simple.mlp", 0
str_12: db "	", 0
str_11: db " ", 0
str_10: db "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", 0
str_9: db "9", 0
str_8: db "8", 0
str_7: db "7", 0
str_6: db "6", 0
str_5: db "5", 0
str_4: db "4", 0
str_3: db "3", 0
str_2: db "2", 0
str_1: db "1", 0
str_0: db "0", 0
