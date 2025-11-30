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
extern mlp_range
extern mlp_range1
extern mlp_range2
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
extern gc_init
extern gc_alloc
extern gc_free
extern gc_retain
extern gc_release
extern gc_collect
extern gc_full_collect
extern gc_shutdown
extern gc_get_object_count
extern gc_get_total_bytes
extern gc_get_collections
extern gc_enable
extern gc_disable
global _start


_start:
    ; Get argc and argv from stack
    pop rdi               ; argc (first item on stack)
    mov rsi, rsp          ; argv (pointer to argv[0])
    call mlp_get_argv     ; Convert to MLP string array

    ; Initialize GC
    call gc_init

    push rbp
    mov rbp, rsp


    ; Print statement
    mov rax, str_0
    mov rdi, rax
    call print_string
    call gc_collect

    ; Print statement
    mov rax, str_1
    mov rdi, rax
    call print_string
    ; Declaration: numeric count
    sub rsp, 8         ; Allocate space for count
    call gc_get_object_count
    mov [rbp-8], rax   ; Initialize count

    ; Print statement
    mov rax, str_2
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_number
    ; Declaration: numeric bytes
    sub rsp, 8         ; Allocate space for bytes
    call gc_get_total_bytes
    mov [rbp-16], rax   ; Initialize bytes

    ; Print statement
    mov rax, str_3
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_number
    ; Declaration: numeric cols
    sub rsp, 8         ; Allocate space for cols
    call gc_get_collections
    mov [rbp-24], rax   ; Initialize cols

    ; Print statement
    mov rax, str_4
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_5
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number

    ; Shutdown GC
    call gc_shutdown

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_5: db "=== GC Test Complete ===", 0
str_4: db "Collections:", 0
str_3: db "Total bytes:", 0
str_2: db "Object count:", 0
str_1: db "gc_collect() called", 0
str_0: db "=== GC Basic Test ===", 0
