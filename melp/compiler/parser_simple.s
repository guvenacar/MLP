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


func_parse_simple:
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
    ; Declaration: numeric TOKEN_TEXT_KW
    sub rsp, 8         ; Allocate space for TOKEN_TEXT_KW
    mov rax, 2
    mov [rbp-24], rax   ; Initialize TOKEN_TEXT_KW
    ; Declaration: numeric TOKEN_NUMBER
    sub rsp, 8         ; Allocate space for TOKEN_NUMBER
    mov rax, 3
    mov [rbp-32], rax   ; Initialize TOKEN_NUMBER
    ; Declaration: numeric TOKEN_IDENTIFIER
    sub rsp, 8         ; Allocate space for TOKEN_IDENTIFIER
    mov rax, 5
    mov [rbp-40], rax   ; Initialize TOKEN_IDENTIFIER
    ; Declaration: numeric TOKEN_ASSIGN
    sub rsp, 8         ; Allocate space for TOKEN_ASSIGN
    mov rax, 20
    mov [rbp-48], rax   ; Initialize TOKEN_ASSIGN
    ; Declaration: text source_code
    sub rsp, 8         ; Allocate space for source_code
    mov rax, str_0
    mov rdi, rax
    call mlp_file_read
    mov [rbp-56], rax   ; Initialize source_code
    ; Declaration: numeric source_len
    sub rsp, 8         ; Allocate space for source_len
    mov rax, [rbp-56]
    mov rdi, rax
    call mlp_string_length
    mov [rbp-64], rax   ; Initialize source_len
    ; Declaration: numeric[] token_types (dynamic array, pointer)
    sub rsp, 8         ; Allocate space for pointer token_types
    mov rax, 100
    mov rdi, rax
    call mlp_array_alloc
    mov [rbp-72], rax   ; Store array pointer to token_types
    ; Declaration: text[] token_values (dynamic array, pointer)
    sub rsp, 8         ; Allocate space for pointer token_values
    mov rax, 100
    mov rdi, rax
    call mlp_array_alloc
    mov [rbp-80], rax   ; Store array pointer to token_values
    ; Declaration: numeric token_count
    sub rsp, 8         ; Allocate space for token_count
    mov rax, 0
    mov [rbp-88], rax   ; Initialize token_count
    ; Declaration: numeric pos
    sub rsp, 8         ; Allocate space for pos
    mov rax, 0
    mov [rbp-96], rax   ; Initialize pos

    ; While loop
.L0:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-96]
    push rax
    mov rax, [rbp-64]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L1
    ; While body
    ; Declaration: text ch
    sub rsp, 8         ; Allocate space for ch
    mov rax, [rbp-56]
    mov rdi, rax
    mov rax, [rbp-96]
    mov rsi, rax
    call mlp_charAt
    mov [rbp-104], rax   ; Initialize ch

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_1
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L2
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-96]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-96], rax   ; Store to pos
    jmp .L3
.L2:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_2
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L4
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-96]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-96], rax   ; Store to pos
    jmp .L5
.L4:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    call mlp_string_length
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L6
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-96]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-96], rax   ; Store to pos
    jmp .L7
.L6:
    ; Else body
    ; Declaration: numeric is_digit
    sub rsp, 8         ; Allocate space for is_digit
    mov rax, 0
    mov [rbp-112], rax   ; Initialize is_digit

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_3
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L8
    ; Then body

    ; Assignment: is_digit = ...
    mov rax, 1
    mov [rbp-112], rax   ; Store to is_digit
.L8:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_4
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L10
    ; Then body

    ; Assignment: is_digit = ...
    mov rax, 1
    mov [rbp-112], rax   ; Store to is_digit
.L10:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_5
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L12
    ; Then body

    ; Assignment: is_digit = ...
    mov rax, 1
    mov [rbp-112], rax   ; Store to is_digit
.L12:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_6
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L14
    ; Then body

    ; Assignment: is_digit = ...
    mov rax, 1
    mov [rbp-112], rax   ; Store to is_digit
.L14:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_7
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L16
    ; Then body

    ; Assignment: is_digit = ...
    mov rax, 1
    mov [rbp-112], rax   ; Store to is_digit
