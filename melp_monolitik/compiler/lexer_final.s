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
    mov rax, str_13
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
    jz .L32
    ; Then body
    mov rax, 2
    mov rsp, rbp
    pop rbp
    ret
.L32:

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
    jz .L34
    ; Then body
    mov rax, 6
    mov rsp, rbp
    pop rbp
    ret
.L34:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_16
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L36
    ; Then body
    mov rax, 15
    mov rsp, rbp
    pop rbp
    ret
.L36:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_17
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L38
    ; Then body
    mov rax, 16
    mov rsp, rbp
    pop rbp
    ret
.L38:
    mov rax, 5
    mov rsp, rbp
    pop rbp
    ret

func_tokenize:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param source_code
    sub rsp, 8
    mov [rbp-8], rax   ; Store param source_code locally
    ; Declaration: numeric TOKEN_EOF
    sub rsp, 8         ; Allocate space for TOKEN_EOF
    mov rax, 0
    mov [rbp-16], rax   ; Initialize TOKEN_EOF
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
    ; Declaration: numeric TOKEN_PLUS
    sub rsp, 8         ; Allocate space for TOKEN_PLUS
    mov rax, 21
    mov [rbp-48], rax   ; Initialize TOKEN_PLUS
    ; Declaration: numeric TOKEN_MINUS
    sub rsp, 8         ; Allocate space for TOKEN_MINUS
    mov rax, 22
    mov [rbp-56], rax   ; Initialize TOKEN_MINUS
    ; Declaration: numeric TOKEN_STAR
    sub rsp, 8         ; Allocate space for TOKEN_STAR
    mov rax, 23
    mov [rbp-64], rax   ; Initialize TOKEN_STAR
    ; Declaration: numeric TOKEN_SLASH
    sub rsp, 8         ; Allocate space for TOKEN_SLASH
    mov rax, 24
    mov [rbp-72], rax   ; Initialize TOKEN_SLASH
    ; Declaration: numeric TOKEN_LPAREN
    sub rsp, 8         ; Allocate space for TOKEN_LPAREN
    mov rax, 25
    mov [rbp-80], rax   ; Initialize TOKEN_LPAREN
    ; Declaration: numeric TOKEN_RPAREN
    sub rsp, 8         ; Allocate space for TOKEN_RPAREN
    mov rax, 26
    mov [rbp-88], rax   ; Initialize TOKEN_RPAREN
    ; Declaration: numeric TOKEN_COMMA
    sub rsp, 8         ; Allocate space for TOKEN_COMMA
    mov rax, 33
    mov [rbp-96], rax   ; Initialize TOKEN_COMMA
    ; Declaration: numeric source_len
    sub rsp, 8         ; Allocate space for source_len
    mov rax, [rbp-8]
    mov rdi, rax
    call mlp_string_length
    mov [rbp-104], rax   ; Initialize source_len
    ; Declaration: numeric pos
    sub rsp, 8         ; Allocate space for pos
    mov rax, 0
    mov [rbp-112], rax   ; Initialize pos
    ; Declaration: numeric[] token_types (dynamic array, pointer)
    sub rsp, 8         ; Allocate space for pointer token_types
    mov rax, 1000
    mov rdi, rax
    call mlp_array_alloc
    mov [rbp-120], rax   ; Store array pointer to token_types
    ; Declaration: text[] token_values (dynamic array, pointer)
    sub rsp, 8         ; Allocate space for pointer token_values
    mov rax, 1000
    mov rdi, rax
    call mlp_array_alloc
    mov [rbp-128], rax   ; Store array pointer to token_values
    ; Declaration: numeric token_count
    sub rsp, 8         ; Allocate space for token_count
    mov rax, 0
    mov [rbp-136], rax   ; Initialize token_count

    ; While loop
