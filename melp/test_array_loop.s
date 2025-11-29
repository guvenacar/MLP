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
global _start


func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: numeric[] squares (array, size=5, total=40 bytes)
    sub rsp, 40         ; Allocate space for array squares
    mov rax, 0
    mov [rbp-40], rax   ; Initialize squares[0]
    mov rax, 0
    mov [rbp-32], rax   ; Initialize squares[1]
    mov rax, 0
    mov [rbp-24], rax   ; Initialize squares[2]
    mov rax, 0
    mov [rbp-16], rax   ; Initialize squares[3]
    mov rax, 0
    mov [rbp-8], rax   ; Initialize squares[4]

    ; For loop
    sub rsp, 8         ; Allocate i
    mov rax, 0
    mov [rbp-48], rax   ; Initialize i
.L0:  ; loop_start
    mov rax, [rbp-48]   ; Load i
    push rax
    mov rax, 4
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jg .L2           ; Exit if i > end
    ; Loop body

    ; Array assignment: squares[...] = ...
    mov rax, [rbp-48]
    push rax            ; Save index
    mov rax, [rbp-48]
    push rax
    mov rax, [rbp-48]
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 40         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]
.L1:  ; loop_continue
    ; Increment loop variable
    mov rax, [rbp-48]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-48], rax
    jmp .L0
.L2:  ; loop_end

    ; For loop
    sub rsp, 8         ; Allocate i
    mov rax, 0
    mov [rbp-56], rax   ; Initialize i
.L3:  ; loop_start
    mov rax, [rbp-56]   ; Load i
    push rax
    mov rax, 4
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jg .L5           ; Exit if i > end
    ; Loop body

    ; Print statement

    ; Array indexing
    mov rax, [rbp-56]
    mov rbx, 8          ; Element size
    imul rax, rbx       ; index * 8
    mov rbx, 40         ; Base offset
    sub rbx, rax        ; base - (index * 8)
    neg rbx             ; -(base - index*8)
    mov rax, [rbp + rbx] ; Load array[index]
    mov rdi, rax
    call print_number
.L4:  ; loop_continue
    ; Increment loop variable
    mov rax, [rbp-56]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-56], rax
    jmp .L3
.L5:  ; loop_end
    ; Declaration: numeric sum
    sub rsp, 8         ; Allocate space for sum
    mov rax, 0
    mov [rbp-64], rax   ; Initialize sum

    ; For loop
    sub rsp, 8         ; Allocate i
    mov rax, 0
    mov [rbp-72], rax   ; Initialize i
.L6:  ; loop_start
    mov rax, [rbp-72]   ; Load i
    push rax
    mov rax, 4
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jg .L8           ; Exit if i > end
    ; Loop body

    ; Assignment: sum = ...
    mov rax, [rbp-64]
    push rax

    ; Array indexing
    mov rax, [rbp-72]
    mov rbx, 8          ; Element size
    imul rax, rbx       ; index * 8
    mov rbx, 40         ; Base offset
    sub rbx, rax        ; base - (index * 8)
    neg rbx             ; -(base - index*8)
    mov rax, [rbp + rbx] ; Load array[index]
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-64], rax   ; Store to sum
.L7:  ; loop_continue
    ; Increment loop variable
    mov rax, [rbp-72]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-72], rax
    jmp .L6
.L8:  ; loop_end

    ; Print statement
    mov rax, [rbp-64]
    mov rdi, rax
    call print_number
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

_start:
    push rbp
    mov rbp, rsp

    call func_main

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
