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
    ; Declaration: numeric pi (TTO:double)
    sub rsp, 8         ; Allocate space for pi
    ; TTO: Loading double value
    mov rax, 0x400921fb54442d11    ; double 3.14159265358979 bit pattern
    mov [rbp-8], rax   ; Initialize pi

    ; Print statement
    mov rax, [rbp-8]
    movq xmm0, rax    ; TTO: Load double for print
    push rbx          ; Save for alignment
    mov rbx, rsp      ; Save stack
    and rsp, -16      ; Align to 16 bytes
    call print_double
    mov rsp, rbx      ; Restore stack
    pop rbx           ; Restore rbx
    ; Declaration: numeric e (TTO:double)
    sub rsp, 8         ; Allocate space for e
    ; TTO: Loading double value
    mov rax, 0x4005bf0a8b14575e    ; double 2.71828182845904 bit pattern
    mov [rbp-16], rax   ; Initialize e

    ; Print statement
    mov rax, [rbp-16]
    movq xmm0, rax    ; TTO: Load double for print
    push rbx          ; Save for alignment
    mov rbx, rsp      ; Save stack
    and rsp, -16      ; Align to 16 bytes
    call print_double
    mov rsp, rbx      ; Restore stack
    pop rbx           ; Restore rbx
    ; Declaration: numeric small (TTO:double)
    sub rsp, 8         ; Allocate space for small
    ; TTO: Loading double value
    mov rax, 0x3f50624dd2f1a9fc    ; double 0.001 bit pattern
    mov [rbp-24], rax   ; Initialize small

    ; Print statement
    mov rax, [rbp-24]
    movq xmm0, rax    ; TTO: Load double for print
    push rbx          ; Save for alignment
    mov rbx, rsp      ; Save stack
    and rsp, -16      ; Align to 16 bytes
    call print_double
    mov rsp, rbx      ; Restore stack
    pop rbx           ; Restore rbx
    ; Declaration: numeric large (TTO:double)
    sub rsp, 8         ; Allocate space for large
    ; TTO: Loading double value
    mov rax, 0x40fe240c9fbe76c9    ; double 123456.789 bit pattern
    mov [rbp-32], rax   ; Initialize large

    ; Print statement
    mov rax, [rbp-32]
    movq xmm0, rax    ; TTO: Load double for print
    push rbx          ; Save for alignment
    mov rbx, rsp      ; Save stack
    and rsp, -16      ; Align to 16 bytes
    call print_double
    mov rsp, rbx      ; Restore stack
    pop rbx           ; Restore rbx

    ; Print statement
    mov rax, str_1
    mov rdi, rax
    call print_string
    ; Declaration: numeric x
    sub rsp, 8         ; Allocate space for x
    mov rax, 42
    mov [rbp-40], rax   ; Initialize x

    ; Print statement
    mov rax, [rbp-40]
    mov rdi, rax
    call print_number
    ; Declaration: numeric big
    sub rsp, 8         ; Allocate space for big
    mov rax, 9223372036854775807
    mov [rbp-48], rax   ; Initialize big

    ; Print statement
    mov rax, [rbp-48]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_2
    mov rdi, rax
    call print_string
    ; Declaration: numeric a
    sub rsp, 8         ; Allocate space for a
    mov rax, 10
    mov [rbp-56], rax   ; Initialize a
    ; Declaration: numeric b
    sub rsp, 8         ; Allocate space for b
    mov rax, 3
    mov [rbp-64], rax   ; Initialize b
    ; Declaration: numeric sum_val
    sub rsp, 8         ; Allocate space for sum_val
    mov rax, [rbp-56]
    push rax
    mov rax, [rbp-64]
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rbp-72], rax   ; Initialize sum_val

    ; Print statement
    mov rax, [rbp-72]
    mov rdi, rax
    call print_number
    ; Declaration: numeric diff
    sub rsp, 8         ; Allocate space for diff
    mov rax, [rbp-56]
    push rax
    mov rax, [rbp-64]
    mov rbx, rax
    pop rax
    sub rax, rbx
    mov [rbp-80], rax   ; Initialize diff

    ; Print statement
    mov rax, [rbp-80]
    mov rdi, rax
    call print_number
    ; Declaration: numeric prod
    sub rsp, 8         ; Allocate space for prod
    mov rax, [rbp-56]
    push rax
    mov rax, [rbp-64]
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov [rbp-88], rax   ; Initialize prod

    ; Print statement
    mov rax, [rbp-88]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_3
    mov rdi, rax
    call print_string

    ; Print statement
    ; TTO: Loading double value
    mov rax, 0x3ff8000000000000    ; double 1.5 bit pattern
    movq xmm0, rax    ; TTO: double bit pattern to xmm0
    push rbx          ; Save for alignment calculation
    mov rbx, rsp      ; Save current stack
    and rsp, -16      ; Align stack to 16 bytes
    call print_double
    mov rsp, rbx      ; Restore stack
    pop rbx           ; Restore rbx

    ; Print statement
    ; TTO: Loading double value
    mov rax, 0x4058ff5c28f5c28f    ; double 99.99 bit pattern
    movq xmm0, rax    ; TTO: double bit pattern to xmm0
    push rbx          ; Save for alignment calculation
    mov rbx, rsp      ; Save current stack
    and rsp, -16      ; Align stack to 16 bytes
    call print_double
    mov rsp, rbx      ; Restore stack
    pop rbx           ; Restore rbx

    ; Print statement
    ; TTO: Loading double value
    mov rax, 0x3f1a36e2eb1c432d    ; double 0.0001 bit pattern
    movq xmm0, rax    ; TTO: double bit pattern to xmm0
    push rbx          ; Save for alignment calculation
    mov rbx, rsp      ; Save current stack
    and rsp, -16      ; Align stack to 16 bytes
    call print_double
    mov rsp, rbx      ; Restore stack
    pop rbx           ; Restore rbx

    ; Print statement
    mov rax, str_4
    mov rdi, rax
    call print_string

    ; Shutdown GC
    call gc_shutdown

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_4: db "TTO Comprehensive Test Complete!", 0
str_3: db "=== Direct Double Prints ===", 0
str_2: db "=== Integer Arithmetic ===", 0
str_1: db "=== Integer Literals ===", 0
str_0: db "=== Double Literals ===", 0