.L40:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-112]
    push rax
    mov rax, [rbp-104]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L41
    ; While body
    ; Declaration: text ch
    sub rsp, 8         ; Allocate space for ch
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, [rbp-112]
    mov rsi, rax
    call mlp_charAt
    mov [rbp-144], rax   ; Initialize ch
    ; Declaration: numeric is_ws
    sub rsp, 8         ; Allocate space for is_ws
    mov rax, [rbp-144]
    push rax
    call func_is_whitespace
    add rsp, 8
    mov [rbp-152], rax   ; Initialize is_ws

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

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L43
.L42:
    ; Else body
    ; Declaration: numeric is_d
    sub rsp, 8         ; Allocate space for is_d
    mov rax, [rbp-144]
    push rax
    call func_is_digit
    add rsp, 8
    mov [rbp-160], rax   ; Initialize is_d

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-160]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L44
    ; Then body
    ; Declaration: numeric start
    sub rsp, 8         ; Allocate space for start
    mov rax, [rbp-112]
    mov [rbp-168], rax   ; Initialize start
    ; Declaration: numeric done
    sub rsp, 8         ; Allocate space for done
    mov rax, 0
    mov [rbp-176], rax   ; Initialize done

    ; While loop
.L46:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-176]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L47
    ; While body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-112]
    push rax
    mov rax, [rbp-104]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L48
    ; Then body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-176], rax   ; Store to done
    jmp .L49
.L48:
    ; Else body
    ; Declaration: text numch
    sub rsp, 8         ; Allocate space for numch
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, [rbp-112]
    mov rsi, rax
    call mlp_charAt
    mov [rbp-184], rax   ; Initialize numch
    ; Declaration: numeric is_numchar
    sub rsp, 8         ; Allocate space for is_numchar
    mov rax, [rbp-184]
    push rax
    call func_is_digit
    add rsp, 8
    mov [rbp-192], rax   ; Initialize is_numchar

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-192]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L50
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L51
.L50:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-184]
    mov rdi, rax
    push rdi
    mov rax, str_18
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L52
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L53
.L52:
    ; Else body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-176], rax   ; Store to done
.L53:
.L51:
.L49:
    jmp .L46
.L47:  ; while_end
    ; Declaration: numeric num_len
    sub rsp, 8         ; Allocate space for num_len
    mov rax, [rbp-112]
    push rax
    mov rax, [rbp-168]
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-200], rax   ; Initialize num_len
    ; Declaration: text num_str
    sub rsp, 8         ; Allocate space for num_str
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, [rbp-168]
    mov rsi, rax
    mov rax, [rbp-200]
    mov rdx, rax
    call mlp_substring
    mov [rbp-208], rax   ; Initialize num_str

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, [rbp-24]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-120]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, [rbp-208]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-128]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-136]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-136], rax   ; Store to token_count
    jmp .L45
.L44:
    ; Else body
    ; Declaration: numeric is_a
    sub rsp, 8         ; Allocate space for is_a
    mov rax, [rbp-144]
    push rax
    call func_is_alpha
    add rsp, 8
    mov [rbp-216], rax   ; Initialize is_a

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-216]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L54
    ; Then body
    ; Declaration: numeric start
    sub rsp, 8         ; Allocate space for start
    mov rax, [rbp-112]
    mov [rbp-224], rax   ; Initialize start
    ; Declaration: numeric done
    sub rsp, 8         ; Allocate space for done
    mov rax, 0
    mov [rbp-232], rax   ; Initialize done

    ; While loop
.L56:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-232]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L57
    ; While body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-112]
    push rax
    mov rax, [rbp-104]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L58
    ; Then body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-232], rax   ; Store to done
    jmp .L59
.L58:
    ; Else body
    ; Declaration: text idch
    sub rsp, 8         ; Allocate space for idch
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, [rbp-112]
    mov rsi, rax
    call mlp_charAt
    mov [rbp-240], rax   ; Initialize idch
    ; Declaration: numeric is_idchar
    sub rsp, 8         ; Allocate space for is_idchar
    mov rax, [rbp-240]
    push rax
    call func_is_alnum
    add rsp, 8
    mov [rbp-248], rax   ; Initialize is_idchar

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-248]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L60
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L61
.L60:
    ; Else body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-232], rax   ; Store to done
