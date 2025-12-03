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


global func_lexer_init
func_lexer_init:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param src
    sub rsp, 8
    mov [rbp-8], rax   ; Store param src locally

    ; Assignment: g_source = ...
    mov rax, [rbp-8]
    mov [state_g_source], rax   ; Store to state g_source

    ; Assignment: g_pos = ...
    mov rax, 0
    mov [state_g_pos], rax   ; Store to state g_pos

    ; Assignment: g_line = ...
    mov rax, 1
    mov [state_g_line], rax   ; Store to state g_line

    ; Assignment: g_length = ...
    mov rax, [rbp-8]
    push rax
    pop rdi
    call mlp_string_length
    mov [state_g_length], rax   ; Store to state g_length
    mov rsp, rbp
    pop rbp
    ret

global func_is_whitespace
func_is_whitespace:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param c
    sub rsp, 8
    mov [rbp-8], rax   ; Store param c locally

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_0
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L0
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L0:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_1
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L2
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L2:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_2
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L4
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L4:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_3
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L6
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L6:
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

global func_is_digit
func_is_digit:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param c
    sub rsp, 8
    mov [rbp-8], rax   ; Store param c locally

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_4
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L8
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L8:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_5
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L10
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L10:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_6
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L12
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L12:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_7
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L14
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L14:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_8
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L16
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L16:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_9
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L18
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L18:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_10
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L20
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L20:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_11
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L22
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L22:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_12
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L24
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L24:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_13
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L26
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L26:
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

global func_is_alpha
func_is_alpha:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param c
    sub rsp, 8
    mov [rbp-8], rax   ; Store param c locally

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_14
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L28
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L28:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_15
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L30
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L30:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_16
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L32
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L32:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_17
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L34
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L34:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_18
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L36
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L36:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_19
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L38
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L38:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_20
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L40
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L40:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_21
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L42
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L42:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_22
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L44
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L44:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_23
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L46
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L46:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_24
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L48
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L48:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_25
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L50
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L50:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_26
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L52
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L52:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_27
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L54
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L54:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_28
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L56
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L56:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_29
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L58
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L58:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_30
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L60
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L60:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_31
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L62
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L62:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_32
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L64
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L64:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_33
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L66
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L66:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_34
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L68
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L68:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_35
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L70
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L70:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_36
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L72
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L72:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_37
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L74
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L74:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_38
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L76
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L76:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_39
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L78
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L78:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_40
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L80
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L80:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_41
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L82
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L82:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_42
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L84
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L84:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_43
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L86
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L86:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_44
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L88
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L88:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_45
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L90
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L90:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_46
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L92
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L92:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_47
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L94
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L94:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_48
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L96
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L96:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_49
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L98
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L98:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_50
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L100
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L100:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_51
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L102
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L102:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_52
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L104
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L104:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_53
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L106
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L106:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_54
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L108
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L108:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_55
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L110
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L110:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_56
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L112
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L112:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_57
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L114
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L114:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_58
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L116
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L116:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_59
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L118
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L118:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_60
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L120
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L120:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_61
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L122
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L122:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_62
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L124
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L124:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_63
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L126
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L126:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_64
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L128
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L128:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_65
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L130
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L130:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_66
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L132
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L132:
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

global func_is_alnum
func_is_alnum:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param c
    sub rsp, 8
    mov [rbp-8], rax   ; Store param c locally

    ; If statement
    mov rax, [rbp-8]
    push rax
    call func_is_alpha
    add rsp, 8
    test rax, rax
    jz .L134
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L134:

    ; If statement
    mov rax, [rbp-8]
    push rax
    call func_is_digit
    add rsp, 8
    test rax, rax
    jz .L136
    ; Then body
    mov rax, 1
    mov rsp, rbp
    pop rbp
    ret
.L136:
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret

global func_lexer_current
func_lexer_current:
    push rbp
    mov rbp, rsp

    ; If statement

    ; Evaluate comparison
    mov rax, [state_g_pos]
    push rax
    mov rax, [state_g_length]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L138
    ; Then body
    mov rax, str_67
    mov rsp, rbp
    pop rbp
    ret
.L138:
    mov rax, [state_g_source]
    push rax
    mov rax, [state_g_pos]
    push rax
    pop rsi
    pop rdi
    call mlp_charAt
    mov rsp, rbp
    pop rbp
    ret

global func_lexer_peek
func_lexer_peek:
    push rbp
    mov rbp, rsp

    ; If statement

    ; Evaluate comparison
    mov rax, [state_g_pos]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    push rax
    mov rax, [state_g_length]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L140
    ; Then body
    mov rax, str_68
    mov rsp, rbp
    pop rbp
    ret
.L140:
    mov rax, [state_g_source]
    push rax
    mov rax, [state_g_pos]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    push rax
    pop rsi
    pop rdi
    call mlp_charAt
    mov rsp, rbp
    pop rbp
    ret

global func_lexer_advance
func_lexer_advance:
    push rbp
    mov rbp, rsp
    ; Declaration: string c
    sub rsp, 8         ; Allocate space for c
    call func_lexer_current
    mov [rbp-8], rax   ; Initialize c

    ; Assignment: g_pos = ...
    mov rax, [state_g_pos]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [state_g_pos], rax   ; Store to state g_pos

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_69
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L142
    ; Then body

    ; Assignment: g_line = ...
    mov rax, [state_g_line]
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [state_g_line], rax   ; Store to state g_line
.L142:
    mov rsp, rbp
    pop rbp
    ret

global func_lexer_skip_whitespace
func_lexer_skip_whitespace:
    push rbp
    mov rbp, rsp

    ; While loop
.L144:  ; while_start

    ; Evaluate comparison
    mov rax, [state_g_pos]
    push rax
    mov rax, [state_g_length]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L145
    ; While body
    ; Declaration: string c
    sub rsp, 8         ; Allocate space for c
    call func_lexer_current
    mov [rbp-8], rax   ; Initialize c

    ; If statement
    mov rax, [rbp-8]
    push rax
    call func_is_whitespace
    add rsp, 8
    test rax, rax
    jz .L146
    ; Then body
    call func_lexer_advance
    jmp .L147
