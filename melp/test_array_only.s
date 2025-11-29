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

    ; Print statement

    ; Array indexing
    mov rax, 0
    mov rbx, 8          ; Element size
    imul rax, rbx       ; index * 8
    mov rbx, 24         ; Base offset
    sub rbx, rax        ; base - (index * 8)
    neg rbx             ; -(base - index*8)
    mov rax, [rbp + rbx] ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement

    ; Array indexing
    mov rax, 1
    mov rbx, 8          ; Element size
    imul rax, rbx       ; index * 8
    mov rbx, 24         ; Base offset
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
    mov rbx, 24         ; Base offset
    sub rbx, rax        ; base - (index * 8)
    neg rbx             ; -(base - index*8)
    mov rax, [rbp + rbx] ; Load array[index]
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