.L61:
.L59:
    jmp .L56
.L57:  ; while_end
    ; Declaration: numeric id_len
    sub rsp, 8         ; Allocate space for id_len
    mov rax, [rbp-112]
    push rax
    mov rax, [rbp-224]
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-256], rax   ; Initialize id_len
    ; Declaration: text word
    sub rsp, 8         ; Allocate space for word
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, [rbp-224]
    mov rsi, rax
    mov rax, [rbp-256]
    mov rdx, rax
    call mlp_substring
    mov [rbp-264], rax   ; Initialize word
    ; Declaration: numeric tok_type
    sub rsp, 8         ; Allocate space for tok_type
    mov rax, [rbp-264]
    push rax
    call func_keyword_type
    add rsp, 8
    mov [rbp-272], rax   ; Initialize tok_type

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, [rbp-272]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-120]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, [rbp-264]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-128]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-136]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-136], rax   ; Store to token_count
    jmp .L55
.L54:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-144]
    mov rdi, rax
    push rdi
    mov rax, str_19
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L62
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, [rbp-40]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-120]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, str_20
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-128]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-136]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-136], rax   ; Store to token_count

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L63
.L62:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-144]
    mov rdi, rax
    push rdi
    mov rax, str_21
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L64
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, [rbp-48]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-120]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, str_22
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-128]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-136]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-136], rax   ; Store to token_count

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L65
.L64:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-144]
    mov rdi, rax
    push rdi
    mov rax, str_23
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L66
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, [rbp-56]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-120]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, str_24
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-128]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-136]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-136], rax   ; Store to token_count

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L67
.L66:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-144]
    mov rdi, rax
    push rdi
    mov rax, str_25
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L68
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, [rbp-64]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-120]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, str_26
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-128]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-136]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-136], rax   ; Store to token_count

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L69
.L68:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-144]
    mov rdi, rax
    push rdi
    mov rax, str_27
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L70
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, [rbp-72]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-120]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, str_28
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-128]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-136]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-136], rax   ; Store to token_count

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L71
.L70:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-144]
    mov rdi, rax
    push rdi
    mov rax, str_29
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L72
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, [rbp-80]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-120]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, str_30
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-128]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-136]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-136], rax   ; Store to token_count

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L73
.L72:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-144]
    mov rdi, rax
    push rdi
    mov rax, str_31
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L74
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, [rbp-88]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-120]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, str_32
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-128]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-136]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-136], rax   ; Store to token_count

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L75
.L74:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-144]
    mov rdi, rax
    push rdi
    mov rax, str_33
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L76
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, [rbp-96]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-120]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, str_34
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-128]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-136]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-136], rax   ; Store to token_count

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L77
.L76:
    ; Else body

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
.L77:
.L75:
.L73:
.L71:
.L69:
.L67:
.L65:
.L63:
.L55:
.L45:
.L43:
    jmp .L40
.L41:  ; while_end

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, [rbp-16]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-120]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-136]
    push rax            ; Save index
    mov rax, str_35
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-128]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-136]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-136], rax   ; Store to token_count
    mov rax, [rbp-136]
    mov rsp, rbp
    pop rbp
    ret