.L146:
    ; Else body
    jmp .L145           ; exit
.L147:
    jmp .L144
.L145:  ; while_end
    mov rsp, rbp
    pop rbp
    ret

global func_lexer_skip_comment
func_lexer_skip_comment:
    push rbp
    mov rbp, rsp
    call func_lexer_advance
    call func_lexer_advance

    ; While loop
.L148:  ; while_start

    ; Evaluate comparison
    mov rax, [state_g_pos]
    push rax
    mov rax, [state_g_length]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L149
    ; While body
    ; Declaration: string c
    sub rsp, 8         ; Allocate space for c
    call func_lexer_current
    mov [rbp-8], rax   ; Initialize c

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_70
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L150
    ; Then body
    jmp .L149           ; exit
.L150:
    call func_lexer_advance
    jmp .L148
.L149:  ; while_end
    mov rsp, rbp
    pop rbp
    ret

global func_lexer_read_word
func_lexer_read_word:
    push rbp
    mov rbp, rsp
    ; Declaration: string word (TTO:SSO)
    sub rsp, 8         ; Allocate space for word
    mov rax, str_71
    mov [rbp-8], rax   ; Initialize word

    ; While loop
.L152:  ; while_start

    ; Evaluate comparison
    mov rax, [state_g_pos]
    push rax
    mov rax, [state_g_length]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L153
    ; While body
    ; Declaration: string c
    sub rsp, 8         ; Allocate space for c
    call func_lexer_current
    mov [rbp-16], rax   ; Initialize c

    ; If statement
    mov rax, [rbp-16]
    push rax
    call func_is_alnum
    add rsp, 8
    test rax, rax
    jz .L154
    ; Then body

    ; Assignment: word = ...
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    mov rsi, rax
    pop rdi
    call string_concat
    mov [rbp-8], rax   ; Store to word
    call func_lexer_advance
    jmp .L155
.L154:
    ; Else body
    jmp .L153           ; exit
.L155:
    jmp .L152
.L153:  ; while_end
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret

global func_lexer_read_number
func_lexer_read_number:
    push rbp
    mov rbp, rsp
    ; Declaration: string num (TTO:SSO)
    sub rsp, 8         ; Allocate space for num
    mov rax, str_72
    mov [rbp-8], rax   ; Initialize num

    ; While loop
.L156:  ; while_start

    ; Evaluate comparison
    mov rax, [state_g_pos]
    push rax
    mov rax, [state_g_length]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L157
    ; While body
    ; Declaration: string c
    sub rsp, 8         ; Allocate space for c
    call func_lexer_current
    mov [rbp-16], rax   ; Initialize c

    ; If statement

    ; Logical OR
    mov rax, [rbp-16]
    push rax
    call func_is_digit
    add rsp, 8
    test rax, rax
    jnz .L160          ; Short-circuit if true
    mov rax, [rbp-16]
    push rax
    mov rax, str_73
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0      ; Default false
    mov rbx, 1      ; True value
    cmove rax, rbx  ; If equal, set rax=1
    test rax, rax
    jnz .L160          ; Check right side
    mov rax, 0          ; Result: false
    jmp .L161
.L160:
    mov rax, 1          ; Result: true
.L161:
    test rax, rax
    jz .L158
    ; Then body

    ; Assignment: num = ...
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    mov rsi, rax
    pop rdi
    call string_concat
    mov [rbp-8], rax   ; Store to num
    call func_lexer_advance
    jmp .L159
.L158:
    ; Else body
    jmp .L157           ; exit
.L159:
    jmp .L156
.L157:  ; while_end
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret

global func_lexer_read_string
func_lexer_read_string:
    push rbp
    mov rbp, rsp
    ; Declaration: string str (TTO:SSO)
    sub rsp, 8         ; Allocate space for str
    mov rax, str_74
    mov [rbp-8], rax   ; Initialize str
    call func_lexer_advance

    ; While loop
.L163:  ; while_start

    ; Evaluate comparison
    mov rax, [state_g_pos]
    push rax
    mov rax, [state_g_length]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L164
    ; While body
    ; Declaration: string c
    sub rsp, 8         ; Allocate space for c
    call func_lexer_current
    mov [rbp-16], rax   ; Initialize c

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-16]
    mov rdi, rax
    push rdi
    mov rax, str_75
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L165
    ; Then body
    call func_lexer_advance
    jmp .L164           ; exit
.L165:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-16]
    mov rdi, rax
    push rdi
    mov rax, str_76
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L167
    ; Then body
    call func_lexer_advance
    ; Declaration: string escaped
    sub rsp, 8         ; Allocate space for escaped
    call func_lexer_current
    mov [rbp-24], rax   ; Initialize escaped

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_77
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L169
    ; Then body

    ; Assignment: str = ...
    mov rax, [rbp-8]
    push rax
    mov rax, str_78
    mov rsi, rax
    pop rdi
    call string_concat
    mov [rbp-8], rax   ; Store to str
    jmp .L170
.L169:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_79
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L171
    ; Then body

    ; Assignment: str = ...
    mov rax, [rbp-8]
    push rax
    mov rax, str_80
    mov rsi, rax
    pop rdi
    call string_concat
    mov [rbp-8], rax   ; Store to str
    jmp .L172
.L171:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_81
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L173
    ; Then body

    ; Assignment: str = ...
    mov rax, [rbp-8]
    push rax
    mov rax, str_82
    mov rsi, rax
    pop rdi
    call string_concat
    mov [rbp-8], rax   ; Store to str
    jmp .L174
.L173:
    ; Else body

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_83
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L175
    ; Then body

    ; Assignment: str = ...
    mov rax, [rbp-8]
    push rax
    mov rax, str_84
    mov rsi, rax
    pop rdi
    call string_concat
    mov [rbp-8], rax   ; Store to str
    jmp .L176
