; MLP Ownership System - Generated Assembly
; Module #55: Ownership, Borrow Checker, Lifetimes

section .data
    ownership_violation_msg db "OWNERSHIP VIOLATION: ", 0
    use_after_move_msg db "Use after move detected", 10, 0
    double_borrow_msg db "Multiple mutable borrows", 10, 0
    lifetime_msg db "Lifetime violation", 10, 0
    borrow_ok_msg db "Borrow check: OK", 10, 0
    move_ok_msg db "Move: OK", 10, 0

section .bss
    x resq 1          ; numeric (owned)
    x_moved resb 1     ; move flag
    x_borrow_count resq 1  ; active borrows
    y resq 1          ; numeric (owned)
    y_moved resb 1     ; move flag
    y_borrow_count resq 1  ; active borrows
    message resq 1          ; text (owned)
    message_moved resb 1     ; move flag
    message_borrow_count resq 1  ; active borrows
    x resq 1          ; borrow (owned)
    x_moved resb 1     ; move flag
    x_borrow_count resq 1  ; active borrows
    x resq 1          ; borrow (owned)
    x_moved resb 1     ; move flag
    x_borrow_count resq 1  ; active borrows
    y resq 1          ; borrow_mut (owned)
    y_moved resb 1     ; move flag
    y_borrow_count resq 1  ; active borrows
    x resq 1          ; move (owned)
    x_moved resb 1     ; move flag
    x_borrow_count resq 1  ; active borrows
    y resq 1          ; copy (owned)
    y_moved resb 1     ; move flag
    y_borrow_count resq 1  ; active borrows
    message resq 1          ; clone (owned)
    message_moved resb 1     ; move flag
    message_borrow_count resq 1  ; active borrows

section .text
    global _start
    extern printf

; Runtime ownership check functions
check_use_after_move:
    ; rdi = variable address
    ; rsi = moved flag address
    mov al, [rsi]
    test al, al
    jz .ok
    ; Report violation
    lea rdi, [rel use_after_move_msg]
    call print_error
    mov rax, 60
    mov rdi, 1
    syscall
.ok:
    ret

check_borrow_conflict:
    ; rdi = variable borrow_count address
    ; rsi = 1 for mutable, 0 for immutable
    mov rax, [rdi]
    test rsi, rsi
    jz .immutable_borrow
.mutable_borrow:
    ; Check if already borrowed
    test rax, rax
    jnz .conflict
    ; Mark as exclusively borrowed
    mov qword [rdi], 0xFFFFFFFFFFFFFFFF
    jmp .ok
.immutable_borrow:
    ; Check if mutably borrowed
    cmp rax, 0xFFFFFFFFFFFFFFFF
    je .conflict
    ; Increment shared borrow count
    inc qword [rdi]
    jmp .ok
.conflict:
    lea rdi, [rel double_borrow_msg]
    call print_error
    mov rax, 60
    mov rdi, 1
    syscall
.ok:
    ret

print_error:
    ; rdi = message address
    mov rsi, rdi
    mov rdi, 1          ; stdout
    mov rdx, 50         ; max length
    mov rax, 1          ; sys_write
    syscall
    ret

_start:
    ; Initialize ownership system

    ; Owner: x : numeric
    mov qword [rel x], 0
    mov byte [rel x_moved], 0
    mov qword [rel x_borrow_count], 0

    ; Owner: y : numeric
    mov qword [rel y], 0
    mov byte [rel y_moved], 0
    mov qword [rel y_borrow_count], 0

    ; Owner: message : text
    mov qword [rel message], 0
    mov byte [rel message_moved], 0
    mov qword [rel message_borrow_count], 0

    ; Owner: x : borrow
    mov qword [rel x], 0
    mov byte [rel x_moved], 0
    mov qword [rel x_borrow_count], 0

    ; Owner: x : borrow
    mov qword [rel x], 0
    mov byte [rel x_moved], 0
    mov qword [rel x_borrow_count], 0

    ; Owner: y : borrow_mut
    mov qword [rel y], 0
    mov byte [rel y_moved], 0
    mov qword [rel y_borrow_count], 0

    ; Owner: x : move
    mov qword [rel x], 0
    mov byte [rel x_moved], 0
    mov qword [rel x_borrow_count], 0

    ; Owner: y : copy
    mov qword [rel y], 0
    mov byte [rel y_moved], 0
    mov qword [rel y_borrow_count], 0

    ; Owner: message : clone
    mov qword [rel message], 0
    mov byte [rel message_moved], 0
    mov qword [rel message_borrow_count], 0

    ; Lifetime 'a' (type: named, scope: 1--1)
    ; Named lifetime - scope tracked at compile time

    ; Lifetime 'static' (type: static, scope: 27--1)
    ; Static lifetime - no runtime check needed

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
