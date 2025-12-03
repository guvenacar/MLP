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
    ; Declaration: numeric a (TTO:double)
    sub rsp, 8         ; Allocate space for a
    ; TTO: Loading double value
    mov rax, 0x400921f9f01b866e    ; double 3.14159 bit pattern
    mov [rbp-8], rax   ; Initialize a
    ; Declaration: numeric b (TTO:double)
    sub rsp, 8         ; Allocate space for b
    ; TTO: Loading double value
    mov rax, 0x4005bf0995aaf790    ; double 2.71828 bit pattern
    mov [rbp-16], rax   ; Initialize b
    ; Declaration: numeric c (TTO:double)
    sub rsp, 8         ; Allocate space for c
    ; TTO: Double arithmetic
    mov rax, [rbp-8]
    movq xmm0, rax        ; Load double bit pattern
    sub rsp, 8
    movsd [rsp], xmm0      ; Save left operand
    mov rax, [rbp-16]
    movq xmm1, rax        ; Load double bit pattern
    movsd xmm0, [rsp]      ; Restore left operand
    add rsp, 8
    addsd xmm0, xmm1     ; Double add
    movq rax, xmm0         ; Result to rax as bit pattern
    mov [rbp-24], rax   ; Initialize c

    ; Print statement
    mov rax, [rbp-24]
    movq xmm0, rax    ; TTO: Load double for print
    push rbx          ; Save for alignment
    mov rbx, rsp      ; Save stack
    and rsp, -16      ; Align to 16 bytes
    call print_double
    mov rsp, rbx      ; Restore stack
    pop rbx           ; Restore rbx
    ; Declaration: numeric d (TTO:double)
    sub rsp, 8         ; Allocate space for d
    ; TTO: Double arithmetic
    mov rax, [rbp-8]
    movq xmm0, rax        ; Load double bit pattern
    sub rsp, 8
    movsd [rsp], xmm0      ; Save left operand
    mov rax, [rbp-16]
    movq xmm1, rax        ; Load double bit pattern
    movsd xmm0, [rsp]      ; Restore left operand
    add rsp, 8
    subsd xmm0, xmm1     ; Double subtract
    movq rax, xmm0         ; Result to rax as bit pattern
    mov [rbp-32], rax   ; Initialize d

    ; Print statement
    mov rax, [rbp-32]
    movq xmm0, rax    ; TTO: Load double for print
    push rbx          ; Save for alignment
    mov rbx, rsp      ; Save stack
    and rsp, -16      ; Align to 16 bytes
    call print_double
    mov rsp, rbx      ; Restore stack
    pop rbx           ; Restore rbx
    ; Declaration: numeric e (TTO:double)
    sub rsp, 8         ; Allocate space for e
    ; TTO: Double arithmetic
    mov rax, [rbp-8]
    movq xmm0, rax        ; Load double bit pattern
    sub rsp, 8
    movsd [rsp], xmm0      ; Save left operand
    mov rax, [rbp-16]
    movq xmm1, rax        ; Load double bit pattern
    movsd xmm0, [rsp]      ; Restore left operand
    add rsp, 8
    mulsd xmm0, xmm1     ; Double multiply
    movq rax, xmm0         ; Result to rax as bit pattern
    mov [rbp-40], rax   ; Initialize e

    ; Print statement
    mov rax, [rbp-40]
    movq xmm0, rax    ; TTO: Load double for print
    push rbx          ; Save for alignment
    mov rbx, rsp      ; Save stack
    and rsp, -16      ; Align to 16 bytes
    call print_double
    mov rsp, rbx      ; Restore stack
    pop rbx           ; Restore rbx
    ; Declaration: numeric f (TTO:double)
    sub rsp, 8         ; Allocate space for f
    ; TTO: Double arithmetic
    mov rax, [rbp-8]
    movq xmm0, rax        ; Load double bit pattern
    sub rsp, 8
    movsd [rsp], xmm0      ; Save left operand
    mov rax, [rbp-16]
    movq xmm1, rax        ; Load double bit pattern
    movsd xmm0, [rsp]      ; Restore left operand
    add rsp, 8
    divsd xmm0, xmm1     ; Double divide
    movq rax, xmm0         ; Result to rax as bit pattern
    mov [rbp-48], rax   ; Initialize f

    ; Print statement
    mov rax, [rbp-48]
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
    mov rax, 10
    mov [rbp-56], rax   ; Initialize x
    ; Declaration: numeric y (TTO:double)
    sub rsp, 8         ; Allocate space for y
    ; TTO: Loading double value
    mov rax, 0x4004000000000000    ; double 2.5 bit pattern
    mov [rbp-64], rax   ; Initialize y
    ; Declaration: numeric z (TTO:double)
    sub rsp, 8         ; Allocate space for z
    ; TTO: Double arithmetic
    mov rax, [rbp-56]
    cvtsi2sd xmm0, rax   ; Convert int to double
    sub rsp, 8
    movsd [rsp], xmm0      ; Save left operand
    mov rax, [rbp-64]
    movq xmm1, rax        ; Load double bit pattern
    movsd xmm0, [rsp]      ; Restore left operand
    add rsp, 8
    addsd xmm0, xmm1     ; Double add
    movq rax, xmm0         ; Result to rax as bit pattern
    mov [rbp-72], rax   ; Initialize z

    ; Print statement
    mov rax, [rbp-72]
    movq xmm0, rax    ; TTO: Load double for print
    push rbx          ; Save for alignment
    mov rbx, rsp      ; Save stack
    and rsp, -16      ; Align to 16 bytes
    call print_double
    mov rsp, rbx      ; Restore stack
    pop rbx           ; Restore rbx
    ; Declaration: numeric w (TTO:double)
    sub rsp, 8         ; Allocate space for w
    ; TTO: Double arithmetic
    mov rax, [rbp-56]
    cvtsi2sd xmm0, rax   ; Convert int to double
    sub rsp, 8
    movsd [rsp], xmm0      ; Save left operand
    mov rax, [rbp-64]
    movq xmm1, rax        ; Load double bit pattern
    movsd xmm0, [rsp]      ; Restore left operand
    add rsp, 8
    mulsd xmm0, xmm1     ; Double multiply
    movq rax, xmm0         ; Result to rax as bit pattern
    mov [rbp-80], rax   ; Initialize w

    ; Print statement
    mov rax, [rbp-80]
    movq xmm0, rax    ; TTO: Load double for print
    push rbx          ; Save for alignment
    mov rbx, rsp      ; Save stack
    and rsp, -16      ; Align to 16 bytes
    call print_double
    mov rsp, rbx      ; Restore stack
    pop rbx           ; Restore rbx
    ; Declaration: numeric v (TTO:double)
    sub rsp, 8         ; Allocate space for v
    ; TTO: Double arithmetic
    mov rax, [rbp-56]
    cvtsi2sd xmm0, rax   ; Convert int to double
    sub rsp, 8
    movsd [rsp], xmm0      ; Save left operand
    mov rax, [rbp-64]
    movq xmm1, rax        ; Load double bit pattern
    movsd xmm0, [rsp]      ; Restore left operand
    add rsp, 8
    divsd xmm0, xmm1     ; Double divide
    movq rax, xmm0         ; Result to rax as bit pattern
    mov [rbp-88], rax   ; Initialize v

    ; Print statement
    mov rax, [rbp-88]
    movq xmm0, rax    ; TTO: Load double for print
    push rbx          ; Save for alignment
    mov rbx, rsp      ; Save stack
    and rsp, -16      ; Align to 16 bytes
    call print_double
    mov rsp, rbx      ; Restore stack
    pop rbx           ; Restore rbx

    ; Print statement
    mov rax, str_2
    mov rdi, rax
    call print_string
    ; Declaration: numeric p (TTO:double)
    sub rsp, 8         ; Allocate space for p
    ; TTO: Loading double value
    mov rax, 0x400c000000000000    ; double 3.5 bit pattern
    mov [rbp-96], rax   ; Initialize p
    ; Declaration: numeric q
    sub rsp, 8         ; Allocate space for q
    mov rax, 2
    mov [rbp-104], rax   ; Initialize q
    ; Declaration: numeric r (TTO:double)
    sub rsp, 8         ; Allocate space for r
    ; TTO: Double arithmetic
    mov rax, [rbp-96]
    movq xmm0, rax        ; Load double bit pattern
    sub rsp, 8
    movsd [rsp], xmm0      ; Save left operand
    mov rax, [rbp-104]
    cvtsi2sd xmm1, rax   ; Convert int to double
    movsd xmm0, [rsp]      ; Restore left operand
    add rsp, 8
    addsd xmm0, xmm1     ; Double add
    movq rax, xmm0         ; Result to rax as bit pattern
    mov [rbp-112], rax   ; Initialize r

    ; Print statement
    mov rax, [rbp-112]
    movq xmm0, rax    ; TTO: Load double for print
    push rbx          ; Save for alignment
    mov rbx, rsp      ; Save stack
    and rsp, -16      ; Align to 16 bytes
    call print_double
    mov rsp, rbx      ; Restore stack
    pop rbx           ; Restore rbx
    ; Declaration: numeric s (TTO:double)
    sub rsp, 8         ; Allocate space for s
    ; TTO: Double arithmetic
    mov rax, [rbp-96]
    movq xmm0, rax        ; Load double bit pattern
    sub rsp, 8
    movsd [rsp], xmm0      ; Save left operand
    mov rax, [rbp-104]
    cvtsi2sd xmm1, rax   ; Convert int to double
    movsd xmm0, [rsp]      ; Restore left operand
    add rsp, 8
    mulsd xmm0, xmm1     ; Double multiply
    movq rax, xmm0         ; Result to rax as bit pattern
    mov [rbp-120], rax   ; Initialize s

    ; Print statement
    mov rax, [rbp-120]
    movq xmm0, rax    ; TTO: Load double for print
    push rbx          ; Save for alignment
    mov rbx, rsp      ; Save stack
    and rsp, -16      ; Align to 16 bytes
    call print_double
    mov rsp, rbx      ; Restore stack
    pop rbx           ; Restore rbx

    ; Print statement
    mov rax, str_3
    mov rdi, rax
    call print_string
    ; Declaration: numeric result (TTO:double)
    sub rsp, 8         ; Allocate space for result
    ; TTO: Double arithmetic
    ; TTO: Double arithmetic
    ; TTO: Loading double value
    mov rax, 0x3ff8000000000000    ; double 1.5 bit pattern
    movq xmm0, rax        ; Load double bit pattern
    sub rsp, 8
    movsd [rsp], xmm0      ; Save left operand
    ; TTO: Loading double value
    mov rax, 0x4004000000000000    ; double 2.5 bit pattern
    movq xmm1, rax        ; Load double bit pattern
    movsd xmm0, [rsp]      ; Restore left operand
    add rsp, 8
    addsd xmm0, xmm1     ; Double add
    movq rax, xmm0         ; Result to rax as bit pattern
    movq xmm0, rax        ; Load double bit pattern
    sub rsp, 8
    movsd [rsp], xmm0      ; Save left operand
    ; TTO: Loading double value
    mov rax, 0x4008000000000000    ; double 3 bit pattern
    movq xmm1, rax        ; Load double bit pattern
    movsd xmm0, [rsp]      ; Restore left operand
    add rsp, 8
    mulsd xmm0, xmm1     ; Double multiply
    movq rax, xmm0         ; Result to rax as bit pattern
    mov [rbp-128], rax   ; Initialize result

    ; Print statement
    mov rax, [rbp-128]
    movq xmm0, rax    ; TTO: Load double for print
    push rbx          ; Save for alignment
    mov rbx, rsp      ; Save stack
    and rsp, -16      ; Align to 16 bytes
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
str_4: db "TTO Double Arithmetic Complete!", 0
str_3: db "=== Complex Expression ===", 0
str_2: db "=== Double + Int (Mixed) ===", 0
str_1: db "=== Int + Double (Mixed) ===", 0
str_0: db "=== Double + Double ===", 0