.L175:
    ; Else body

    ; Assignment: str = ...
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-24]
    mov rsi, rax
    pop rdi
    call string_concat
    mov [rbp-8], rax   ; Store to str
.L176:
.L174:
.L172:
.L170:
    call func_lexer_advance
    jmp .L168
.L167:
    ; Else body

    ; Assignment: str = ...
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    mov rsi, rax
    pop rdi
    call string_concat
    mov [rbp-8], rax   ; Store to str
    call func_lexer_advance
.L168:
    jmp .L163
.L164:  ; while_end
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret

global func_get_keyword_type
func_get_keyword_type:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param word
    sub rsp, 8
    mov [rbp-8], rax   ; Store param word locally

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_85
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L177
    ; Then body
    mov rax, [state_TOKEN_NUMERIC]
    mov rsp, rbp
    pop rbp
    ret
.L177:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_86
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L179
    ; Then body
    mov rax, [state_TOKEN_DECIMAL]
    mov rsp, rbp
    pop rbp
    ret
.L179:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_87
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L181
    ; Then body
    mov rax, [state_TOKEN_BOOLEAN]
    mov rsp, rbp
    pop rbp
    ret
.L181:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_88
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L183
    ; Then body
    mov rax, [state_TOKEN_TEXT]
    mov rsp, rbp
    pop rbp
    ret
.L183:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_89
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L185
    ; Then body
    mov rax, [state_TOKEN_PRINT]
    mov rsp, rbp
    pop rbp
    ret
.L185:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_90
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L187
    ; Then body
    mov rax, [state_TOKEN_IF]
    mov rsp, rbp
    pop rbp
    ret
.L187:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_91
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L189
    ; Then body
    mov rax, [state_TOKEN_THEN]
    mov rsp, rbp
    pop rbp
    ret
.L189:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_92
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L191
    ; Then body
    mov rax, [state_TOKEN_ELSE]
    mov rsp, rbp
    pop rbp
    ret
.L191:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_93
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L193
    ; Then body
    mov rax, [state_TOKEN_END]
    mov rsp, rbp
    pop rbp
    ret
.L193:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_94
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L195
    ; Then body
    mov rax, [state_TOKEN_FOR]
    mov rsp, rbp
    pop rbp
    ret
.L195:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_95
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L197
    ; Then body
    mov rax, [state_TOKEN_TO]
    mov rsp, rbp
    pop rbp
    ret
.L197:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_96
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L199
    ; Then body
    mov rax, [state_TOKEN_STEP]
    mov rsp, rbp
    pop rbp
    ret
.L199:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_97
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L201
    ; Then body
    mov rax, [state_TOKEN_WHILE]
    mov rsp, rbp
    pop rbp
    ret
.L201:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_98
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L203
    ; Then body
    mov rax, [state_TOKEN_DO]
    mov rsp, rbp
    pop rbp
    ret
.L203:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_99
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L205
    ; Then body
    mov rax, [state_TOKEN_AND]
    mov rsp, rbp
    pop rbp
    ret
.L205:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_100
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L207
    ; Then body
    mov rax, [state_TOKEN_OR]
    mov rsp, rbp
    pop rbp
    ret
.L207:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_101
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L209
    ; Then body
    mov rax, [state_TOKEN_NOT]
    mov rsp, rbp
    pop rbp
    ret
.L209:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_102
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L211
    ; Then body
    mov rax, [state_TOKEN_FUNC]
    mov rsp, rbp
    pop rbp
    ret
.L211:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_103
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L213
    ; Then body
    mov rax, [state_TOKEN_RETURN]
    mov rsp, rbp
    pop rbp
    ret
.L213:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_104
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L215
    ; Then body
    mov rax, [state_TOKEN_RETURNS]
    mov rsp, rbp
    pop rbp
    ret
.L215:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_105
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L217
    ; Then body
    mov rax, [state_TOKEN_EXIT]
    mov rsp, rbp
    pop rbp
    ret
.L217:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_106
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L219
    ; Then body
    mov rax, [state_TOKEN_CONTINUE]
    mov rsp, rbp
    pop rbp
    ret
.L219:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_107
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L221
    ; Then body
    mov rax, [state_TOKEN_STRUCT]
    mov rsp, rbp
    pop rbp
    ret
.L221:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_108
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L223
    ; Then body
    mov rax, [state_TOKEN_IMPORT]
    mov rsp, rbp
    pop rbp
    ret
.L223:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_109
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L225
    ; Then body
    mov rax, [state_TOKEN_MODULE]
    mov rsp, rbp
    pop rbp
    ret
.L225:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    mov rdi, rax
    push rdi
    mov rax, str_110
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L227
    ; Then body
    mov rax, [state_TOKEN_EXPORT]
    mov rsp, rbp
    pop rbp
    ret
.L227:
    mov rax, [state_TOKEN_IDENTIFIER]
    mov rsp, rbp
    pop rbp
    ret

global func_set_token
func_set_token:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param ttype
    sub rsp, 8
    mov [rbp-8], rax   ; Store param ttype locally
    mov rax, [rbp+24]   ; Load param tvalue
    sub rsp, 8
    mov [rbp-16], rax   ; Store param tvalue locally
    mov rax, [rbp+32]   ; Load param tline
    sub rsp, 8
    mov [rbp-24], rax   ; Store param tline locally

    ; Assignment: g_tok_type = ...
    mov rax, [rbp-8]
    mov [state_g_tok_type], rax   ; Store to state g_tok_type

    ; Assignment: g_tok_value = ...
    mov rax, [rbp-16]
    mov [state_g_tok_value], rax   ; Store to state g_tok_value

    ; Assignment: g_tok_line = ...
    mov rax, [rbp-24]
    mov [state_g_tok_line], rax   ; Store to state g_tok_line
    mov rsp, rbp
    pop rbp
    ret

global func_lexer_next_token
func_lexer_next_token:
    push rbp
    mov rbp, rsp

    ; While loop
