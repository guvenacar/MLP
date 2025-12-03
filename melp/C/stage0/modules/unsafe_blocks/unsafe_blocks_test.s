section .data
    ; Unsafe module data section
    warning_msg db 'UNSAFE: Undefined behavior possible', 10, 0

section .bss
    ptr resq 1    ; const pointer (i32)
    mut_ptr resq 1    ; mut pointer (i32)

section .text
    global main
    extern malloc, free, printf

; Unsafe context: dangerous_operation (type: 1)
    ; Unsafe function: dangerous_operation
dangerous_operation:
    push rbp
    mov rbp, rsp

main:
    push rbp
    mov rbp, rsp

; Unsafe context: unsafe_block (type: 0)
    ; Begin unsafe block
    ; Safety checks disabled within this scope

; Unsafe operation: 0
; Code: let x = *raw_ptr
    ; Dereference raw pointer: raw_ptr
    mov rax, [raw_ptr]        ; load raw pointer
    mov rax, [rax]       ; dereference (UNSAFE)

; Unsafe operation: 7
; Code: std::mem::transmute::<u32, i32>(x)
    ; Type transmutation (UNSAFE)
    ; Reinterpret bits as different type
    mov rax, [source]    ; load source value
    ; Now treat as target type

    ; End unsafe operations
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

; End dangerous_operation
    mov rsp, rbp
    pop rbp
    ret
