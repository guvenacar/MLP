section .data

section .bss
    var_x resq 1 ; INT64
    var_y resq 1 ; INT64

section .text
    global _start

; Print integer in rax to stdout
print_int:
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    mov rsi, rsp
    sub rsp, 32          ; Buffer for digits
    mov rdi, rsp
    add rdi, 31          ; End of buffer
    mov byte [rdi], 10   ; Newline
    dec rdi
    mov rbx, 10
    test rax, rax
    jns .print_positive
    neg rax
    mov rcx, 1           ; Negative flag
    jmp .print_loop
.print_positive:
    xor rcx, rcx         ; Positive
.print_loop:
    xor rdx, rdx
    div rbx
    add dl, '0'
    mov [rdi], dl
    dec rdi
    test rax, rax
    jnz .print_loop
    test rcx, rcx
    jz .print_write
    mov byte [rdi], '-'
    dec rdi
.print_write:
    inc rdi
    mov rax, 1           ; sys_write
    mov rsi, rdi
    lea rdx, [rsp + 32]
    sub rdx, rdi
    mov rdi, 1           ; stdout
    syscall
    add rsp, 32
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    ret

; Print null-terminated string (pointer in rax) to stdout
print_string:
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    mov rsi, rax         ; String pointer
    mov rdi, rsi         ; Save start
    ; Calculate string length
    xor rcx, rcx
.strlen_loop:
    mov al, [rsi + rcx]
    test al, al
    jz .strlen_done
    inc rcx
    jmp .strlen_loop
.strlen_done:
    ; rcx = length, rdi = string pointer
    mov rdx, rcx         ; Length
    mov rsi, rdi         ; String pointer
    mov rax, 1           ; sys_write
    mov rdi, 1           ; stdout
    syscall
    ; Print newline
    push 10              ; Newline character
    mov rax, 1
    mov rdi, 1
    mov rsi, rsp
    mov rdx, 1
    syscall
    add rsp, 8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    ret

; String length: rax = pointer to string, returns length in rax
str_length:
    push rcx
    push rsi
    mov rsi, rax         ; String pointer
    xor rcx, rcx         ; Counter = 0
.str_len_loop:
    mov al, [rsi + rcx]
    test al, al
    jz .str_len_done
    inc rcx
    jmp .str_len_loop
.str_len_done:
    mov rax, rcx         ; Return length
    pop rsi
    pop rcx
    ret

; Char at index: rdi = string pointer, rsi = index, returns char in rax
str_char_at:
    xor rax, rax
    mov al, [rdi + rsi]  ; Get char at index
    ret

; String concat: rdi = str1, rsi = str2, returns new string in rax
str_concat:
    push rbx
    push rcx
    push rdx
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    mov r12, rdi         ; Save str1
    mov r13, rsi         ; Save str2
    mov rax, rdi
    call str_length
    mov r8, rax          ; len1
    mov rax, r13
    call str_length
    mov r9, rax          ; len2
    mov r10, r8
    add r10, r9
    inc r10              ; +1 for null
    mov rax, 9           ; mmap syscall
    xor rdi, rdi         ; addr = NULL
    mov rsi, r10         ; length
    mov rdx, 3           ; PROT_READ | PROT_WRITE
    mov r10, 0x22        ; MAP_PRIVATE | MAP_ANONYMOUS
    mov r8, -1           ; fd = -1
    xor r9, r9           ; offset = 0
    syscall
    mov r11, rax         ; Save new buffer address
    mov rax, r12
    call str_length
    mov r8, rax          ; len1 again
    mov rax, r13
    call str_length
    mov r9, rax          ; len2 again
    xor rcx, rcx         ; index = 0
.concat_copy1:
    cmp rcx, r8
    jge .concat_copy2_start
    mov al, [r12 + rcx]
    mov [r11 + rcx], al
    inc rcx
    jmp .concat_copy1
.concat_copy2_start:
    xor rdx, rdx         ; index2 = 0
.concat_copy2:
    cmp rdx, r9
    jge .concat_done
    mov al, [r13 + rdx]
    mov [r11 + rcx], al
    inc rcx
    inc rdx
    jmp .concat_copy2
.concat_done:
    mov byte [r11 + rcx], 0
    mov rax, r11         ; Return new string
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdx
    pop rcx
    pop rbx
    ret

; String substr: rdi = str, rsi = start, rdx = len, returns new string in rax
str_substr:
    push rbx
    push rcx
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    mov r12, rdi         ; Save source string
    mov r13, rsi         ; Save start index
    mov r14, rdx         ; Save length
    mov rax, 9           ; mmap syscall
    xor rdi, rdi         ; addr = NULL
    mov rsi, r14
    inc rsi              ; length + 1
    mov rdx, 3           ; PROT_READ | PROT_WRITE
    mov r10, 0x22        ; MAP_PRIVATE | MAP_ANONYMOUS
    mov r8, -1           ; fd = -1
    xor r9, r9           ; offset = 0
    syscall
    mov r11, rax         ; Save new buffer address
    xor rcx, rcx         ; dest index = 0
.substr_copy:
    cmp rcx, r14
    jge .substr_done
    mov rax, r13         ; start
    add rax, rcx         ; start + i
    mov al, [r12 + rax]  ; src[start + i]
    mov [r11 + rcx], al  ; dest[i] = src[start + i]
    inc rcx
    jmp .substr_copy
.substr_done:
    mov byte [r11 + rcx], 0
    mov rax, r11         ; Return new string
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rcx
    pop rbx
    ret

_start:
    ; x = 42 (INT64)
    mov rax, 42
    mov [var_x], rax
    ; y = 10 (INT64)
    mov rax, 10
    mov [var_y], rax
    ; print numeric variable
    mov rax, [var_x]
    call print_int
    ; print numeric variable
    mov rax, [var_y]
    call print_int

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
