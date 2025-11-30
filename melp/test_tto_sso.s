section .data
    ; String literals will be added here

section .text
extern print_number
extern print_double
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


    ; Print statement
    mov rax, str_0
    mov rdi, rax
    call print_string
    ; Declaration: text short1 (TTO:SSO)
    sub rsp, 8         ; Allocate space for short1
    mov rax, str_1
    mov [rbp-8], rax   ; Initialize short1

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_string
    ; Declaration: text short2 (TTO:SSO)
    sub rsp, 8         ; Allocate space for short2
    mov rax, str_2
    mov [rbp-16], rax   ; Initialize short2

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_string
    ; Declaration: text short3 (TTO:SSO)
    sub rsp, 8         ; Allocate space for short3
    mov rax, str_3
    mov [rbp-24], rax   ; Initialize short3

    ; Print statement
    mov rax, [rbp-24]
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_4
    mov rdi, rax
    call print_string
    ; Declaration: text long1
    sub rsp, 8         ; Allocate space for long1
    mov rax, str_5
    mov [rbp-32], rax   ; Initialize long1

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_string
    ; Declaration: text long2
    sub rsp, 8         ; Allocate space for long2
    mov rax, str_6
    mov [rbp-40], rax   ; Initialize long2

    ; Print statement
    mov rax, [rbp-40]
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_7
    mov rdi, rax
    call print_string
    ; Declaration: text name (TTO:SSO)
    sub rsp, 8         ; Allocate space for name
    mov rax, str_8
    mov [rbp-48], rax   ; Initialize name
    ; Declaration: text greeting
    sub rsp, 8         ; Allocate space for greeting
    mov rax, str_9
    push rax
    mov rax, [rbp-48]
    mov rsi, rax
    pop rdi
    call string_concat
    push rax
    mov rax, str_10
    mov rsi, rax
    pop rdi
    call string_concat
    mov [rbp-56], rax   ; Initialize greeting

    ; Print statement
    mov rax, [rbp-56]
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_11
    mov rdi, rax
    call print_string

    ; Shutdown GC
    call gc_shutdown

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_11: db "TTO SSO Test Complete!", 0
str_10: db "!", 0
str_9: db "Merhaba, ", 0
str_8: db "Ali", 0
str_7: db "=== Mixed Test ===", 0
str_6: db "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0
str_5: db "This is a longer string that exceeds 23 bytes", 0
str_4: db "=== Long Strings (Heap) ===", 0
str_3: db "12345678901234567890123", 0
str_2: db "MLP Language", 0
str_1: db "Hello", 0
str_0: db "=== Short Strings (SSO) ===", 0
