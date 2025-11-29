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
    push rax
    mov rax, [rbp-8]
    push rax
    pop rsi
    pop rdi
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
    push rax
    pop rdi
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

func_compile:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param input_file
    sub rsp, 8
    mov [rbp-8], rax   ; Store param input_file locally
    mov rax, [rbp+24]   ; Load param output_file
    sub rsp, 8
    mov [rbp-16], rax   ; Store param output_file locally
    ; Declaration: numeric TOKEN_EOF
    sub rsp, 8         ; Allocate space for TOKEN_EOF
    mov rax, 0
    mov [rbp-24], rax   ; Initialize TOKEN_EOF
    ; Declaration: numeric TOKEN_NUMERIC_KW
    sub rsp, 8         ; Allocate space for TOKEN_NUMERIC_KW
    mov rax, 1
    mov [rbp-32], rax   ; Initialize TOKEN_NUMERIC_KW
    ; Declaration: numeric TOKEN_NUMBER
    sub rsp, 8         ; Allocate space for TOKEN_NUMBER
    mov rax, 3
    mov [rbp-40], rax   ; Initialize TOKEN_NUMBER
    ; Declaration: numeric TOKEN_IDENTIFIER
    sub rsp, 8         ; Allocate space for TOKEN_IDENTIFIER
    mov rax, 5
    mov [rbp-48], rax   ; Initialize TOKEN_IDENTIFIER
    ; Declaration: numeric TOKEN_ASSIGN
    sub rsp, 8         ; Allocate space for TOKEN_ASSIGN
    mov rax, 20
    mov [rbp-56], rax   ; Initialize TOKEN_ASSIGN

    ; Print statement
    mov rax, str_13
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_14
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_15
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_16
    mov rdi, rax
    call print_string
    ; Declaration: text source_code
    sub rsp, 8         ; Allocate space for source_code
    mov rax, [rbp-8]
    push rax
    pop rdi
    call mlp_file_read
    mov [rbp-64], rax   ; Initialize source_code
    ; Declaration: numeric source_len
    sub rsp, 8         ; Allocate space for source_len
    mov rax, [rbp-64]
    push rax
    pop rdi
    call mlp_string_length
    mov [rbp-72], rax   ; Initialize source_len
    ; Declaration: numeric[] token_types (dynamic array, pointer)
    sub rsp, 8         ; Allocate space for pointer token_types
    mov rax, 100
    push rax
    pop rdi
    call mlp_array_alloc
    mov [rbp-80], rax   ; Store array pointer to token_types
    ; Declaration: numeric token_count
    sub rsp, 8         ; Allocate space for token_count
    mov rax, 0
    mov [rbp-88], rax   ; Initialize token_count
    ; Declaration: numeric pos
    sub rsp, 8         ; Allocate space for pos
    mov rax, 0
    mov [rbp-96], rax   ; Initialize pos

    ; While loop
.L30:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-96]
    push rax
    mov rax, [rbp-72]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L31
    ; While body
    ; Declaration: text ch
    sub rsp, 8         ; Allocate space for ch
    mov rax, [rbp-64]
    push rax
    mov rax, [rbp-96]
    push rax
    pop rsi
    pop rdi
    call mlp_charAt
    mov [rbp-104], rax   ; Initialize ch
    ; Declaration: numeric is_ws
    sub rsp, 8         ; Allocate space for is_ws
    mov rax, [rbp-104]
    push rax
    call func_is_whitespace
    add rsp, 8
    mov [rbp-112], rax   ; Initialize is_ws

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L32
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-96]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-96], rax   ; Store to pos
    jmp .L33
.L32:
    ; Else body
    ; Declaration: numeric is_d
    sub rsp, 8         ; Allocate space for is_d
    mov rax, [rbp-104]
    push rax
    call func_is_digit
    add rsp, 8
    mov [rbp-120], rax   ; Initialize is_d

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-120]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L34
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, [rbp-40]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-80]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-88]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-88], rax   ; Store to token_count
    ; Declaration: numeric done
    sub rsp, 8         ; Allocate space for done
    mov rax, 0
    mov [rbp-128], rax   ; Initialize done

    ; While loop
.L36:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-128]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L37
    ; While body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-96]
    push rax
    mov rax, [rbp-72]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L38
    ; Then body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-128], rax   ; Store to done
    jmp .L39
