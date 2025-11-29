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


_start:
    push rbp
    mov rbp, rsp

    ; Declaration: Point p (struct, size=16 bytes)
    sub rsp, 16         ; Allocate space for struct p
    mov qword [rbp-16], 0   ; Initialize p.x
    mov qword [rbp-8], 0   ; Initialize p.y

    ; Field assignment: p.x = ...
    mov rax, 10
    mov [rbp-16], rax   ; Store to p.x

    ; Field assignment: p.y = ...
    mov rax, 20
    mov [rbp-8], rax   ; Store to p.y

    ; Print statement
    mov rax, [rbp-16]   ; Load p.x
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-8]   ; Load p.y
    mov rdi, rax
    call print_number

    ; Field assignment: p.x = ...
    mov rax, [rbp-16]   ; Load p.x
    push rax
    mov rax, 5
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-16], rax   ; Store to p.x

    ; Print statement
    mov rax, [rbp-16]   ; Load p.x
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
