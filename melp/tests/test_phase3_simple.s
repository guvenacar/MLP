section .text
extern print_number
global _start

_start:
    push rbp
    mov rbp, rsp

    ; Declaration: numeric sum
    sub rsp, 8         ; Allocate space for sum
    mov qword [rbp-8], 0   ; Initialize sum

    ; For loop
    sub rsp, 8         ; Allocate i
    mov rax, 0
    mov [rbp-16], rax   ; Initialize i
.L0:  ; loop_start
    mov rax, [rbp-16]   ; Load i
    push rax
    mov rax, 5
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jg .L1           ; Exit if i > end
    ; Loop body

    ; Assignment: sum = ...
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-8], rax   ; Store to sum

    ; Print statement
    mov rdi, [rbp-8]   ; Load sum
    call print_number
    ; Increment loop variable
    mov rax, [rbp-16]
    add rax, 1
    mov [rbp-16], rax
    jmp .L0
.L1:  ; loop_end

    ; Print statement
    mov rdi, 999       ; Immediate value
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