.L16:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_8
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L18
    ; Then body

    ; Assignment: is_digit = ...
    mov rax, 1
    mov [rbp-112], rax   ; Store to is_digit
.L18:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_9
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L20
    ; Then body

    ; Assignment: is_digit = ...
    mov rax, 1
    mov [rbp-112], rax   ; Store to is_digit
.L20:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_10
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L22
    ; Then body

    ; Assignment: is_digit = ...
    mov rax, 1
    mov [rbp-112], rax   ; Store to is_digit
.L22:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_11
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L24
    ; Then body

    ; Assignment: is_digit = ...
    mov rax, 1
    mov [rbp-112], rax   ; Store to is_digit
.L24:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_12
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L26
    ; Then body

    ; Assignment: is_digit = ...
    mov rax, 1
    mov [rbp-112], rax   ; Store to is_digit
.L26:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L28
    ; Then body
    ; Declaration: numeric start
    sub rsp, 8         ; Allocate space for start
    mov rax, [rbp-96]
    mov [rbp-120], rax   ; Initialize start
    ; Declaration: numeric done
    sub rsp, 8         ; Allocate space for done
    mov rax, 0
    mov [rbp-128], rax   ; Initialize done

    ; While loop
.L30:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-128]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L31
    ; While body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-96]
    push rax
    mov rax, [rbp-64]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L32
    ; Then body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-128], rax   ; Store to done
    jmp .L33
.L32:
    ; Else body
    ; Declaration: text numch
    sub rsp, 8         ; Allocate space for numch
    mov rax, [rbp-56]
    mov rdi, rax
    mov rax, [rbp-96]
    mov rsi, rax
    call mlp_charAt
    mov [rbp-136], rax   ; Initialize numch
    ; Declaration: numeric is_numchar
    sub rsp, 8         ; Allocate space for is_numchar
    mov rax, 0
    mov [rbp-144], rax   ; Initialize is_numchar

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-136]
    mov rdi, rax
    push rdi
    mov rax, str_13
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L34
    ; Then body

    ; Assignment: is_numchar = ...
    mov rax, 1
    mov [rbp-144], rax   ; Store to is_numchar
.L34:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-136]
    mov rdi, rax
    push rdi
    mov rax, str_14
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L36
    ; Then body

    ; Assignment: is_numchar = ...
    mov rax, 1
    mov [rbp-144], rax   ; Store to is_numchar
.L36:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-136]
    mov rdi, rax
    push rdi
    mov rax, str_15
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L38
    ; Then body

    ; Assignment: is_numchar = ...
    mov rax, 1
    mov [rbp-144], rax   ; Store to is_numchar
.L38:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-136]
    mov rdi, rax
    push rdi
    mov rax, str_16
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L40
    ; Then body

    ; Assignment: is_numchar = ...
    mov rax, 1
    mov [rbp-144], rax   ; Store to is_numchar
.L40:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-136]
    mov rdi, rax
    push rdi
    mov rax, str_17
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L42
    ; Then body

    ; Assignment: is_numchar = ...
    mov rax, 1
    mov [rbp-144], rax   ; Store to is_numchar
.L42:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-136]
    mov rdi, rax
    push rdi
    mov rax, str_18
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L44
    ; Then body

    ; Assignment: is_numchar = ...
    mov rax, 1
    mov [rbp-144], rax   ; Store to is_numchar
.L44:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-136]
    mov rdi, rax
    push rdi
    mov rax, str_19
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L46
    ; Then body

    ; Assignment: is_numchar = ...
    mov rax, 1
    mov [rbp-144], rax   ; Store to is_numchar
.L46:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-136]
    mov rdi, rax
    push rdi
    mov rax, str_20
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L48
    ; Then body

    ; Assignment: is_numchar = ...
    mov rax, 1
    mov [rbp-144], rax   ; Store to is_numchar
.L48:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-136]
    mov rdi, rax
    push rdi
    mov rax, str_21
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L50
    ; Then body

    ; Assignment: is_numchar = ...
    mov rax, 1
    mov [rbp-144], rax   ; Store to is_numchar
.L50:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-136]
    mov rdi, rax
    push rdi
    mov rax, str_22
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L52
    ; Then body

    ; Assignment: is_numchar = ...
    mov rax, 1
    mov [rbp-144], rax   ; Store to is_numchar
