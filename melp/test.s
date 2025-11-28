section .text
extern print_number
global _start

_start:
    push rbp
    mov rbp, rsp

    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov qword [rbp-8], 0   ; Initialize x

    ; While loop
.L0:  ; while_start

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, 5
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L1
    ; While body

    ; Assignment: x = ...
    mov rax, [rbp-8]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-8], rax   ; Store to x

    ; Print statement
    mov rdi, [rbp-8]   ; Load x
    call print_number
    jmp .L0
.L1:  ; while_end
    ; Declaration: numeric sum
    sub rsp, 8         ; Allocate space for sum
    mov qword [rbp-16], 0   ; Initialize sum

    ; For loop
    sub rsp, 8         ; Allocate i
    mov rax, 0
    mov [rbp-24], rax   ; Initialize i
.L2:  ; loop_start
    mov rax, [rbp-24]   ; Load i
    push rax
    mov rax, 10
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jg .L4           ; Exit if i > end
    ; Loop body

    ; Assignment: sum = ...
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-24]
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-16], rax   ; Store to sum

    ; Print statement
    mov rdi, [rbp-16]   ; Load sum
    call print_number
.L3:  ; loop_continue
    ; Increment loop variable
    mov rax, [rbp-24]
    push rax
    mov rax, 2
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-24], rax
    jmp .L2
.L4:  ; loop_end

    ; For loop
    sub rsp, 8         ; Allocate i
    mov rax, 0
    mov [rbp-32], rax   ; Initialize i
.L5:  ; loop_start
    mov rax, [rbp-32]   ; Load i
    push rax
    mov rax, 20
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jg .L7           ; Exit if i > end
    ; Loop body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-32]
    push rax
    mov rax, 8
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L8
    ; Then body
    jmp .L7           ; exit
.L8:

    ; Print statement
    mov rdi, [rbp-32]   ; Load i
    call print_number
.L6:  ; loop_continue
    ; Increment loop variable
    mov rax, [rbp-32]
    add rax, 1
    mov [rbp-32], rax
    jmp .L5
.L7:  ; loop_end

    ; Print statement
    mov rdi, 999       ; Immediate value
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
