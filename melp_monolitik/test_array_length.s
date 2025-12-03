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
extern mlp_array_alloc
extern mlp_array_free
extern mlp_array_length
extern mlp_array_resize
global _start


func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: numeric[] arr (dynamic array, pointer)
    sub rsp, 8         ; Allocate space for pointer arr
    mov rax, 10
    mov rdi, rax
    call mlp_array_alloc
    mov [rbp-8], rax   ; Store array pointer to arr

    ; For loop
    sub rsp, 8         ; Allocate i
    mov rax, 0
    mov [rbp-16], rax   ; Initialize i
.L0:  ; loop_start
    mov rax, [rbp-16]   ; Load i
    push rax
    mov rax, 9
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jg .L2           ; Exit if i > end
    ; Loop body

    ; Array assignment: arr[...] = ...
    mov rax, [rbp-16]
    push rax            ; Save index
    mov rax, [rbp-16]
    push rax
    mov rax, 10
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-8]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]
.L1:  ; loop_continue
    ; Increment loop variable
    mov rax, [rbp-16]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-16], rax
    jmp .L0
.L2:  ; loop_end

    ; For loop
    sub rsp, 8         ; Allocate i
    mov rax, 0
    mov [rbp-24], rax   ; Initialize i
.L3:  ; loop_start
    mov rax, [rbp-24]   ; Load i
    push rax
    mov rax, 9
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jg .L5           ; Exit if i > end
    ; Loop body

    ; Print statement

    ; Array indexing
    mov rax, [rbp-24]
    push rax            ; Save index
    mov rbx, [rbp-8]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov rdi, rax
    call print_number
.L4:  ; loop_continue
    ; Increment loop variable
    mov rax, [rbp-24]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-24], rax
    jmp .L3
.L5:  ; loop_end
    mov rax, [rbp-8]
    mov rdi, rax
    call mlp_array_free
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