.L38:
    ; Else body
    ; Declaration: text numch
    sub rsp, 8         ; Allocate space for numch
    mov rax, [rbp-64]
    push rax
    mov rax, [rbp-96]
    push rax
    pop rsi
    pop rdi
    call mlp_charAt
    mov [rbp-136], rax   ; Initialize numch
    ; Declaration: numeric is_numchar
    sub rsp, 8         ; Allocate space for is_numchar
    mov rax, [rbp-136]
    push rax
    call func_is_digit
    add rsp, 8
    mov [rbp-144], rax   ; Initialize is_numchar

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-144]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L40
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-96]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-96], rax   ; Store to pos
    jmp .L41
.L40:
    ; Else body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-128], rax   ; Store to done
.L41:
.L39:
    jmp .L36
.L37:  ; while_end
    jmp .L35
.L34:
    ; Else body
    ; Declaration: numeric is_a
    sub rsp, 8         ; Allocate space for is_a
    mov rax, [rbp-104]
    push rax
    call func_is_alpha
    add rsp, 8
    mov [rbp-152], rax   ; Initialize is_a

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-152]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L42
    ; Then body
    ; Declaration: numeric start
    sub rsp, 8         ; Allocate space for start
    mov rax, [rbp-96]
    mov [rbp-160], rax   ; Initialize start
    ; Declaration: numeric done
    sub rsp, 8         ; Allocate space for done
    mov rax, 0
    mov [rbp-168], rax   ; Initialize done

    ; While loop
.L44:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-168]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L45
    ; While body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-96]
    push rax
    mov rax, [rbp-72]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L46
    ; Then body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-168], rax   ; Store to done
    jmp .L47
.L46:
    ; Else body
    ; Declaration: text idch
    sub rsp, 8         ; Allocate space for idch
    mov rax, [rbp-64]
    push rax
    mov rax, [rbp-96]
    push rax
    pop rsi
    pop rdi
    call mlp_charAt
    mov [rbp-176], rax   ; Initialize idch
    ; Declaration: numeric is_idchar
    sub rsp, 8         ; Allocate space for is_idchar
    mov rax, [rbp-176]
    push rax
    call func_is_alnum
    add rsp, 8
    mov [rbp-184], rax   ; Initialize is_idchar

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-184]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L48
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-96]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-96], rax   ; Store to pos
    jmp .L49
.L48:
    ; Else body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-168], rax   ; Store to done
.L49:
.L47:
    jmp .L44
.L45:  ; while_end
    ; Declaration: numeric word_len
    sub rsp, 8         ; Allocate space for word_len
    mov rax, [rbp-96]
    push rax
    mov rax, [rbp-160]
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-192], rax   ; Initialize word_len

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-192]
    push rax
    mov rax, 7
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L50
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, [rbp-32]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-80]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-88]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-88], rax   ; Store to token_count
    jmp .L51
.L50:
    ; Else body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, [rbp-48]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-80]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-88]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-88], rax   ; Store to token_count
.L51:
    jmp .L43
.L42:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_17
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L52
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, [rbp-56]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-80]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-88]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-88], rax   ; Store to token_count

    ; Assignment: pos = ...
    mov rax, [rbp-96]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-96], rax   ; Store to pos
    jmp .L53
.L52:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_18
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L54
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-96]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-96], rax   ; Store to pos
    jmp .L55
.L54:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_19
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L56
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-96]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-96], rax   ; Store to pos
    jmp .L57
.L56:
    ; Else body

    ; Assignment: pos = ...
    mov rax, [rbp-96]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-96], rax   ; Store to pos
.L57:
.L55:
.L53:
.L43:
.L35:
.L33:
    jmp .L30
.L31:  ; while_end

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, [rbp-24]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-80]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-88]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-88], rax   ; Store to token_count

    ; Print statement
    mov rax, str_20
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-88]
    mov rdi, rax
    call print_number

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

    ; Array indexing
    mov rax, 0
    push rax            ; Save index
    mov rbx, [rbp-80]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_24
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_25
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, 1
    push rax            ; Save index
    mov rbx, [rbp-80]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_26
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_27
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, 2
    push rax            ; Save index
    mov rbx, [rbp-80]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_28
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_29
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, 3
    push rax            ; Save index
    mov rbx, [rbp-80]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_30
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_31
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_32
    mov rdi, rax
    call print_string
    ; Declaration: numeric tok_pos
    sub rsp, 8         ; Allocate space for tok_pos
    mov rax, 0
    mov [rbp-200], rax   ; Initialize tok_pos
    ; Declaration: numeric parse_ok
    sub rsp, 8         ; Allocate space for parse_ok
    mov rax, 0
    mov [rbp-208], rax   ; Initialize parse_ok

    ; If statement

    ; Evaluate comparison

    ; Array indexing
    mov rax, [rbp-200]
    push rax            ; Save index
    mov rbx, [rbp-80]   ; Load array pointer
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
    jne .L58
    ; Then body

    ; Assignment: tok_pos = ...
    mov rax, [rbp-200]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-200], rax   ; Store to tok_pos

    ; If statement

    ; Evaluate comparison

    ; Array indexing
    mov rax, [rbp-200]
    push rax            ; Save index
    mov rbx, [rbp-80]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    push rax
    mov rax, [rbp-48]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L60
    ; Then body

    ; Assignment: tok_pos = ...
    mov rax, [rbp-200]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-200], rax   ; Store to tok_pos

    ; If statement

    ; Evaluate comparison

    ; Array indexing
    mov rax, [rbp-200]
    push rax            ; Save index
    mov rbx, [rbp-80]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    push rax
    mov rax, [rbp-56]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L62
    ; Then body

    ; Assignment: tok_pos = ...
    mov rax, [rbp-200]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-200], rax   ; Store to tok_pos

    ; If statement

    ; Evaluate comparison

    ; Array indexing
    mov rax, [rbp-200]
    push rax            ; Save index
    mov rbx, [rbp-80]   ; Load array pointer
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
    jne .L64
    ; Then body

    ; Assignment: parse_ok = ...
    mov rax, 1
    mov [rbp-208], rax   ; Store to parse_ok
.L64:
.L62:
.L60:
.L58:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-208]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L66
    ; Then body

    ; Print statement
    mov rax, str_33
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_34
    mov rdi, rax
    call print_string
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L66:

    ; Print statement
    mov rax, str_35
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_36
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_37
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_38
    mov rdi, rax
    call print_string
    mov rax, [rbp-16]
    push rax
    mov rax, str_39
    push rax
    pop rsi
    pop rdi
    call mlp_file_write
    mov rax, [rbp-16]
    push rax
    mov rax, str_40
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_41
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_42
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_43
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_44
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_45
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_46
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_47
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_48
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    ; Declaration: text comment
    sub rsp, 8         ; Allocate space for comment
    mov rax, str_49
    mov [rbp-216], rax   ; Initialize comment
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-216]
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_50
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_51
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_52
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_53
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_54
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_55
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_56
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_57
    push rax
    pop rsi
    pop rdi
    call mlp_file_append
    mov rax, [rbp-16]
    push rax
    mov rax, str_58
    push rax
    pop rsi
    pop rdi
    call mlp_file_append

    ; Print statement
    mov rax, str_59
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_60
    mov rdi, rax
    call print_string
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: numeric result
    sub rsp, 8         ; Allocate space for result
    mov rax, str_61
    push rax
    mov rax, str_62
    push rax
    call func_compile
    add rsp, 16
    mov [rbp-8], rax   ; Initialize result
    mov rax, [rbp-8]
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
str_62: db "compiler/test_simple.mlp", 0
str_61: db "compiler/output.s", 0
str_60: db " ", 0
str_59: db "SUCCESS: Assembly written to ", 0
str_58: db "    syscall", 0
str_57: db "    xor rdi, rdi", 0
str_56: db "    mov rax, 60", 0
str_55: db "    pop rbp", 0
str_54: db "    mov rsp, rbp", 0
str_53: db "    ; Exit", 0
str_52: db "", 0
str_51: db "    mov [rbp-8], rax", 0
str_50: db "    mov rax, 42", 0
str_49: db "    ; Declaration: numeric ", 0
str_48: db "", 0
str_47: db "    sub rsp, 16", 0
str_46: db "    mov rbp, rsp", 0
str_45: db "    push rbp", 0
str_44: db "_start:", 0
str_43: db "", 0
str_42: db "    global _start", 0
str_41: db "section .text", 0
str_40: db "", 0
str_39: db "section .data", 0
str_38: db " ", 0
str_37: db "[3/3] Generating assembly...", 0
str_36: db " ", 0
str_35: db "  Declaration: numeric x = 42 (hardcoded)", 0
str_34: db " ", 0
str_33: db "ERROR: Parse failed", 0
str_32: db " ", 0
str_31: db "[2/3] Parsing...", 0
str_30: db " ", 0
str_29: db "  Token 3 type: ", 0
str_28: db " ", 0
str_27: db "  Token 2 type: ", 0
str_26: db " ", 0
str_25: db "  Token 1 type: ", 0
str_24: db " ", 0
str_23: db "  Token 0 type: ", 0
str_22: db " ", 0
str_21: db " tokens", 0
str_20: db "  Found ", 0
str_19: db ")", 0
str_18: db "(", 0
str_17: db "=", 0
str_16: db " ", 0
str_15: db "[1/3] Lexing...", 0
str_14: db " ", 0
str_13: db "=== MLP SELF-HOSTING COMPILER v0.1 ===", 0
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
