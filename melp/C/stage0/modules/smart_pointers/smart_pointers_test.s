section .data
    ; Smart pointer data section

section .bss
    box_ptr resq 1    ; i32
    rc_ptr resq 1    ; String
    arc_ptr resq 1    ; Vec

section .text
    global main
    extern malloc, free, printf

main:
    push rbp
    mov rbp, rsp

; Smart pointer declaration: box_ptr (type: 0)
    ; Box<i32> - unique ownership
    mov rdi, 8       ; size for pointer
    call malloc      ; allocate heap memory
    mov [box_ptr], rax    ; store pointer

; Smart pointer declaration: rc_ptr (type: 1)
    ; Rc<String> - reference counted
    mov rdi, 16      ; size for pointer + refcount
    call malloc
    mov qword [rax], 1   ; initialize refcount to 1
    mov [rc_ptr], rax    ; store pointer

; Smart pointer declaration: arc_ptr (type: 2)
    ; Arc<Vec> - atomic reference counted
    mov rdi, 16      ; size for pointer + atomic refcount
    call malloc
    mov qword [rax], 1   ; initialize refcount to 1
    mov [arc_ptr], rax    ; store pointer

; Smart pointer operation on: rc_ptr (op: 3)
    ; Clone reference counted pointer
    mov rax, [rc_ptr]    ; load pointer
    inc qword [rax]  ; increment refcount
    ; Return cloned pointer in rax

    ; Clean exit
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret
