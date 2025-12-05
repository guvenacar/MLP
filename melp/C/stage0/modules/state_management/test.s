; State Management Module
section .data
    _st_msg: db 'State Management OK!',10,0
    _state_management: dq 0
    _state_sayi: dq 0
    _state_counter: dq 0
    _state_sayi: dq 0
    _state_MAX: dq 0
    _state_metin: dq 0
    _state_name: dq 0
section .text
    global _start
; Getters/Setters
_get_management:
    lea rax, [_state_management]
    mov rax, [rax]
    ret
_set_management:
    lea rax, [_state_management]
    mov [rax], rdi
    ret
_get_sayi:
    lea rax, [_state_sayi]
    mov rax, [rax]
    ret
_set_sayi:
    lea rax, [_state_sayi]
    mov [rax], rdi
    ret
_get_counter:
    lea rax, [_state_counter]
    mov rax, [rax]
    ret
_set_counter:
    lea rax, [_state_counter]
    mov [rax], rdi
    ret
_get_sayi:
    lea rax, [_state_sayi]
    mov rax, [rax]
    ret
_set_sayi:
    lea rax, [_state_sayi]
    mov [rax], rdi
    ret
_get_MAX:
    lea rax, [_state_MAX]
    mov rax, [rax]
    ret
_set_MAX:
    lea rax, [_state_MAX]
    mov [rax], rdi
    ret
_get_metin:
    lea rax, [_state_metin]
    mov rax, [rax]
    ret
_set_metin:
    lea rax, [_state_metin]
    mov [rax], rdi
    ret
_get_name:
    lea rax, [_state_name]
    mov rax, [rax]
    ret
_set_name:
    lea rax, [_state_name]
    mov [rax], rdi
    ret
_start:
    mov rax, 1
    mov rdi, 1
    lea rsi, [_st_msg]
    mov rdx, 21
    syscall
    mov rax, 60
    xor rdi, rdi
    syscall