.L229:  ; while_start

    ; Evaluate comparison
    mov rax, [state_g_pos]
    push rax
    mov rax, [state_g_length]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jge .L230
    ; While body
    call func_lexer_skip_whitespace
    ; Declaration: string c
    sub rsp, 8         ; Allocate space for c
    call func_lexer_current
    mov [rbp-8], rax   ; Initialize c
    ; Declaration: string nxt
    sub rsp, 8         ; Allocate space for nxt
    call func_lexer_peek
    mov [rbp-16], rax   ; Initialize nxt

    ; If statement

    ; Logical AND
    mov rax, [rbp-8]
    push rax
    mov rax, str_111
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0      ; Default false
    mov rbx, 1      ; True value
    cmove rax, rbx  ; If equal, set rax=1
    test rax, rax
    jz .L233          ; Short-circuit if false
    mov rax, [rbp-16]
    push rax
    mov rax, str_112
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0      ; Default false
    mov rbx, 1      ; True value
    cmove rax, rbx  ; If equal, set rax=1
    test rax, rax
    jz .L233          ; Check right side
    mov rax, 1          ; Result: true
    jmp .L234
.L233:
    mov rax, 0          ; Result: false
.L234:
    test rax, rax
    jz .L231
    ; Then body
    call func_lexer_skip_comment
    jmp .L232
.L231:
    ; Else body
    jmp .L230           ; exit
.L232:
    jmp .L229
.L230:  ; while_end

    ; If statement

    ; Evaluate comparison
    mov rax, [state_g_pos]
    push rax
    mov rax, [state_g_length]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jl .L237
    ; Then body
    mov rax, [state_g_line]
    push rax
    mov rax, str_113
    push rax
    mov rax, [state_TOKEN_EOF]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L237:
    ; Declaration: string c
    sub rsp, 8         ; Allocate space for c
    call func_lexer_current
    mov [rbp-24], rax   ; Initialize c
    ; Declaration: numeric cur_line
    sub rsp, 8         ; Allocate space for cur_line
    mov rax, [state_g_line]
    mov [rbp-32], rax   ; Initialize cur_line

    ; If statement
    mov rax, [rbp-24]
    push rax
    call func_is_digit
    add rsp, 8
    test rax, rax
    jz .L239
    ; Then body
    ; Declaration: string num
    sub rsp, 8         ; Allocate space for num
    call func_lexer_read_number
    mov [rbp-40], rax   ; Initialize num
    mov rax, [rbp-32]
    push rax
    mov rax, [rbp-40]
    push rax
    mov rax, [state_TOKEN_NUMBER]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L239:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_114
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L241
    ; Then body
    ; Declaration: string str
    sub rsp, 8         ; Allocate space for str
    call func_lexer_read_string
    mov [rbp-48], rax   ; Initialize str
    mov rax, [rbp-32]
    push rax
    mov rax, [rbp-48]
    push rax
    mov rax, [state_TOKEN_STRING]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L241:

    ; If statement
    mov rax, [rbp-24]
    push rax
    call func_is_alpha
    add rsp, 8
    test rax, rax
    jz .L243
    ; Then body
    ; Declaration: string word
    sub rsp, 8         ; Allocate space for word
    call func_lexer_read_word
    mov [rbp-56], rax   ; Initialize word
    ; Declaration: numeric ttype
    sub rsp, 8         ; Allocate space for ttype
    mov rax, [rbp-56]
    push rax
    call func_get_keyword_type
    add rsp, 8
    mov [rbp-64], rax   ; Initialize ttype
    mov rax, [rbp-32]
    push rax
    mov rax, [rbp-56]
    push rax
    mov rax, [rbp-64]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L243:
    ; Declaration: string nxt
    sub rsp, 8         ; Allocate space for nxt
    call func_lexer_peek
    mov [rbp-72], rax   ; Initialize nxt

    ; If statement

    ; Logical AND
    mov rax, [rbp-24]
    push rax
    mov rax, str_115
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0      ; Default false
    mov rbx, 1      ; True value
    cmove rax, rbx  ; If equal, set rax=1
    test rax, rax
    jz .L247          ; Short-circuit if false
    mov rax, [rbp-72]
    push rax
    mov rax, str_116
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0      ; Default false
    mov rbx, 1      ; True value
    cmove rax, rbx  ; If equal, set rax=1
    test rax, rax
    jz .L247          ; Check right side
    mov rax, 1          ; Result: true
    jmp .L248
.L247:
    mov rax, 0          ; Result: false
.L248:
    test rax, rax
    jz .L245
    ; Then body
    call func_lexer_advance
    call func_lexer_advance
    mov rax, [rbp-32]
    push rax
    mov rax, str_117
    push rax
    mov rax, [state_TOKEN_EQUAL]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L245:

    ; If statement

    ; Logical AND
    mov rax, [rbp-24]
    push rax
    mov rax, str_118
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0      ; Default false
    mov rbx, 1      ; True value
    cmove rax, rbx  ; If equal, set rax=1
    test rax, rax
    jz .L253          ; Short-circuit if false
    mov rax, [rbp-72]
    push rax
    mov rax, str_119
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0      ; Default false
    mov rbx, 1      ; True value
    cmove rax, rbx  ; If equal, set rax=1
    test rax, rax
    jz .L253          ; Check right side
    mov rax, 1          ; Result: true
    jmp .L254
.L253:
    mov rax, 0          ; Result: false
.L254:
    test rax, rax
    jz .L251
    ; Then body
    call func_lexer_advance
    call func_lexer_advance
    mov rax, [rbp-32]
    push rax
    mov rax, str_120
    push rax
    mov rax, [state_TOKEN_NOT_EQUAL]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L251:

    ; If statement

    ; Logical AND
    mov rax, [rbp-24]
    push rax
    mov rax, str_121
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0      ; Default false
    mov rbx, 1      ; True value
    cmove rax, rbx  ; If equal, set rax=1
    test rax, rax
    jz .L259          ; Short-circuit if false
    mov rax, [rbp-72]
    push rax
    mov rax, str_122
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0      ; Default false
    mov rbx, 1      ; True value
    cmove rax, rbx  ; If equal, set rax=1
    test rax, rax
    jz .L259          ; Check right side
    mov rax, 1          ; Result: true
    jmp .L260