func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: text source_code
    sub rsp, 8         ; Allocate space for source_code
    mov rax, str_36
    mov rdi, rax
    call mlp_file_read
    mov [rbp-8], rax   ; Initialize source_code
    ; Declaration: numeric TOKEN_EOF
    sub rsp, 8         ; Allocate space for TOKEN_EOF
    mov rax, 0
    mov [rbp-16], rax   ; Initialize TOKEN_EOF
    ; Declaration: numeric TOKEN_NUMERIC_KW
    sub rsp, 8         ; Allocate space for TOKEN_NUMERIC_KW
    mov rax, 1
    mov [rbp-24], rax   ; Initialize TOKEN_NUMERIC_KW
    ; Declaration: numeric TOKEN_TEXT_KW
    sub rsp, 8         ; Allocate space for TOKEN_TEXT_KW
    mov rax, 2
    mov [rbp-32], rax   ; Initialize TOKEN_TEXT_KW
    ; Declaration: numeric TOKEN_NUMBER
    sub rsp, 8         ; Allocate space for TOKEN_NUMBER
    mov rax, 3
    mov [rbp-40], rax   ; Initialize TOKEN_NUMBER
    ; Declaration: numeric TOKEN_IDENTIFIER
    sub rsp, 8         ; Allocate space for TOKEN_IDENTIFIER
    mov rax, 5
    mov [rbp-48], rax   ; Initialize TOKEN_IDENTIFIER
    ; Declaration: numeric TOKEN_PRINT_KW
    sub rsp, 8         ; Allocate space for TOKEN_PRINT_KW
    mov rax, 6
    mov [rbp-56], rax   ; Initialize TOKEN_PRINT_KW
    ; Declaration: numeric TOKEN_ASSIGN
    sub rsp, 8         ; Allocate space for TOKEN_ASSIGN
    mov rax, 20
    mov [rbp-64], rax   ; Initialize TOKEN_ASSIGN
    ; Declaration: numeric TOKEN_LPAREN
    sub rsp, 8         ; Allocate space for TOKEN_LPAREN
    mov rax, 25
    mov [rbp-72], rax   ; Initialize TOKEN_LPAREN
    ; Declaration: numeric TOKEN_RPAREN
    sub rsp, 8         ; Allocate space for TOKEN_RPAREN
    mov rax, 26
    mov [rbp-80], rax   ; Initialize TOKEN_RPAREN
    ; Declaration: numeric[] token_types (dynamic array, pointer)
    sub rsp, 8         ; Allocate space for pointer token_types
    mov rax, 1000
    mov rdi, rax
    call mlp_array_alloc
    mov [rbp-88], rax   ; Store array pointer to token_types
    ; Declaration: text[] token_values (dynamic array, pointer)
    sub rsp, 8         ; Allocate space for pointer token_values
    mov rax, 1000
    mov rdi, rax
    call mlp_array_alloc
    mov [rbp-96], rax   ; Store array pointer to token_values
    ; Declaration: numeric source_len
    sub rsp, 8         ; Allocate space for source_len
    mov rax, [rbp-8]
    mov rdi, rax
    call mlp_string_length
    mov [rbp-104], rax   ; Initialize source_len
    ; Declaration: numeric pos
    sub rsp, 8         ; Allocate space for pos
    mov rax, 0
    mov [rbp-112], rax   ; Initialize pos
    ; Declaration: numeric token_count
    sub rsp, 8         ; Allocate space for token_count
    mov rax, 0
    mov [rbp-120], rax   ; Initialize token_count

    ; While loop
.L78:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-112]
    push rax
    mov rax, [rbp-104]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L79
    ; While body
    ; Declaration: text ch
    sub rsp, 8         ; Allocate space for ch
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, [rbp-112]
    mov rsi, rax
    call mlp_charAt
    mov [rbp-128], rax   ; Initialize ch
    ; Declaration: numeric is_ws
    sub rsp, 8         ; Allocate space for is_ws
    mov rax, [rbp-128]
    push rax
    call func_is_whitespace
    add rsp, 8
    mov [rbp-136], rax   ; Initialize is_ws

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-136]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L80
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L81
.L80:
    ; Else body
    ; Declaration: numeric is_d
    sub rsp, 8         ; Allocate space for is_d
    mov rax, [rbp-128]
    push rax
    call func_is_digit
    add rsp, 8
    mov [rbp-144], rax   ; Initialize is_d

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-144]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L82
    ; Then body
    ; Declaration: numeric start
    sub rsp, 8         ; Allocate space for start
    mov rax, [rbp-112]
    mov [rbp-152], rax   ; Initialize start
    ; Declaration: numeric done
    sub rsp, 8         ; Allocate space for done
    mov rax, 0
    mov [rbp-160], rax   ; Initialize done

    ; While loop
