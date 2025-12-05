section .data
    msg: db "Generator OK!", 10, 0
    msg_len: equ $ - msg

section .bss
    var_range: resq 1
    gen_range_state: resq 1    ; Generator state
    gen_range_value: resq 1    ; Current yield value
    var_iter: resq 1
    var_val: resq 1

section .text
    global _start

; Generator: range (yields: 0)
gen_range:
    push rbp
    mov rbp, rsp
    ; State machine based on gen_range_state
    mov rax, [gen_range_state]
    cmp rax, 0    ; Check if finished
    jge .gen_range_done
    ; Yield current value
    inc qword [gen_range_state]
    mov rax, [gen_range_value]
    pop rbp
    ret
.gen_range_done:
    xor rax, rax    ; Return 0 when done
    pop rbp
    ret

_start:
    push rbp
    mov rbp, rsp
    sub rsp, 64

    ; range: generator [GENERATOR yields=0]
    mov qword [gen_range_state], 0
    mov qword [gen_range_value], 0

    ; iter: iterator [ITERATOR]
    mov qword [var_iter], 0    ; Iterator ref

    ; val: numeric
    mov qword [var_val], 0

    ; Print result
    mov rax, 1
    mov rdi, 1
    lea rsi, [msg]
    mov rdx, msg_len
    syscall

    ; Exit
    mov rsp, rbp
    pop rbp
    mov rax, 60
    xor rdi, rdi
    syscall
