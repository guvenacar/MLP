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

    ; Declaration: text num_str (TTO:SSO)
    sub rsp, 8         ; Allocate space for num_str
    mov rax, str_0
    mov [rbp-8], rax   ; Initialize num_str
    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, [rbp-8]
    push rax
    pop rdi
    call mlp_to_numeric
    mov [rbp-16], rax   ; Initialize x

    ; Print statement
    mov rax, [rbp-16]
    mov rdi, rax
    call print_number
    ; Declaration: numeric y
    sub rsp, 8         ; Allocate space for y
    mov rax, 123
    mov [rbp-24], rax   ; Initialize y
    ; Declaration: text y_str
    sub rsp, 8         ; Allocate space for y_str
    mov rax, [rbp-24]
    push rax
    pop rdi
    call mlp_to_text
    mov [rbp-32], rax   ; Initialize y_str

    ; Print statement
    mov rax, [rbp-32]
    mov rdi, rax
    call print_string
    ; Declaration: numeric neg
    sub rsp, 8         ; Allocate space for neg
    mov rax, 0
    push rax
    mov rax, 5
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-40], rax   ; Initialize neg
    ; Declaration: numeric pos
    sub rsp, 8         ; Allocate space for pos
    mov rax, [rbp-40]
    push rax
    pop rdi
    call mlp_abs_int
    mov [rbp-48], rax   ; Initialize pos

    ; Print statement
    mov rax, [rbp-48]
    mov rdi, rax
    call print_number
    ; Declaration: text hello (TTO:SSO)
    sub rsp, 8         ; Allocate space for hello
    mov rax, str_1
    mov [rbp-56], rax   ; Initialize hello
    ; Declaration: text trimmed
    sub rsp, 8         ; Allocate space for trimmed
    mov rax, [rbp-56]
    push rax
    pop rdi
    call mlp_trim
    mov [rbp-64], rax   ; Initialize trimmed

    ; Print statement
    mov rax, [rbp-64]
    mov rdi, rax
    call print_string
    ; Declaration: text upper
    sub rsp, 8         ; Allocate space for upper
    mov rax, str_2
    push rax
    pop rdi
    call mlp_to_upper
    mov [rbp-72], rax   ; Initialize upper

    ; Print statement
    mov rax, [rbp-72]
    mov rdi, rax
    call print_string
    ; Declaration: text lower
    sub rsp, 8         ; Allocate space for lower
    mov rax, str_3
    push rax
    pop rdi
    call mlp_to_lower
    mov [rbp-80], rax   ; Initialize lower

    ; Print statement
    mov rax, [rbp-80]
    mov rdi, rax
    call print_string
    ; Declaration: numeric sw
    sub rsp, 8         ; Allocate space for sw
    mov rax, str_4
    push rax
    mov rax, str_5
    push rax
    pop rsi
    pop rdi
    call mlp_starts_with
    mov [rbp-88], rax   ; Initialize sw

    ; Print statement
    mov rax, [rbp-88]
    mov rdi, rax
    call print_number
    ; Declaration: numeric ew
    sub rsp, 8         ; Allocate space for ew
    mov rax, str_6
    push rax
    mov rax, str_7
    push rax
    pop rsi
    pop rdi
    call mlp_ends_with
    mov [rbp-96], rax   ; Initialize ew

    ; Print statement
    mov rax, [rbp-96]
    mov rdi, rax
    call print_number
    ; Declaration: numeric ct
    sub rsp, 8         ; Allocate space for ct
    mov rax, str_8
    push rax
    mov rax, str_9
    push rax
    pop rsi
    pop rdi
    call mlp_contains
    mov [rbp-104], rax   ; Initialize ct

    ; Print statement
    mov rax, [rbp-104]
    mov rdi, rax
    call print_number
    ; Declaration: text replaced
    sub rsp, 8         ; Allocate space for replaced
    mov rax, str_10
    push rax
    mov rax, str_11
    push rax
    mov rax, str_12
    push rax
    pop rdx
    pop rsi
    pop rdi
    call mlp_replace
    mov [rbp-112], rax   ; Initialize replaced

    ; Print statement
    mov rax, [rbp-112]
    mov rdi, rax
    call print_string
    mov rax, 1
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0      ; Default false
    mov rbx, 1      ; True value
    cmove rax, rbx  ; If equal, set rax=1
    push rax
    mov rax, str_13
    push rax
    pop rsi
    pop rdi
    call mlp_assert

    ; Print statement
    mov rax, str_14
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_15
    mov rdi, rax
    call print_string

    ; Shutdown GC
    call gc_shutdown

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_15: db "All Phase 20 tests completed!", 0
str_14: db "Assert passed!", 0
str_13: db "Math is broken!", 0
str_12: db "MLP", 0
str_11: db "World", 0
str_10: db "Hello World", 0
str_9: db "World", 0
str_8: db "Hello World", 0
str_7: db "lo", 0
str_6: db "Hello", 0
str_5: db "He", 0
str_4: db "Hello", 0
str_3: db "HELLO", 0
str_2: db "hello", 0
str_1: db "  Hello World  ", 0
str_0: db "42", 0