.L259:
    mov rax, 0          ; Result: false
.L260:
    test rax, rax
    jz .L257
    ; Then body
    call func_lexer_advance
    call func_lexer_advance
    mov rax, [rbp-32]
    push rax
    mov rax, str_123
    push rax
    mov rax, [state_TOKEN_LESS_EQUAL]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L257:

    ; If statement

    ; Logical AND
    mov rax, [rbp-24]
    push rax
    mov rax, str_124
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0      ; Default false
    mov rbx, 1      ; True value
    cmove rax, rbx  ; If equal, set rax=1
    test rax, rax
    jz .L265          ; Short-circuit if false
    mov rax, [rbp-72]
    push rax
    mov rax, str_125
    mov rbx, rax
    pop rax
    cmp rax, rbx
    mov rax, 0      ; Default false
    mov rbx, 1      ; True value
    cmove rax, rbx  ; If equal, set rax=1
    test rax, rax
    jz .L265          ; Check right side
    mov rax, 1          ; Result: true
    jmp .L266
.L265:
    mov rax, 0          ; Result: false
.L266:
    test rax, rax
    jz .L263
    ; Then body
    call func_lexer_advance
    call func_lexer_advance
    mov rax, [rbp-32]
    push rax
    mov rax, str_126
    push rax
    mov rax, [state_TOKEN_GREATER_EQUAL]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L263:
    call func_lexer_advance

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_127
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L269
    ; Then body
    mov rax, [rbp-32]
    push rax
    mov rax, str_128
    push rax
    mov rax, [state_TOKEN_ASSIGN]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L269:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_129
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L271
    ; Then body
    mov rax, [rbp-32]
    push rax
    mov rax, str_130
    push rax
    mov rax, [state_TOKEN_PLUS]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L271:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_131
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L273
    ; Then body
    mov rax, [rbp-32]
    push rax
    mov rax, str_132
    push rax
    mov rax, [state_TOKEN_MINUS]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L273:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_133
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L275
    ; Then body
    mov rax, [rbp-32]
    push rax
    mov rax, str_134
    push rax
    mov rax, [state_TOKEN_MULTIPLY]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L275:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_135
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L277
    ; Then body
    mov rax, [rbp-32]
    push rax
    mov rax, str_136
    push rax
    mov rax, [state_TOKEN_DIVIDE]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L277:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_137
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L279
    ; Then body
    mov rax, [rbp-32]
    push rax
    mov rax, str_138
    push rax
    mov rax, [state_TOKEN_LPAREN]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L279:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_139
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L281
    ; Then body
    mov rax, [rbp-32]
    push rax
    mov rax, str_140
    push rax
    mov rax, [state_TOKEN_RPAREN]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L281:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_141
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L283
    ; Then body
    mov rax, [rbp-32]
    push rax
    mov rax, str_142
    push rax
    mov rax, [state_TOKEN_LBRACKET]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L283:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_143
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L285
    ; Then body
    mov rax, [rbp-32]
    push rax
    mov rax, str_144
    push rax
    mov rax, [state_TOKEN_RBRACKET]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L285:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_145
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L287
    ; Then body
    mov rax, [rbp-32]
    push rax
    mov rax, str_146
    push rax
    mov rax, [state_TOKEN_COMMA]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L287:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_147
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L289
    ; Then body
    mov rax, [rbp-32]
    push rax
    mov rax, str_148
    push rax
    mov rax, [state_TOKEN_DOT]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L289:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_149
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L291
    ; Then body
    mov rax, [rbp-32]
    push rax
    mov rax, str_150
    push rax
    mov rax, [state_TOKEN_COLON]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L291:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_151
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L293
    ; Then body
    mov rax, [rbp-32]
    push rax
    mov rax, str_152
    push rax
    mov rax, [state_TOKEN_LESS]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L293:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-24]
    mov rdi, rax
    push rdi
    mov rax, str_153
    mov rsi, rax
    pop rdi
    call string_equal
    test rax, rax
    jz .L295
    ; Then body
    mov rax, [rbp-32]
    push rax
    mov rax, str_154
    push rax
    mov rax, [state_TOKEN_GREATER]
    push rax
    call func_set_token
    add rsp, 24
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
.L295:
    mov rax, [rbp-32]
    push rax
    mov rax, str_155
    push rax
    mov rax, [state_TOKEN_EOF]
    push rax
    call func_set_token
    add rsp, 24
    mov rsp, rbp
    pop rbp
    ret

global func_token_type_name
func_token_type_name:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]   ; Load param ttype
    sub rsp, 8
    mov [rbp-8], rax   ; Store param ttype locally

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_EOF]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L297
    ; Then body
    mov rax, str_156
    mov rsp, rbp
    pop rbp
    ret
.L297:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_NUMERIC]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L299
    ; Then body
    mov rax, str_157
    mov rsp, rbp
    pop rbp
    ret
.L299:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_DECIMAL]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L301
    ; Then body
    mov rax, str_158
    mov rsp, rbp
    pop rbp
    ret
.L301:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_BOOLEAN]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L303
    ; Then body
    mov rax, str_159
    mov rsp, rbp
    pop rbp
    ret
.L303:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_TEXT]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L305
    ; Then body
    mov rax, str_160
    mov rsp, rbp
    pop rbp
    ret
.L305:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_IDENTIFIER]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L307
    ; Then body
    mov rax, str_161
    mov rsp, rbp
    pop rbp
    ret
.L307:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_NUMBER]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L309
    ; Then body
    mov rax, str_162
    mov rsp, rbp
    pop rbp
    ret
.L309:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_STRING]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L311
    ; Then body
    mov rax, str_163
    mov rsp, rbp
    pop rbp
    ret
.L311:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_ASSIGN]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L313
    ; Then body
    mov rax, str_164
    mov rsp, rbp
    pop rbp
    ret
.L313:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_PLUS]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L315
    ; Then body
    mov rax, str_165
    mov rsp, rbp
    pop rbp
    ret
