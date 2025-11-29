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
global _start


; Async function: fetchData (compiled as sync for now)
func_fetchData:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param id
    sub rsp, 8
    mov [rbp-8], rax   ; Store param id locally
    mov rax, [rbp-8]
    push rax
    mov rax, 100
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov rsp, rbp
    pop rbp
    ret

; Async function: processData (compiled as sync for now)
func_processData:
    push rbp
    mov rbp, rsp
    ; Declaration: numeric result
    sub rsp, 8         ; Allocate space for result
    ; Await expression (compiled as sync call for now)
    mov rax, 5
    push rax
    call func_fetchData
    add rsp, 8
    mov [rbp-8], rax   ; Initialize result

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_number
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


    ; Print statement
    mov rax, 42
    mov rdi, rax
    call print_number

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