.L84:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-160]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L85
    ; While body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-112]
    push rax
    mov rax, [rbp-104]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L86
    ; Then body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-160], rax   ; Store to done
    jmp .L87
.L86:
    ; Else body
    ; Declaration: text numch
    sub rsp, 8         ; Allocate space for numch
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, [rbp-112]
    mov rsi, rax
    call mlp_charAt
    mov [rbp-168], rax   ; Initialize numch
    ; Declaration: numeric is_numchar
    sub rsp, 8         ; Allocate space for is_numchar
    mov rax, [rbp-168]
    push rax
    call func_is_digit
    add rsp, 8
    mov [rbp-176], rax   ; Initialize is_numchar

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-176]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L88
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L89
.L88:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-168]
    mov rdi, rax
    push rdi
    mov rax, str_37
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L90
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L91
.L90:
    ; Else body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-160], rax   ; Store to done
.L91:
.L89:
.L87:
    jmp .L84
.L85:  ; while_end
    ; Declaration: numeric num_len
    sub rsp, 8         ; Allocate space for num_len
    mov rax, [rbp-112]
    push rax
    mov rax, [rbp-152]
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-184], rax   ; Initialize num_len
    ; Declaration: text num_str
    sub rsp, 8         ; Allocate space for num_str
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, [rbp-152]
    mov rsi, rax
    mov rax, [rbp-184]
    mov rdx, rax
    call mlp_substring
    mov [rbp-192], rax   ; Initialize num_str

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-120]
    push rax            ; Save index
    mov rax, [rbp-40]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-88]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-120]
    push rax            ; Save index
    mov rax, [rbp-192]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-96]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-120]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-120], rax   ; Store to token_count
    jmp .L83
.L82:
    ; Else body
    ; Declaration: numeric is_a
    sub rsp, 8         ; Allocate space for is_a
    mov rax, [rbp-128]
    push rax
    call func_is_alpha
    add rsp, 8
    mov [rbp-200], rax   ; Initialize is_a

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-200]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L92
    ; Then body
    ; Declaration: numeric start
    sub rsp, 8         ; Allocate space for start
    mov rax, [rbp-112]
    mov [rbp-208], rax   ; Initialize start
    ; Declaration: numeric done
    sub rsp, 8         ; Allocate space for done
    mov rax, 0
    mov [rbp-216], rax   ; Initialize done

    ; While loop
.L94:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-216]
    push rax
    mov rax, 0
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L95
    ; While body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-112]
    push rax
    mov rax, [rbp-104]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L96
    ; Then body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-216], rax   ; Store to done
    jmp .L97
.L96:
    ; Else body
    ; Declaration: text idch
    sub rsp, 8         ; Allocate space for idch
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, [rbp-112]
    mov rsi, rax
    call mlp_charAt
    mov [rbp-224], rax   ; Initialize idch
    ; Declaration: numeric is_idchar
    sub rsp, 8         ; Allocate space for is_idchar
    mov rax, [rbp-224]
    push rax
    call func_is_alnum
    add rsp, 8
    mov [rbp-232], rax   ; Initialize is_idchar

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-232]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L98
    ; Then body

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L99
.L98:
    ; Else body

    ; Assignment: done = ...
    mov rax, 1
    mov [rbp-216], rax   ; Store to done
.L99:
.L97:
    jmp .L94
.L95:  ; while_end
    ; Declaration: numeric id_len
    sub rsp, 8         ; Allocate space for id_len
    mov rax, [rbp-112]
    push rax
    mov rax, [rbp-208]
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-240], rax   ; Initialize id_len
    ; Declaration: text word
    sub rsp, 8         ; Allocate space for word
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, [rbp-208]
    mov rsi, rax
    mov rax, [rbp-240]
    mov rdx, rax
    call mlp_substring
    mov [rbp-248], rax   ; Initialize word
    ; Declaration: numeric tok_type
    sub rsp, 8         ; Allocate space for tok_type
    mov rax, [rbp-248]
    push rax
    call func_keyword_type
    add rsp, 8
    mov [rbp-256], rax   ; Initialize tok_type

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-120]
    push rax            ; Save index
    mov rax, [rbp-256]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-88]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-120]
    push rax            ; Save index
    mov rax, [rbp-248]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-96]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-120]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-120], rax   ; Store to token_count
    jmp .L93