.L315:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_MINUS]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L317
    ; Then body
    mov rax, str_166
    mov rsp, rbp
    pop rbp
    ret
.L317:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_MULTIPLY]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L319
    ; Then body
    mov rax, str_167
    mov rsp, rbp
    pop rbp
    ret
.L319:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_DIVIDE]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L321
    ; Then body
    mov rax, str_168
    mov rsp, rbp
    pop rbp
    ret
.L321:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_PRINT]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L323
    ; Then body
    mov rax, str_169
    mov rsp, rbp
    pop rbp
    ret
.L323:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_IF]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L325
    ; Then body
    mov rax, str_170
    mov rsp, rbp
    pop rbp
    ret
.L325:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_THEN]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L327
    ; Then body
    mov rax, str_171
    mov rsp, rbp
    pop rbp
    ret
.L327:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_ELSE]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L329
    ; Then body
    mov rax, str_172
    mov rsp, rbp
    pop rbp
    ret
.L329:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_END]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L331
    ; Then body
    mov rax, str_173
    mov rsp, rbp
    pop rbp
    ret
.L331:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_EQUAL]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L333
    ; Then body
    mov rax, str_174
    mov rsp, rbp
    pop rbp
    ret
.L333:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_LESS]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L335
    ; Then body
    mov rax, str_175
    mov rsp, rbp
    pop rbp
    ret
.L335:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_GREATER]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L337
    ; Then body
    mov rax, str_176
    mov rsp, rbp
    pop rbp
    ret
.L337:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_FOR]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L339
    ; Then body
    mov rax, str_177
    mov rsp, rbp
    pop rbp
    ret
.L339:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_TO]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L341
    ; Then body
    mov rax, str_178
    mov rsp, rbp
    pop rbp
    ret
.L341:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_WHILE]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L343
    ; Then body
    mov rax, str_179
    mov rsp, rbp
    pop rbp
    ret
.L343:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_FUNC]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L345
    ; Then body
    mov rax, str_180
    mov rsp, rbp
    pop rbp
    ret
.L345:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_RETURN]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L347
    ; Then body
    mov rax, str_181
    mov rsp, rbp
    pop rbp
    ret
.L347:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_STRUCT]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L349
    ; Then body
    mov rax, str_182
    mov rsp, rbp
    pop rbp
    ret
.L349:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_LPAREN]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L351
    ; Then body
    mov rax, str_183
    mov rsp, rbp
    pop rbp
    ret
.L351:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_RPAREN]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L353
    ; Then body
    mov rax, str_184
    mov rsp, rbp
    pop rbp
    ret
.L353:

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-8]
    push rax
    mov rax, [state_TOKEN_COMMA]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L355
    ; Then body
    mov rax, str_185
    mov rsp, rbp
    pop rbp
    ret
.L355:
    mov rax, str_186
    mov rsp, rbp
    pop rbp
    ret

_start:
    ; Get argc and argv from stack
    pop rdi               ; argc (first item on stack)
    mov rsi, rsp          ; argv (pointer to argv[0])
    call mlp_get_argv     ; Convert to MLP string array

    ; Initialize GC
    call gc_init

    push rbp
    mov rbp, rsp

    ; Initialize state variables
    mov rax, 0
    mov [state_TOKEN_EOF], rax
    mov rax, 1
    mov [state_TOKEN_NUMERIC], rax
    mov rax, 2
    mov [state_TOKEN_DECIMAL], rax
    mov rax, 3
    mov [state_TOKEN_BOOLEAN], rax
    mov rax, 4
    mov [state_TOKEN_TEXT], rax
    mov rax, 5
    mov [state_TOKEN_IDENTIFIER], rax
    mov rax, 6
    mov [state_TOKEN_NUMBER], rax
    mov rax, 7
    mov [state_TOKEN_STRING], rax
    mov rax, 8
    mov [state_TOKEN_ASSIGN], rax
    mov rax, 9
    mov [state_TOKEN_LPAREN], rax
    mov rax, 10
    mov [state_TOKEN_RPAREN], rax
    mov rax, 11
    mov [state_TOKEN_PLUS], rax
    mov rax, 12
    mov [state_TOKEN_MINUS], rax
    mov rax, 13
    mov [state_TOKEN_MULTIPLY], rax
    mov rax, 14
    mov [state_TOKEN_DIVIDE], rax
    mov rax, 15
    mov [state_TOKEN_PRINT], rax
    mov rax, 16
    mov [state_TOKEN_IF], rax
    mov rax, 17
    mov [state_TOKEN_THEN], rax
    mov rax, 18
    mov [state_TOKEN_ELSE], rax
    mov rax, 19
    mov [state_TOKEN_END], rax
    mov rax, 20
    mov [state_TOKEN_EQUAL], rax
    mov rax, 21
    mov [state_TOKEN_NOT_EQUAL], rax
    mov rax, 22
    mov [state_TOKEN_LESS], rax
    mov rax, 23
    mov [state_TOKEN_LESS_EQUAL], rax
    mov rax, 24
    mov [state_TOKEN_GREATER], rax
    mov rax, 25
    mov [state_TOKEN_GREATER_EQUAL], rax
    mov rax, 26
    mov [state_TOKEN_FOR], rax
    mov rax, 27
    mov [state_TOKEN_TO], rax
    mov rax, 28
    mov [state_TOKEN_STEP], rax
    mov rax, 29
    mov [state_TOKEN_WHILE], rax
    mov rax, 30
    mov [state_TOKEN_DO], rax
    mov rax, 31
    mov [state_TOKEN_AND], rax
    mov rax, 32
    mov [state_TOKEN_OR], rax
    mov rax, 33
    mov [state_TOKEN_NOT], rax
    mov rax, 34
    mov [state_TOKEN_FUNC], rax
    mov rax, 35
    mov [state_TOKEN_RETURN], rax
    mov rax, 36
    mov [state_TOKEN_COMMA], rax
    mov rax, 37
    mov [state_TOKEN_DOT], rax
    mov rax, 38
    mov [state_TOKEN_LBRACKET], rax
    mov rax, 39
    mov [state_TOKEN_RBRACKET], rax
    mov rax, 40
    mov [state_TOKEN_COLON], rax
    mov rax, 41
    mov [state_TOKEN_EXIT], rax
    mov rax, 42
    mov [state_TOKEN_CONTINUE], rax
    mov rax, 43
    mov [state_TOKEN_STRUCT], rax
    mov rax, 44
    mov [state_TOKEN_IMPORT], rax
    mov rax, 45
    mov [state_TOKEN_MODULE], rax
    mov rax, 46
    mov [state_TOKEN_EXPORT], rax
    mov rax, 47
    mov [state_TOKEN_RETURNS], rax
    mov rax, str_187
    mov [state_g_source], rax
    mov rax, 0
    mov [state_g_pos], rax
    mov rax, 1
    mov [state_g_line], rax
    mov rax, 0
    mov [state_g_length], rax
    mov rax, 0
    mov [state_g_tok_type], rax
    mov rax, str_188
    mov [state_g_tok_value], rax
    mov rax, 0
    mov [state_g_tok_line], rax


    ; Print statement
    mov rax, str_189
    mov rdi, rax
    call print_string
    ; Declaration: string test_code
    sub rsp, 8         ; Allocate space for test_code
    mov rax, str_190
    mov [rbp-8], rax   ; Initialize test_code

    ; Print statement
    mov rax, str_191
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-8]
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_192
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_193
    mov rdi, rax
    call print_string
    mov rax, [rbp-8]
    push rax
    call func_lexer_init
    add rsp, 8

    ; While loop