.L52:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-144]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L54
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

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-128], rax   ; Store to done
.L55:
.L33:
    jmp .L30
.L31:  ; while_end
    ; Declaration: numeric num_len
    sub rsp, 8         ; Allocate space for num_len
    mov rax, [rbp-96]
    push rax
    mov rax, [rbp-120]
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-152], rax   ; Initialize num_len
    ; Declaration: text num_str
    sub rsp, 8         ; Allocate space for num_str
    mov rax, [rbp-56]
    mov rdi, rax
    mov rax, [rbp-120]
    mov rsi, rax
    mov rax, [rbp-152]
    mov rdx, rax
    call mlp_substring
    mov [rbp-160], rax   ; Initialize num_str

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, [rbp-32]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-72]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, [rbp-160]
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
    jmp .L29
.L28:
    ; Else body
    ; Declaration: numeric idx
    sub rsp, 8         ; Allocate space for idx
    mov rax, str_23
    mov rdi, rax
    mov rax, [rbp-104]
    mov rsi, rax
    call mlp_indexOf
    mov [rbp-168], rax   ; Initialize idx

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-168]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L56
    ; Then body
    ; Declaration: numeric start
    sub rsp, 8         ; Allocate space for start
    mov rax, [rbp-96]
    mov [rbp-176], rax   ; Initialize start
    ; Declaration: numeric done
    sub rsp, 8         ; Allocate space for done
    mov rax, 0
    mov [rbp-184], rax   ; Initialize done

    ; While loop
.L58:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-184]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L59
    ; While body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-96]
    push rax
    mov rax, [rbp-64]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L60
    ; Then body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-184], rax   ; Store to done
    jmp .L61
.L60:
    ; Else body
    ; Declaration: text idch
    sub rsp, 8         ; Allocate space for idch
    mov rax, [rbp-56]
    mov rdi, rax
    mov rax, [rbp-96]
    mov rsi, rax
    call mlp_charAt
    mov [rbp-192], rax   ; Initialize idch
    ; Declaration: numeric idx2
    sub rsp, 8         ; Allocate space for idx2
    mov rax, str_24
    mov rdi, rax
    mov rax, [rbp-192]
    mov rsi, rax
    call mlp_indexOf
    mov [rbp-200], rax   ; Initialize idx2

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-200]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L62
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-96]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-96], rax   ; Store to pos
    jmp .L63
.L62:
    ; Else body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-184], rax   ; Store to done
.L63:
.L61:
    jmp .L58
.L59:  ; while_end
    ; Declaration: numeric id_len
    sub rsp, 8         ; Allocate space for id_len
    mov rax, [rbp-96]
    push rax
    mov rax, [rbp-176]
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-208], rax   ; Initialize id_len
    ; Declaration: text word
    sub rsp, 8         ; Allocate space for word
    mov rax, [rbp-56]
    mov rdi, rax
    mov rax, [rbp-176]
    mov rsi, rax
    mov rax, [rbp-208]
    mov rdx, rax
    call mlp_substring
    mov [rbp-216], rax   ; Initialize word

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-216]
    mov rdi, rax
    push rdi
    mov rax, str_25
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L64
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, [rbp-16]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-72]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, [rbp-216]
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
    jmp .L65
.L64:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-216]
    mov rdi, rax
    push rdi
    mov rax, str_26
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L66
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, [rbp-40]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-72]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, [rbp-216]
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
    jmp .L67
.L66:
    ; Else body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, [rbp-40]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-72]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, [rbp-216]
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
.L67:
.L65:
    jmp .L57
.L56:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_27
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L68
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, [rbp-48]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-72]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, str_28
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
    jmp .L69
.L68:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_29
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L70
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-96]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-96], rax   ; Store to pos
    jmp .L71
.L70:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-104]
    mov rdi, rax
    push rdi
    mov rax, str_30
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L72
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-96]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-96], rax   ; Store to pos
    jmp .L73
.L72:
    ; Else body

    ; Assignment: pos = ...
    mov rax, [rbp-96]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-96], rax   ; Store to pos
.L73:
.L71:
.L69:
.L57:
.L29:
.L7:
.L5:
.L3:
    jmp .L0
