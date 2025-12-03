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
extern malloc
extern free
extern mlp_array_alloc
extern mlp_array_free
extern mlp_array_length
extern mlp_array_resize
extern mlp_file_read
extern mlp_file_write
extern mlp_file_exists
extern mlp_file_append
extern mlp_substring
extern mlp_indexOf
extern mlp_charAt
extern mlp_string_length
extern mlp_get_argv
extern mlp_get_argc
extern setjmp
extern strcmp
extern mlp_exception_push
extern mlp_exception_pop
extern mlp_throw
extern mlp_exception_type
extern mlp_exception_message
extern mlp_exception_code
extern mlp_exception_has_handler
extern mlp_exception_has_parent_handler
global _start


global func_main
func_main:
    push rbp
    mov rbp, rsp
    ; Declaration: numeric[] arr (array, size=3, total=24 bytes)
    sub rsp, 24         ; Allocate space for array arr
    mov qword [rbp-24], 0   ; Initialize arr[0]
    mov qword [rbp-16], 0   ; Initialize arr[1]
    mov qword [rbp-8], 0   ; Initialize arr[2]

    ; Array assignment: arr[...] = ...
    mov rax, 0
    push rax            ; Save index
    mov rax, 10
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 24         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]

    ; Array assignment: arr[...] = ...
    mov rax, 1
    push rax            ; Save index
    mov rax, 20
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 24         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]

    ; Array assignment: arr[...] = ...
    mov rax, 2
    push rax            ; Save index
    mov rax, 30
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 24         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]
    lea rax, [rbp-24]   ; Load address of arr
    sub rsp, 8
    push rax   ; Save array pointer
    mov rbx, [rax+0]   ; Load array[0]
    mov [rbp-32], rbx   ; Store to a
    pop rax   ; Restore array pointer
    sub rsp, 8
    push rax   ; Save array pointer
    mov rbx, [rax+8]   ; Load array[1]
    mov [rbp-40], rbx   ; Store to b
    pop rax   ; Restore array pointer
    sub rsp, 8
    push rax   ; Save array pointer
    mov rbx, [rax+16]   ; Load array[2]
    mov [rbp-48], rbx   ; Store to c
    pop rax   ; Restore array pointer

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-40]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-48]
    mov rdi, rax
    call print_number
    ; Declaration: numeric[] arr2 (array, size=5, total=40 bytes)
    sub rsp, 40         ; Allocate space for array arr2
    mov qword [rbp-88], 0   ; Initialize arr2[0]
    mov qword [rbp-80], 0   ; Initialize arr2[1]
    mov qword [rbp-72], 0   ; Initialize arr2[2]
    mov qword [rbp-64], 0   ; Initialize arr2[3]
    mov qword [rbp-56], 0   ; Initialize arr2[4]

    ; Array assignment: arr2[...] = ...
    mov rax, 0
    push rax            ; Save index
    mov rax, 100
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 88         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]

    ; Array assignment: arr2[...] = ...
    mov rax, 1
    push rax            ; Save index
    mov rax, 200
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 88         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]

    ; Array assignment: arr2[...] = ...
    mov rax, 2
    push rax            ; Save index
    mov rax, 300
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 88         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]

    ; Array assignment: arr2[...] = ...
    mov rax, 3
    push rax            ; Save index
    mov rax, 400
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 88         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]

    ; Array assignment: arr2[...] = ...
    mov rax, 4
    push rax            ; Save index
    mov rax, 500
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 88         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]
    lea rax, [rbp-88]   ; Load address of arr2
    sub rsp, 8
    push rax   ; Save array pointer
    mov rbx, [rax+0]   ; Load array[0]
    mov [rbp-96], rbx   ; Store to x
    pop rax   ; Restore array pointer
    sub rsp, 8
    push rax   ; Save array pointer
    mov rbx, [rax+8]   ; Load array[1]
    mov [rbp-104], rbx   ; Store to y
    pop rax   ; Restore array pointer

    ; Print statement
    mov rax, [rbp-96]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, [rbp-104]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

_start:
    ; Get argc and argv from stack
    pop rdi               ; argc (first item on stack)
    mov rsi, rsp          ; argv (pointer to argv[0])
    call mlp_get_argv     ; Convert to MLP string array

    push rbp
    mov rbp, rsp

    ; Call main function
    call func_main

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