.L357:  ; while_start

    ; Evaluate comparison
    mov rax, 1
    push rax
    mov rax, 1
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L358
    ; While body
    call func_lexer_next_token
    ; Declaration: numeric tt
    sub rsp, 8         ; Allocate space for tt
    mov rax, [state_g_tok_type]
    mov [rbp-16], rax   ; Initialize tt
    ; Declaration: string tv
    sub rsp, 8         ; Allocate space for tv
    mov rax, [state_g_tok_value]
    mov [rbp-24], rax   ; Initialize tv
    ; Declaration: string type_name
    sub rsp, 8         ; Allocate space for type_name
    mov rax, [rbp-16]
    push rax
    call func_token_type_name
    add rsp, 8
    mov [rbp-32], rax   ; Initialize type_name
    ; Declaration: string output
    sub rsp, 8         ; Allocate space for output
    mov rax, str_194
    push rax
    mov rax, [rbp-32]
    mov rsi, rax
    pop rdi
    call string_concat
    push rax
    mov rax, str_195
    mov rsi, rax
    pop rdi
    call string_concat
    push rax
    mov rax, [rbp-24]
    mov rsi, rax
    pop rdi
    call string_concat
    push rax
    mov rax, str_196
    mov rsi, rax
    pop rdi
    call string_concat
    mov [rbp-40], rax   ; Initialize output

    ; Print statement
    mov rax, [rbp-40]
    mov rdi, rax
    call print_string

    ; If statement

    ; Evaluate comparison
    mov rax, [rbp-16]
    push rax
    mov rax, [state_TOKEN_EOF]
    mov rbx, rax
    pop rax
    cmp rax, rbx
    jne .L359
    ; Then body
    jmp .L358           ; exit
.L359:
    jmp .L357