.L92:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-128]
    mov rdi, rax
    push rdi
    mov rax, str_38
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L100
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-120]
    push rax            ; Save index
    mov rax, [rbp-64]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-88]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-120]
    push rax            ; Save index
    mov rax, str_39
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-96]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-120]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-120], rax   ; Store to token_count

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L101
.L100:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-128]
    mov rdi, rax
    push rdi
    mov rax, str_40
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L102
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-120]
    push rax            ; Save index
    mov rax, [rbp-72]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-88]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-120]
    push rax            ; Save index
    mov rax, str_41
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-96]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-120]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-120], rax   ; Store to token_count

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L103
.L102:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-128]
    mov rdi, rax
    push rdi
    mov rax, str_42
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L104
    ; Then body

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-120]
    push rax            ; Save index
    mov rax, [rbp-80]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-88]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-120]
    push rax            ; Save index
    mov rax, str_43
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-96]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-120]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-120], rax   ; Store to token_count

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
    jmp .L105
.L104:
    ; Else body

    ; Assignment: pos = ...
    mov rax, [rbp-112]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-112], rax   ; Store to pos
.L105:
.L103:
.L101:
.L93:
.L83:
.L81:
    jmp .L78
.L79:  ; while_end

    ; Array assignment: token_types[...] = ...
    mov rax, [rbp-120]
    push rax            ; Save index
    mov rax, [rbp-16]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-88]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: token_values[...] = ...
    mov rax, [rbp-120]
    push rax            ; Save index
    mov rax, str_44
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-96]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Assignment: token_count = ...
    mov rax, [rbp-120]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-120], rax   ; Store to token_count

    ; Print statement
    mov rax, str_45
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_46
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-120]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_47
    mov rdi, rax
    call print_string
    ; Declaration: numeric i
    sub rsp, 8         ; Allocate space for i
    mov rax, 0
    mov [rbp-264], rax   ; Initialize i

    ; While loop
.L106:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-264]
    push rax
    mov rax, [rbp-120]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L107
    ; While body

    ; Print statement
    mov rax, str_48
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-264]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_49
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, [rbp-264]
    push rax            ; Save index
    mov rbx, [rbp-88]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_50
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, [rbp-264]
    push rax            ; Save index
    mov rbx, [rbp-96]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_51
    mov rdi, rax
    call print_string

    ; Assignment: i = ...
    mov rax, [rbp-264]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-264], rax   ; Store to i
    jmp .L106
.L107:  ; while_end
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
str_51: db " ", 0
str_50: db " value=", 0
str_49: db ": type=", 0
str_48: db "Token ", 0
str_47: db " ", 0
str_46: db "Token count: ", 0
str_45: db "=== LEXER RESULTS ===", 0
str_44: db "", 0
str_43: db ")", 0
str_42: db ")", 0
str_41: db "(", 0
str_40: db "(", 0
str_39: db "=", 0
str_38: db "=", 0
str_37: db ".", 0
str_36: db "compiler/test_simple.mlp", 0
str_35: db "", 0
str_34: db ",", 0
str_33: db ",", 0
str_32: db ")", 0
str_31: db ")", 0
str_30: db "(", 0
str_29: db "(", 0
str_28: db "/", 0
str_27: db "/", 0
str_26: db "*", 0
str_25: db "*", 0
str_24: db "-", 0
str_23: db "-", 0
str_22: db "+", 0
str_21: db "+", 0
str_20: db "=", 0
str_19: db "=", 0
str_18: db ".", 0
str_17: db "return", 0
str_16: db "func", 0
str_15: db "print", 0
str_14: db "text", 0
str_13: db "numeric", 0
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
