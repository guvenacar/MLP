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

    ; Declaration: numeric PARSE_TYPE_KEYWORD
    sub rsp, 8         ; Allocate space for PARSE_TYPE_KEYWORD
    mov rax, 0
    mov [rbp-8], rax   ; Initialize PARSE_TYPE_KEYWORD
    ; Declaration: numeric PARSE_TYPE_NAME
    sub rsp, 8         ; Allocate space for PARSE_TYPE_NAME
    mov rax, 1
    mov [rbp-16], rax   ; Initialize PARSE_TYPE_NAME
    ; Declaration: numeric PARSE_TYPE_BODY
    sub rsp, 8         ; Allocate space for PARSE_TYPE_BODY
    mov rax, 2
    mov [rbp-24], rax   ; Initialize PARSE_TYPE_BODY
    ; Declaration: numeric PARSE_TYPE_COMPLETE
    sub rsp, 8         ; Allocate space for PARSE_TYPE_COMPLETE
    mov rax, 3
    mov [rbp-32], rax   ; Initialize PARSE_TYPE_COMPLETE
    ; Declaration: numeric PARSE_FIELD_NAME
    sub rsp, 8         ; Allocate space for PARSE_FIELD_NAME
    mov rax, 10
    mov [rbp-40], rax   ; Initialize PARSE_FIELD_NAME
    ; Declaration: numeric PARSE_FIELD_TYPE
    sub rsp, 8         ; Allocate space for PARSE_FIELD_TYPE
    mov rax, 11
    mov [rbp-48], rax   ; Initialize PARSE_FIELD_TYPE
    ; Declaration: numeric PARSE_FIELD_DEFAULT
    sub rsp, 8         ; Allocate space for PARSE_FIELD_DEFAULT
    mov rax, 12
    mov [rbp-56], rax   ; Initialize PARSE_FIELD_DEFAULT
    ; Declaration: numeric PARSE_FIELD_ATTRS
    sub rsp, 8         ; Allocate space for PARSE_FIELD_ATTRS
    mov rax, 13
    mov [rbp-64], rax   ; Initialize PARSE_FIELD_ATTRS
    ; Declaration: numeric PARSE_FIELD_COMPLETE
    sub rsp, 8         ; Allocate space for PARSE_FIELD_COMPLETE
    mov rax, 14
    mov [rbp-72], rax   ; Initialize PARSE_FIELD_COMPLETE
    ; Declaration: numeric PARSE_METHOD_NAME
    sub rsp, 8         ; Allocate space for PARSE_METHOD_NAME
    mov rax, 20
    mov [rbp-80], rax   ; Initialize PARSE_METHOD_NAME
    ; Declaration: numeric PARSE_METHOD_PARAMS
    sub rsp, 8         ; Allocate space for PARSE_METHOD_PARAMS
    mov rax, 21
    mov [rbp-88], rax   ; Initialize PARSE_METHOD_PARAMS
    ; Declaration: numeric PARSE_METHOD_RETURN
    sub rsp, 8         ; Allocate space for PARSE_METHOD_RETURN
    mov rax, 22
    mov [rbp-96], rax   ; Initialize PARSE_METHOD_RETURN
    ; Declaration: numeric PARSE_METHOD_BODY
    sub rsp, 8         ; Allocate space for PARSE_METHOD_BODY
    mov rax, 23
    mov [rbp-104], rax   ; Initialize PARSE_METHOD_BODY
    ; Declaration: numeric PARSE_METHOD_ATTRS
    sub rsp, 8         ; Allocate space for PARSE_METHOD_ATTRS
    mov rax, 24
    mov [rbp-112], rax   ; Initialize PARSE_METHOD_ATTRS
    ; Declaration: numeric PARSE_METHOD_COMPLETE
    sub rsp, 8         ; Allocate space for PARSE_METHOD_COMPLETE
    mov rax, 25
    mov [rbp-120], rax   ; Initialize PARSE_METHOD_COMPLETE
    ; Declaration: numeric PARSE_ATTR_NAME
    sub rsp, 8         ; Allocate space for PARSE_ATTR_NAME
    mov rax, 30
    mov [rbp-128], rax   ; Initialize PARSE_ATTR_NAME
    ; Declaration: numeric PARSE_ATTR_ARGS
    sub rsp, 8         ; Allocate space for PARSE_ATTR_ARGS
    mov rax, 31
    mov [rbp-136], rax   ; Initialize PARSE_ATTR_ARGS
    ; Declaration: numeric PARSE_ATTR_COMPLETE
    sub rsp, 8         ; Allocate space for PARSE_ATTR_COMPLETE
    mov rax, 32
    mov [rbp-144], rax   ; Initialize PARSE_ATTR_COMPLETE
    ; Declaration: numeric PARSE_TYPEOF_KEYWORD
    sub rsp, 8         ; Allocate space for PARSE_TYPEOF_KEYWORD
    mov rax, 40
    mov [rbp-152], rax   ; Initialize PARSE_TYPEOF_KEYWORD
    ; Declaration: numeric PARSE_TYPEOF_LPAREN
    sub rsp, 8         ; Allocate space for PARSE_TYPEOF_LPAREN
    mov rax, 41
    mov [rbp-160], rax   ; Initialize PARSE_TYPEOF_LPAREN
    ; Declaration: numeric PARSE_TYPEOF_EXPR
    sub rsp, 8         ; Allocate space for PARSE_TYPEOF_EXPR
    mov rax, 42
    mov [rbp-168], rax   ; Initialize PARSE_TYPEOF_EXPR
    ; Declaration: numeric PARSE_TYPEOF_RPAREN
    sub rsp, 8         ; Allocate space for PARSE_TYPEOF_RPAREN
    mov rax, 43
    mov [rbp-176], rax   ; Initialize PARSE_TYPEOF_RPAREN
    ; Declaration: numeric PARSE_ASSERT_EXPR
    sub rsp, 8         ; Allocate space for PARSE_ASSERT_EXPR
    mov rax, 50
    mov [rbp-184], rax   ; Initialize PARSE_ASSERT_EXPR
    ; Declaration: numeric PARSE_ASSERT_AS
    sub rsp, 8         ; Allocate space for PARSE_ASSERT_AS
    mov rax, 51
    mov [rbp-192], rax   ; Initialize PARSE_ASSERT_AS
    ; Declaration: numeric PARSE_ASSERT_TYPE
    sub rsp, 8         ; Allocate space for PARSE_ASSERT_TYPE
    mov rax, 52
    mov [rbp-200], rax   ; Initialize PARSE_ASSERT_TYPE
    ; Declaration: numeric PARSE_ASSERT_COMPLETE
    sub rsp, 8         ; Allocate space for PARSE_ASSERT_COMPLETE
    mov rax, 53
    mov [rbp-208], rax   ; Initialize PARSE_ASSERT_COMPLETE
    ; Declaration: numeric PARSE_CHECK_EXPR
    sub rsp, 8         ; Allocate space for PARSE_CHECK_EXPR
    mov rax, 60
    mov [rbp-216], rax   ; Initialize PARSE_CHECK_EXPR
    ; Declaration: numeric PARSE_CHECK_IS
    sub rsp, 8         ; Allocate space for PARSE_CHECK_IS
    mov rax, 61
    mov [rbp-224], rax   ; Initialize PARSE_CHECK_IS
    ; Declaration: numeric PARSE_CHECK_TYPE
    sub rsp, 8         ; Allocate space for PARSE_CHECK_TYPE
    mov rax, 62
    mov [rbp-232], rax   ; Initialize PARSE_CHECK_TYPE
    ; Declaration: numeric PARSE_CHECK_COMPLETE
    sub rsp, 8         ; Allocate space for PARSE_CHECK_COMPLETE
    mov rax, 63
    mov [rbp-240], rax   ; Initialize PARSE_CHECK_COMPLETE
    ; Declaration: numeric PARSE_REFL_TARGET
    sub rsp, 8         ; Allocate space for PARSE_REFL_TARGET
    mov rax, 70
    mov [rbp-248], rax   ; Initialize PARSE_REFL_TARGET
    ; Declaration: numeric PARSE_REFL_DOT
    sub rsp, 8         ; Allocate space for PARSE_REFL_DOT
    mov rax, 71
    mov [rbp-256], rax   ; Initialize PARSE_REFL_DOT
    ; Declaration: numeric PARSE_REFL_FIELD
    sub rsp, 8         ; Allocate space for PARSE_REFL_FIELD
    mov rax, 72
    mov [rbp-264], rax   ; Initialize PARSE_REFL_FIELD
    ; Declaration: numeric PARSE_REFL_COMPLETE
    sub rsp, 8         ; Allocate space for PARSE_REFL_COMPLETE
    mov rax, 73
    mov [rbp-272], rax   ; Initialize PARSE_REFL_COMPLETE
    ; Declaration: numeric PARSE_INV_TARGET
    sub rsp, 8         ; Allocate space for PARSE_INV_TARGET
    mov rax, 80
    mov [rbp-280], rax   ; Initialize PARSE_INV_TARGET
    ; Declaration: numeric PARSE_INV_DOT
    sub rsp, 8         ; Allocate space for PARSE_INV_DOT
    mov rax, 81
    mov [rbp-288], rax   ; Initialize PARSE_INV_DOT
    ; Declaration: numeric PARSE_INV_METHOD
    sub rsp, 8         ; Allocate space for PARSE_INV_METHOD
    mov rax, 82
    mov [rbp-296], rax   ; Initialize PARSE_INV_METHOD
    ; Declaration: numeric PARSE_INV_LPAREN
    sub rsp, 8         ; Allocate space for PARSE_INV_LPAREN
    mov rax, 83
    mov [rbp-304], rax   ; Initialize PARSE_INV_LPAREN
    ; Declaration: numeric PARSE_INV_ARGS
    sub rsp, 8         ; Allocate space for PARSE_INV_ARGS
    mov rax, 84
    mov [rbp-312], rax   ; Initialize PARSE_INV_ARGS
    ; Declaration: numeric PARSE_INV_RPAREN
    sub rsp, 8         ; Allocate space for PARSE_INV_RPAREN
    mov rax, 85
    mov [rbp-320], rax   ; Initialize PARSE_INV_RPAREN
    ; Declaration: numeric PARSE_INV_COMPLETE
    sub rsp, 8         ; Allocate space for PARSE_INV_COMPLETE
    mov rax, 86
    mov [rbp-328], rax   ; Initialize PARSE_INV_COMPLETE
    ; Declaration: numeric PARSE_ATTRQ_TARGET
    sub rsp, 8         ; Allocate space for PARSE_ATTRQ_TARGET
    mov rax, 90
    mov [rbp-336], rax   ; Initialize PARSE_ATTRQ_TARGET
    ; Declaration: numeric PARSE_ATTRQ_AT
    sub rsp, 8         ; Allocate space for PARSE_ATTRQ_AT
    mov rax, 91
    mov [rbp-344], rax   ; Initialize PARSE_ATTRQ_AT
    ; Declaration: numeric PARSE_ATTRQ_NAME
    sub rsp, 8         ; Allocate space for PARSE_ATTRQ_NAME
    mov rax, 92
    mov [rbp-352], rax   ; Initialize PARSE_ATTRQ_NAME
    ; Declaration: numeric PARSE_ATTRQ_COMPLETE
    sub rsp, 8         ; Allocate space for PARSE_ATTRQ_COMPLETE
    mov rax, 93
    mov [rbp-360], rax   ; Initialize PARSE_ATTRQ_COMPLETE
    ; Declaration: numeric KW_TYPE
    sub rsp, 8         ; Allocate space for KW_TYPE
    mov rax, 100
    mov [rbp-368], rax   ; Initialize KW_TYPE
    ; Declaration: numeric KW_STRUCT
    sub rsp, 8         ; Allocate space for KW_STRUCT
    mov rax, 101
    mov [rbp-376], rax   ; Initialize KW_STRUCT
    ; Declaration: numeric KW_ENUM
    sub rsp, 8         ; Allocate space for KW_ENUM
    mov rax, 102
    mov [rbp-384], rax   ; Initialize KW_ENUM
    ; Declaration: numeric KW_INTERFACE
    sub rsp, 8         ; Allocate space for KW_INTERFACE
    mov rax, 103
    mov [rbp-392], rax   ; Initialize KW_INTERFACE
    ; Declaration: numeric KW_TYPEOF
    sub rsp, 8         ; Allocate space for KW_TYPEOF
    mov rax, 104
    mov [rbp-400], rax   ; Initialize KW_TYPEOF
    ; Declaration: numeric KW_AS
    sub rsp, 8         ; Allocate space for KW_AS
    mov rax, 105
    mov [rbp-408], rax   ; Initialize KW_AS
    ; Declaration: numeric KW_IS
    sub rsp, 8         ; Allocate space for KW_IS
    mov rax, 106
    mov [rbp-416], rax   ; Initialize KW_IS
    ; Declaration: numeric KW_REFLECT
    sub rsp, 8         ; Allocate space for KW_REFLECT
    mov rax, 107
    mov [rbp-424], rax   ; Initialize KW_REFLECT
    ; Declaration: numeric KW_PUBLIC
    sub rsp, 8         ; Allocate space for KW_PUBLIC
    mov rax, 110
    mov [rbp-432], rax   ; Initialize KW_PUBLIC
    ; Declaration: numeric KW_PRIVATE
    sub rsp, 8         ; Allocate space for KW_PRIVATE
    mov rax, 111
    mov [rbp-440], rax   ; Initialize KW_PRIVATE
    ; Declaration: numeric KW_PROTECTED
    sub rsp, 8         ; Allocate space for KW_PROTECTED
    mov rax, 112
    mov [rbp-448], rax   ; Initialize KW_PROTECTED
    ; Declaration: numeric KW_STATIC
    sub rsp, 8         ; Allocate space for KW_STATIC
    mov rax, 113
    mov [rbp-456], rax   ; Initialize KW_STATIC
    ; Declaration: numeric KW_VIRTUAL
    sub rsp, 8         ; Allocate space for KW_VIRTUAL
    mov rax, 114
    mov [rbp-464], rax   ; Initialize KW_VIRTUAL
    ; Declaration: numeric KW_ABSTRACT
    sub rsp, 8         ; Allocate space for KW_ABSTRACT
    mov rax, 115
    mov [rbp-472], rax   ; Initialize KW_ABSTRACT
    ; Declaration: numeric KW_READONLY
    sub rsp, 8         ; Allocate space for KW_READONLY
    mov rax, 116
    mov [rbp-480], rax   ; Initialize KW_READONLY
    ; Declaration: numeric SYNTAX_TYPE_DECL
    sub rsp, 8         ; Allocate space for SYNTAX_TYPE_DECL
    mov rax, 1
    mov [rbp-488], rax   ; Initialize SYNTAX_TYPE_DECL
    ; Declaration: numeric SYNTAX_ATTRIBUTE
    sub rsp, 8         ; Allocate space for SYNTAX_ATTRIBUTE
    mov rax, 1
    mov [rbp-496], rax   ; Initialize SYNTAX_ATTRIBUTE
    ; Declaration: numeric ERROR_INVALID_TYPE_NAME
    sub rsp, 8         ; Allocate space for ERROR_INVALID_TYPE_NAME
    mov rax, 1
    mov [rbp-504], rax   ; Initialize ERROR_INVALID_TYPE_NAME
    ; Declaration: numeric ERROR_MISSING_BODY
    sub rsp, 8         ; Allocate space for ERROR_MISSING_BODY
    mov rax, 2
    mov [rbp-512], rax   ; Initialize ERROR_MISSING_BODY
    ; Declaration: numeric ERROR_INVALID_FIELD
    sub rsp, 8         ; Allocate space for ERROR_INVALID_FIELD
    mov rax, 3
    mov [rbp-520], rax   ; Initialize ERROR_INVALID_FIELD
    ; Declaration: numeric ERROR_INVALID_METHOD
    sub rsp, 8         ; Allocate space for ERROR_INVALID_METHOD
    mov rax, 4
    mov [rbp-528], rax   ; Initialize ERROR_INVALID_METHOD
    ; Declaration: numeric ERROR_MISSING_AS
    sub rsp, 8         ; Allocate space for ERROR_MISSING_AS
    mov rax, 5
    mov [rbp-536], rax   ; Initialize ERROR_MISSING_AS
    ; Declaration: numeric ERROR_MISSING_IS
    sub rsp, 8         ; Allocate space for ERROR_MISSING_IS
    mov rax, 6
    mov [rbp-544], rax   ; Initialize ERROR_MISSING_IS
    ; Declaration: numeric ERROR_INVALID_ATTRIBUTE
    sub rsp, 8         ; Allocate space for ERROR_INVALID_ATTRIBUTE
    mov rax, 7
    mov [rbp-552], rax   ; Initialize ERROR_INVALID_ATTRIBUTE
    ; Declaration: numeric CTX_IN_TYPE_DECL
    sub rsp, 8         ; Allocate space for CTX_IN_TYPE_DECL
    mov rax, 200
    mov [rbp-560], rax   ; Initialize CTX_IN_TYPE_DECL
    ; Declaration: numeric CTX_IN_METHOD
    sub rsp, 8         ; Allocate space for CTX_IN_METHOD
    mov rax, 201
    mov [rbp-568], rax   ; Initialize CTX_IN_METHOD
    ; Declaration: numeric CTX_IN_ATTRIBUTE
    sub rsp, 8         ; Allocate space for CTX_IN_ATTRIBUTE
    mov rax, 202
    mov [rbp-576], rax   ; Initialize CTX_IN_ATTRIBUTE

    ; Shutdown GC
    call gc_shutdown

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
