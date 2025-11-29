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
    mov rax, 5
    mov rdi, rax
    call mlp_array_alloc
    mov [rbp-8], rax   ; Store array pointer to arr

    ; Array assignment: arr[...] = ...
    mov rax, 0
    push rax            ; Save index
    mov rax, 100
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-8]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: arr[...] = ...
    mov rax, 1
    push rax            ; Save index
    mov rax, 200
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-8]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: arr[...] = ...
    mov rax, 2
    push rax            ; Save index
    mov rax, 300
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-8]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: arr[...] = ...
    mov rax, 3
    push rax            ; Save index
    mov rax, 400
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-8]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Array assignment: arr[...] = ...
    mov rax, 4
    push rax            ; Save index
    mov rax, 500
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    push rbx            ; Save value
    mov rcx, [rbp-8]   ; Load array pointer
    pop rbx             ; Restore value
    mov rdx, 8          ; Element size
    imul rax, rdx       ; index * 8
    add rcx, rax        ; ptr + (index * 8)
    mov [rcx], rbx      ; Store value to array[index]

    ; Print statement

    ; Array indexing
    mov rax, 0
    push rax            ; Save index
    mov rbx, [rbp-8]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement

    ; Array indexing
    mov rax, 2
    push rax            ; Save index
    mov rbx, [rbp-8]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement

    ; Array indexing
    mov rax, 4
    push rax            ; Save index
    mov rbx, [rbp-8]   ; Load array pointer
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    add rbx, rax        ; ptr + (index * 8)
    mov rax, [rbx]      ; Load array[index]
    mov rdi, rax
    call print_number
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
