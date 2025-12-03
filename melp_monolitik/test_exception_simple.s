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
extern mlp_to_string
extern mlp_to_decimal
extern mlp_decimal_to_string
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


    ; Try-catch-finally block (setjmp/longjmp)
    ; Push exception handler
    call mlp_exception_push
    mov r15, rax        ; Save handler pointer
    ; setjmp - returns 0 on first call, 1 on longjmp
    mov rdi, r15
    call setjmp
    test rax, rax
    jnz .L0_catch    ; Jump to catch if exception thrown
.L0_try:
    ; Try body
    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, 10
    mov [rbp-8], rax   ; Initialize x
    ; Declaration: numeric y
    sub rsp, 8         ; Allocate space for y
    mov rax, 0
    mov [rbp-16], rax   ; Initialize y

    ; Throw exception
    mov rdi, str_0    ; Default type: Error
    push rdi           ; Save type
    mov rax, [rbp-16]
    mov rsi, rax       ; Message in rsi
    pop rdi            ; Restore type
    mov rdx, 1         ; Error code
    call mlp_throw
    ; Try succeeded - pop handler
    call mlp_exception_pop
    jmp .L1_end
.L0_catch:
    ; Catch block(s)

    ; Catch block 0
    ; Store exception in variable
    call mlp_exception_message
    mov [rbp-24], rax

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number
    ; Caught - pop handler
    call mlp_exception_pop
    jmp .L1_end
.L3:  ; Re-throw/Uncaught
    ; No catch matched - check if parent handler exists (before popping)
    call mlp_exception_has_parent_handler
    test rax, rax
    jnz .L3_do_rethrow
.L3_uncaught:
    ; Uncaught exception - print and exit
    mov rdi, str_1
    call print_string
    call mlp_exception_type
    mov rdi, rax
    call print_string
    mov rdi, str_2
    call print_string
    call mlp_exception_message
    mov rdi, rax
    call print_string
    mov rax, 60       ; sys_exit
    mov rdi, 1        ; error code
    syscall
.L3_do_rethrow:
    ; Re-throw to parent handler
    call mlp_exception_type
    push rax
    call mlp_exception_message
    push rax
    call mlp_exception_code
    mov rdx, rax
    pop rsi
    pop rdi
    call mlp_exception_pop    ; Pop AFTER saving data
    call mlp_throw
.L1_end:

    ; Print statement
    mov rax, 42
    mov rdi, rax
    call print_number

    ; Shutdown GC
    call gc_shutdown

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_2: db " - ", 0
str_1: db "Uncaught exception: ", 0
str_0: db "Error", 0