.L1:  ; while_end

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, [rbp-8]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-72]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-88]
    push rax            ; Save index
    mov rax, str_31
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
    mov rax, str_32
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_33
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-88]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_34
    mov rdi, rax
    call print_string
    ; Declaration: numeric tok_pos
    sub rsp, 8         ; Allocate space for tok_pos
    mov rax, 0
    mov [rbp-224], rax   ; Initialize tok_pos

    ; If statement

    ; Evaluate comparison

    ; Array indexing
    mov rax, [rbp-224]
    push rax            ; Save index
    mov rbx, [rbp-72]   ; Load array pointer
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
    jne .L74
    ; Then body

    ; Print statement
    mov rax, str_35
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, [rbp-224]
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
    mov rax, str_36
    mov rdi, rax
    call print_string

    ; Assignment: tok_pos = ...
    mov rax, [rbp-224]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-224], rax   ; Store to tok_pos

    ; If statement

    ; Evaluate comparison

    ; Array indexing
    mov rax, [rbp-224]
    push rax            ; Save index
    mov rbx, [rbp-72]   ; Load array pointer
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
    jne .L76
    ; Then body

    ; Print statement
    mov rax, str_37
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, [rbp-224]
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
    mov rax, str_38
    mov rdi, rax
    call print_string
    ; Declaration: text var_name
    sub rsp, 8         ; Allocate space for var_name

    ; Array indexing
    mov rax, [rbp-224]
    push rax            ; Save index
    mov rbx, [rbp-80]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov [rbp-232], rax   ; Initialize var_name

    ; Assignment: tok_pos = ...
    mov rax, [rbp-224]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-224], rax   ; Store to tok_pos

    ; If statement

    ; Evaluate comparison

    ; Array indexing
    mov rax, [rbp-224]
    push rax            ; Save index
    mov rbx, [rbp-72]   ; Load array pointer
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
    jne .L78
    ; Then body

    ; Print statement
    mov rax, str_39
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_40
    mov rdi, rax
    call print_string

    ; Assignment: tok_pos = ...
    mov rax, [rbp-224]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-224], rax   ; Store to tok_pos

    ; If statement

    ; Evaluate comparison

    ; Array indexing
    mov rax, [rbp-224]
    push rax            ; Save index
    mov rbx, [rbp-72]   ; Load array pointer
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
    jne .L80
    ; Then body

    ; Print statement
    mov rax, str_41
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, [rbp-224]
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
    mov rax, str_42
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_43
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_44
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-232]
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_45
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, [rbp-224]
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
    mov rax, str_46
    mov rdi, rax
    call print_string
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L80:
.L78:
.L76:
.L74:

    ; Print statement
    mov rax, str_47
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_48
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
    call func_parse_simple
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
str_48: db " ", 0
str_47: db "Parse error", 0
str_46: db " ", 0
str_45: db " = ", 0
str_44: db "Declaration: numeric ", 0
str_43: db "=== PARSE SUCCESS ===", 0
str_42: db " ", 0
str_41: db "Found value: ", 0
str_40: db " ", 0
str_39: db "Found assignment", 0
str_38: db " ", 0
str_37: db "Found variable: ", 0
str_36: db " ", 0
str_35: db "Found type: ", 0
str_34: db " ", 0
str_33: db "Count: ", 0
str_32: db "=== TOKENS ===", 0
str_31: db "", 0
str_30: db ")", 0
str_29: db "(", 0
str_28: db "=", 0
str_27: db "=", 0
str_26: db "print", 0
str_25: db "numeric", 0
str_24: db "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789", 0
str_23: db "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", 0
str_22: db "9", 0
str_21: db "8", 0
str_20: db "7", 0
str_19: db "6", 0
str_18: db "5", 0
str_17: db "4", 0
str_16: db "3", 0
str_15: db "2", 0
str_14: db "1", 0
str_13: db "0", 0
str_12: db "9", 0
str_11: db "8", 0
str_10: db "7", 0
str_9: db "6", 0
str_8: db "5", 0
str_7: db "4", 0
str_6: db "3", 0
str_5: db "2", 0
str_4: db "1", 0
str_3: db "0", 0
str_2: db "	", 0
str_1: db " ", 0
str_0: db "compiler/test_simple.mlp", 0
