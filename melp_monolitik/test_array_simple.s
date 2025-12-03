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


func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: numeric[] nums (array, size=5, total=40 bytes)
    sub rsp, 40         ; Allocate space for array nums
    mov rax, 10
    mov [rbp-40], rax   ; Initialize nums[0]
    mov rax, 20
    mov [rbp-32], rax   ; Initialize nums[1]
    mov rax, 30
    mov [rbp-24], rax   ; Initialize nums[2]
    mov rax, 40
    mov [rbp-16], rax   ; Initialize nums[3]
    mov rax, 50
    mov [rbp-8], rax   ; Initialize nums[4]

    ; Print statement

    ; Array indexing
    mov rax, 0
    mov rbx, 8          ; Element size
    imul rax, rbx       ; index * 8
    mov rbx, 40         ; Base offset
    sub rbx, rax        ; base - (index * 8)
    neg rbx             ; -(base - index*8)
    mov rax, [rbp + rbx] ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement

    ; Array indexing
    mov rax, 2
    mov rbx, 8          ; Element size
    imul rax, rbx       ; index * 8
    mov rbx, 40         ; Base offset
    sub rbx, rax        ; base - (index * 8)
    neg rbx             ; -(base - index*8)
    mov rax, [rbp + rbx] ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement

    ; Array indexing
    mov rax, 4
    mov rbx, 8          ; Element size
    imul rax, rbx       ; index * 8
    mov rbx, 40         ; Base offset
    sub rbx, rax        ; base - (index * 8)
    neg rbx             ; -(base - index*8)
    mov rax, [rbp + rbx] ; Load array[index]
    mov rdi, rax
    call print_number

    ; Array assignment: nums[...] = ...
    mov rax, 0
    push rax            ; Save index
    mov rax, 100
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 40         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]

    ; Array assignment: nums[...] = ...
    mov rax, 2
    push rax            ; Save index
    mov rax, 300
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 40         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]

    ; Array assignment: nums[...] = ...
    mov rax, 4
    push rax            ; Save index
    mov rax, 500
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 40         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]

    ; Print statement

    ; Array indexing
    mov rax, 0
    mov rbx, 8          ; Element size
    imul rax, rbx       ; index * 8
    mov rbx, 40         ; Base offset
    sub rbx, rax        ; base - (index * 8)
    neg rbx             ; -(base - index*8)
    mov rax, [rbp + rbx] ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement

    ; Array indexing
    mov rax, 2
    mov rbx, 8          ; Element size
    imul rax, rbx       ; index * 8
    mov rbx, 40         ; Base offset
    sub rbx, rax        ; base - (index * 8)
    neg rbx             ; -(base - index*8)
    mov rax, [rbp + rbx] ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement

    ; Array indexing
    mov rax, 4
    mov rbx, 8          ; Element size
    imul rax, rbx       ; index * 8
    mov rbx, 40         ; Base offset
    sub rbx, rax        ; base - (index * 8)
    neg rbx             ; -(base - index*8)
    mov rax, [rbp + rbx] ; Load array[index]
    mov rdi, rax
    call print_number
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
