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
extern mlp_to_numeric
extern mlp_to_text
extern mlp_to_decimal
extern mlp_decimal_to_text
extern mlp_parse_int
extern mlp_parse_float
extern mlp_input
extern mlp_input_prompt
extern mlp_sin
extern mlp_cos
extern mlp_tan
extern mlp_sqrt
extern mlp_pow
extern mlp_abs
extern mlp_abs_int
extern mlp_floor
extern mlp_ceil
extern mlp_round
extern mlp_log
extern mlp_log10
extern mlp_exp
extern mlp_fmod
extern mlp_pi
extern mlp_e
extern mlp_split
extern mlp_trim
extern mlp_replace
extern mlp_to_upper
extern mlp_to_lower
extern mlp_starts_with
extern mlp_ends_with
extern mlp_contains
extern mlp_assert
extern mlp_band
extern mlp_bor
extern mlp_bxor
extern mlp_bnot
extern mlp_shl
extern mlp_shr
extern mlp_ushr
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

    ; Declaration: numeric a
    sub rsp, 8         ; Allocate space for a
    mov rax, 12
    push rax
    mov rax, 10
    push rax
    pop rsi
    pop rdi
    call mlp_band
    mov [rbp-8], rax   ; Initialize a

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_number
    ; Declaration: numeric b
    sub rsp, 8         ; Allocate space for b
    mov rax, 12
    push rax
    mov rax, 10
    push rax
    pop rsi
    pop rdi
    call mlp_bor
    mov [rbp-16], rax   ; Initialize b

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_number
    ; Declaration: numeric c
    sub rsp, 8         ; Allocate space for c
    mov rax, 12
    push rax
    mov rax, 10
    push rax
    pop rsi
    pop rdi
    call mlp_bxor
    mov [rbp-24], rax   ; Initialize c

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_number
    ; Declaration: numeric d
    sub rsp, 8         ; Allocate space for d
    mov rax, 3
    push rax
    mov rax, 2
    push rax
    pop rsi
    pop rdi
    call mlp_shl
    mov [rbp-32], rax   ; Initialize d

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_number
    ; Declaration: numeric e
    sub rsp, 8         ; Allocate space for e
    mov rax, 12
    push rax
    mov rax, 2
    push rax
    pop rsi
    pop rdi
    call mlp_shr
    mov [rbp-40], rax   ; Initialize e

    ; Print statement
    mov rax, [rbp-40]
    mov rdi, rax
    call print_number
    ; Declaration: numeric val
    sub rsp, 8         ; Allocate space for val
    mov rax, 5
    mov [rbp-48], rax   ; Initialize val

    ; Assignment: val = ...
    mov rax, [rbp-48]
    push rax
    mov rax, 1
    push rax
    pop rsi
    pop rdi
    call mlp_shl
    mov [rbp-48], rax   ; Store to val

    ; Assignment: val = ...
    mov rax, [rbp-48]
    push rax
    mov rax, 1
    push rax
    pop rsi
    pop rdi
    call mlp_bor
    mov [rbp-48], rax   ; Store to val

    ; Print statement
    mov rax, [rbp-48]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_0
    mov rdi, rax
    call print_string

    ; Shutdown GC
    call gc_shutdown

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_0: db "Bitwise tests completed!", 0