.L358:  ; while_end

    ; Print statement
    mov rax, str_197
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_198
    mov rdi, rax
    call print_string

    ; Shutdown GC
    call gc_shutdown

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_198: db "=== Lexer Test Complete ===", 0
str_197: db "", 0
str_196: db "'", 0
str_195: db ", Value: '", 0
str_194: db "  Type: ", 0
str_193: db "Tokens:", 0
str_192: db "", 0
str_191: db "Input code:", 0
str_190: db "numeric x = 42", 10, "if x > 10 then", 10, "    print(x)", 10, "end if", 0
str_189: db "=== MLP Self-Hosting Lexer Test ===", 0
str_188: db "", 0
str_187: db "", 0
str_186: db "UNKNOWN", 0
str_185: db "COMMA", 0
str_184: db "RPAREN", 0
str_183: db "LPAREN", 0
str_182: db "STRUCT", 0
str_181: db "RETURN", 0
str_180: db "FUNC", 0
str_179: db "WHILE", 0
str_178: db "TO", 0
str_177: db "FOR", 0
str_176: db "GREATER", 0
str_175: db "LESS", 0
str_174: db "EQUAL", 0
str_173: db "END", 0
str_172: db "ELSE", 0
str_171: db "THEN", 0
str_170: db "IF", 0
str_169: db "PRINT", 0
str_168: db "DIVIDE", 0
str_167: db "MULTIPLY", 0
str_166: db "MINUS", 0
str_165: db "PLUS", 0
str_164: db "ASSIGN", 0
str_163: db "STRING", 0
str_162: db "NUMBER", 0
str_161: db "IDENTIFIER", 0
str_160: db "TEXT", 0
str_159: db "BOOLEAN", 0
str_158: db "DECIMAL", 0
str_157: db "NUMERIC", 0
str_156: db "EOF", 0
str_155: db "", 0
str_154: db ">", 0
str_153: db ">", 0
str_152: db "<", 0
str_151: db "<", 0
str_150: db ":", 0
str_149: db ":", 0
str_148: db ".", 0
str_147: db ".", 0
str_146: db ",", 0
str_145: db ",", 0
str_144: db "]", 0
str_143: db "]", 0
str_142: db "[", 0
str_141: db "[", 0
str_140: db ")", 0
str_139: db ")", 0
str_138: db "(", 0
str_137: db "(", 0
str_136: db "/", 0
str_135: db "/", 0
str_134: db "*", 0
str_133: db "*", 0
str_132: db "-", 0
str_131: db "-", 0
str_130: db "+", 0
str_129: db "+", 0
str_128: db "=", 0
str_127: db "=", 0
str_126: db ">=", 0
str_125: db "=", 0
str_124: db ">", 0
str_123: db "<=", 0
str_122: db "=", 0
str_121: db "<", 0
str_120: db "!=", 0
str_119: db "=", 0
str_118: db "!", 0
str_117: db "==", 0
str_116: db "=", 0
str_115: db "=", 0
str_114: db "", 34, "", 0
str_113: db "", 0
str_112: db "-", 0
str_111: db "-", 0
str_110: db "export", 0
str_109: db "module", 0
str_108: db "import", 0
str_107: db "struct", 0
str_106: db "continue", 0
str_105: db "exit", 0
str_104: db "returns", 0
str_103: db "return", 0
str_102: db "func", 0
str_101: db "not", 0
str_100: db "or", 0
str_99: db "and", 0
str_98: db "do", 0
str_97: db "while", 0
str_96: db "step", 0
str_95: db "to", 0
str_94: db "for", 0
str_93: db "end", 0
str_92: db "else", 0
str_91: db "then", 0
str_90: db "if", 0
str_89: db "print", 0
str_88: db "string", 0
str_87: db "boolean", 0
str_86: db "decimal", 0
str_85: db "numeric", 0
str_84: db "", 34, "", 0
str_83: db "", 34, "", 0
str_82: db "\\", 0
str_81: db "\\", 0
str_80: db "", 9, "", 0
str_79: db "t", 0
str_78: db "", 10, "", 0
str_77: db "n", 0
str_76: db "\\", 0
str_75: db "", 34, "", 0
str_74: db "", 0
str_73: db ".", 0
str_72: db "", 0
str_71: db "", 0
str_70: db "", 10, "", 0
str_69: db "", 10, "", 0
str_68: db "", 0
str_67: db "", 0
str_66: db "Z", 0
str_65: db "Y", 0
str_64: db "X", 0
str_63: db "W", 0
str_62: db "V", 0
str_61: db "U", 0
str_60: db "T", 0
str_59: db "S", 0
str_58: db "R", 0
str_57: db "Q", 0
str_56: db "P", 0
str_55: db "O", 0
str_54: db "N", 0
str_53: db "M", 0
str_52: db "L", 0
str_51: db "K", 0
str_50: db "J", 0
str_49: db "I", 0
str_48: db "H", 0
str_47: db "G", 0
str_46: db "F", 0
str_45: db "E", 0
str_44: db "D", 0
str_43: db "C", 0
str_42: db "B", 0
str_41: db "A", 0
str_40: db "z", 0
str_39: db "y", 0
str_38: db "x", 0
str_37: db "w", 0
str_36: db "v", 0
str_35: db "u", 0
str_34: db "t", 0
str_33: db "s", 0
str_32: db "r", 0
str_31: db "q", 0
str_30: db "p", 0
str_29: db "o", 0
str_28: db "n", 0
str_27: db "m", 0
str_26: db "l", 0
str_25: db "k", 0
str_24: db "j", 0
str_23: db "i", 0
str_22: db "h", 0
str_21: db "g", 0
str_20: db "f", 0
str_19: db "e", 0
str_18: db "d", 0
str_17: db "c", 0
str_16: db "b", 0
str_15: db "a", 0
str_14: db "_", 0
str_13: db "9", 0
str_12: db "8", 0
str_11: db "7", 0
str_10: db "6", 0
str_9: db "5", 0
str_8: db "4", 0
str_7: db "3", 0
str_6: db "2", 0
str_5: db "1", 0
str_4: db "0", 0
str_3: db "", 10, "", 0
str_2: db "r", 0
str_1: db "", 9, "", 0
str_0: db " ", 0

section .bss
    ; Global state variables
state_TOKEN_EOF: resq 1
state_TOKEN_NUMERIC: resq 1
state_TOKEN_DECIMAL: resq 1
state_TOKEN_BOOLEAN: resq 1
state_TOKEN_TEXT: resq 1
state_TOKEN_IDENTIFIER: resq 1
state_TOKEN_NUMBER: resq 1
state_TOKEN_STRING: resq 1
state_TOKEN_ASSIGN: resq 1
state_TOKEN_LPAREN: resq 1
state_TOKEN_RPAREN: resq 1
state_TOKEN_PLUS: resq 1
state_TOKEN_MINUS: resq 1
state_TOKEN_MULTIPLY: resq 1
state_TOKEN_DIVIDE: resq 1
state_TOKEN_PRINT: resq 1
state_TOKEN_IF: resq 1
state_TOKEN_THEN: resq 1
state_TOKEN_ELSE: resq 1
state_TOKEN_END: resq 1
state_TOKEN_EQUAL: resq 1
state_TOKEN_NOT_EQUAL: resq 1
state_TOKEN_LESS: resq 1
state_TOKEN_LESS_EQUAL: resq 1
state_TOKEN_GREATER: resq 1
state_TOKEN_GREATER_EQUAL: resq 1
state_TOKEN_FOR: resq 1
state_TOKEN_TO: resq 1
state_TOKEN_STEP: resq 1
state_TOKEN_WHILE: resq 1
state_TOKEN_DO: resq 1
state_TOKEN_AND: resq 1
state_TOKEN_OR: resq 1
state_TOKEN_NOT: resq 1
state_TOKEN_FUNC: resq 1
state_TOKEN_RETURN: resq 1
state_TOKEN_COMMA: resq 1
state_TOKEN_DOT: resq 1
state_TOKEN_LBRACKET: resq 1
state_TOKEN_RBRACKET: resq 1
state_TOKEN_COLON: resq 1
state_TOKEN_EXIT: resq 1
state_TOKEN_CONTINUE: resq 1
state_TOKEN_STRUCT: resq 1
state_TOKEN_IMPORT: resq 1
state_TOKEN_MODULE: resq 1
state_TOKEN_EXPORT: resq 1
state_TOKEN_RETURNS: resq 1
state_g_source: resq 1
state_g_pos: resq 1
state_g_line: resq 1
state_g_length: resq 1
state_g_tok_type: resq 1
state_g_tok_value: resq 1
state_g_tok_line: resq 1
