section .text
global _start

_start:
    push rbp
    mov rbp, rsp

    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    ; Declaration: decimal y
    sub rsp, 8         ; Allocate space for y
    ; Declaration: boolean flag
    sub rsp, 8         ; Allocate space for flag

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
