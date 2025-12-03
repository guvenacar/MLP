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

    ; Declaration: numeric REG_TYPE_INFO
    sub rsp, 8         ; Allocate space for REG_TYPE_INFO
    mov rax, 0
    mov [rbp-8], rax   ; Initialize REG_TYPE_INFO
    ; Declaration: numeric REG_FIELD_INFO
    sub rsp, 8         ; Allocate space for REG_FIELD_INFO
    mov rax, 1
    mov [rbp-16], rax   ; Initialize REG_FIELD_INFO
    ; Declaration: numeric REG_METHOD_INFO
    sub rsp, 8         ; Allocate space for REG_METHOD_INFO
    mov rax, 2
    mov [rbp-24], rax   ; Initialize REG_METHOD_INFO
    ; Declaration: numeric REG_INSTANCE
    sub rsp, 8         ; Allocate space for REG_INSTANCE
    mov rax, 3
    mov [rbp-32], rax   ; Initialize REG_INSTANCE
    ; Declaration: numeric REG_VALUE
    sub rsp, 8         ; Allocate space for REG_VALUE
    mov rax, 4
    mov [rbp-40], rax   ; Initialize REG_VALUE
    ; Declaration: numeric REG_REGISTRY
    sub rsp, 8         ; Allocate space for REG_REGISTRY
    mov rax, 5
    mov [rbp-48], rax   ; Initialize REG_REGISTRY
    ; Declaration: numeric STACK_TYPE_INFO
    sub rsp, 8         ; Allocate space for STACK_TYPE_INFO
    mov rax, 8
    mov [rbp-56], rax   ; Initialize STACK_TYPE_INFO
    ; Declaration: numeric STACK_FIELD_INFO
    sub rsp, 8         ; Allocate space for STACK_FIELD_INFO
    mov rax, 16
    mov [rbp-64], rax   ; Initialize STACK_FIELD_INFO
    ; Declaration: numeric STACK_METHOD_INFO
    sub rsp, 8         ; Allocate space for STACK_METHOD_INFO
    mov rax, 24
    mov [rbp-72], rax   ; Initialize STACK_METHOD_INFO
    ; Declaration: numeric STACK_INSTANCE
    sub rsp, 8         ; Allocate space for STACK_INSTANCE
    mov rax, 32
    mov [rbp-80], rax   ; Initialize STACK_INSTANCE
    ; Declaration: numeric STACK_ATTRIBUTES
    sub rsp, 8         ; Allocate space for STACK_ATTRIBUTES
    mov rax, 40
    mov [rbp-88], rax   ; Initialize STACK_ATTRIBUTES
    ; Declaration: numeric STACK_RESULT
    sub rsp, 8         ; Allocate space for STACK_RESULT
    mov rax, 48
    mov [rbp-96], rax   ; Initialize STACK_RESULT
    ; Declaration: numeric OFF_TYPE_NAME
    sub rsp, 8         ; Allocate space for OFF_TYPE_NAME
    mov rax, 0
    mov [rbp-104], rax   ; Initialize OFF_TYPE_NAME
    ; Declaration: numeric OFF_TYPE_CATEGORY
    sub rsp, 8         ; Allocate space for OFF_TYPE_CATEGORY
    mov rax, 8
    mov [rbp-112], rax   ; Initialize OFF_TYPE_CATEGORY
    ; Declaration: numeric OFF_TYPE_SIZE
    sub rsp, 8         ; Allocate space for OFF_TYPE_SIZE
    mov rax, 16
    mov [rbp-120], rax   ; Initialize OFF_TYPE_SIZE
    ; Declaration: numeric OFF_TYPE_ALIGNMENT
    sub rsp, 8         ; Allocate space for OFF_TYPE_ALIGNMENT
    mov rax, 24
    mov [rbp-128], rax   ; Initialize OFF_TYPE_ALIGNMENT
    ; Declaration: numeric OFF_TYPE_FLAGS
    sub rsp, 8         ; Allocate space for OFF_TYPE_FLAGS
    mov rax, 32
    mov [rbp-136], rax   ; Initialize OFF_TYPE_FLAGS
    ; Declaration: numeric OFF_TYPE_DATA
    sub rsp, 8         ; Allocate space for OFF_TYPE_DATA
    mov rax, 40
    mov [rbp-144], rax   ; Initialize OFF_TYPE_DATA
    ; Declaration: numeric OFF_FIELD_NAME
    sub rsp, 8         ; Allocate space for OFF_FIELD_NAME
    mov rax, 0
    mov [rbp-152], rax   ; Initialize OFF_FIELD_NAME
    ; Declaration: numeric OFF_FIELD_TYPE
    sub rsp, 8         ; Allocate space for OFF_FIELD_TYPE
    mov rax, 8
    mov [rbp-160], rax   ; Initialize OFF_FIELD_TYPE
    ; Declaration: numeric OFF_FIELD_OFFSET
    sub rsp, 8         ; Allocate space for OFF_FIELD_OFFSET
    mov rax, 16
    mov [rbp-168], rax   ; Initialize OFF_FIELD_OFFSET
    ; Declaration: numeric OFF_FIELD_SIZE
    sub rsp, 8         ; Allocate space for OFF_FIELD_SIZE
    mov rax, 24
    mov [rbp-176], rax   ; Initialize OFF_FIELD_SIZE
    ; Declaration: numeric OFF_FIELD_FLAGS
    sub rsp, 8         ; Allocate space for OFF_FIELD_FLAGS
    mov rax, 32
    mov [rbp-184], rax   ; Initialize OFF_FIELD_FLAGS
    ; Declaration: numeric OFF_METHOD_NAME
    sub rsp, 8         ; Allocate space for OFF_METHOD_NAME
    mov rax, 0
    mov [rbp-192], rax   ; Initialize OFF_METHOD_NAME
    ; Declaration: numeric OFF_METHOD_RETURN
    sub rsp, 8         ; Allocate space for OFF_METHOD_RETURN
    mov rax, 8
    mov [rbp-200], rax   ; Initialize OFF_METHOD_RETURN
    ; Declaration: numeric OFF_METHOD_PARAMS
    sub rsp, 8         ; Allocate space for OFF_METHOD_PARAMS
    mov rax, 16
    mov [rbp-208], rax   ; Initialize OFF_METHOD_PARAMS
    ; Declaration: numeric OFF_METHOD_COUNT
    sub rsp, 8         ; Allocate space for OFF_METHOD_COUNT
    mov rax, 24
    mov [rbp-216], rax   ; Initialize OFF_METHOD_COUNT
    ; Declaration: numeric OFF_METHOD_PTR
    sub rsp, 8         ; Allocate space for OFF_METHOD_PTR
    mov rax, 32
    mov [rbp-224], rax   ; Initialize OFF_METHOD_PTR
    ; Declaration: numeric LABEL_TYPE
    sub rsp, 8         ; Allocate space for LABEL_TYPE
    mov rax, 0
    mov [rbp-232], rax   ; Initialize LABEL_TYPE
    ; Declaration: numeric LABEL_FIELD
    sub rsp, 8         ; Allocate space for LABEL_FIELD
    mov rax, 1
    mov [rbp-240], rax   ; Initialize LABEL_FIELD
    ; Declaration: numeric LABEL_METHOD
    sub rsp, 8         ; Allocate space for LABEL_METHOD
    mov rax, 2
    mov [rbp-248], rax   ; Initialize LABEL_METHOD
    ; Declaration: numeric LABEL_ATTRIBUTE
    sub rsp, 8         ; Allocate space for LABEL_ATTRIBUTE
    mov rax, 3
    mov [rbp-256], rax   ; Initialize LABEL_ATTRIBUTE
    ; Declaration: numeric LABEL_REGISTRY
    sub rsp, 8         ; Allocate space for LABEL_REGISTRY
    mov rax, 4
    mov [rbp-264], rax   ; Initialize LABEL_REGISTRY
    ; Declaration: numeric LABEL_TYPEOF
    sub rsp, 8         ; Allocate space for LABEL_TYPEOF
    mov rax, 5
    mov [rbp-272], rax   ; Initialize LABEL_TYPEOF
    ; Declaration: numeric LABEL_CAST
    sub rsp, 8         ; Allocate space for LABEL_CAST
    mov rax, 6
    mov [rbp-280], rax   ; Initialize LABEL_CAST
    ; Declaration: numeric LABEL_CHECK
    sub rsp, 8         ; Allocate space for LABEL_CHECK
    mov rax, 7
    mov [rbp-288], rax   ; Initialize LABEL_CHECK
    ; Declaration: numeric DATA_TYPE_TABLE
    sub rsp, 8         ; Allocate space for DATA_TYPE_TABLE
    mov rax, 0
    mov [rbp-296], rax   ; Initialize DATA_TYPE_TABLE
    ; Declaration: numeric DATA_FIELD_TABLE
    sub rsp, 8         ; Allocate space for DATA_FIELD_TABLE
    mov rax, 1
    mov [rbp-304], rax   ; Initialize DATA_FIELD_TABLE
    ; Declaration: numeric DATA_METHOD_TABLE
    sub rsp, 8         ; Allocate space for DATA_METHOD_TABLE
    mov rax, 2
    mov [rbp-312], rax   ; Initialize DATA_METHOD_TABLE
    ; Declaration: numeric DATA_ATTRIBUTE_TABLE
    sub rsp, 8         ; Allocate space for DATA_ATTRIBUTE_TABLE
    mov rax, 3
    mov [rbp-320], rax   ; Initialize DATA_ATTRIBUTE_TABLE
    ; Declaration: numeric DATA_REGISTRY
    sub rsp, 8         ; Allocate space for DATA_REGISTRY
    mov rax, 4
    mov [rbp-328], rax   ; Initialize DATA_REGISTRY
    ; Declaration: numeric OP_TYPEOF
    sub rsp, 8         ; Allocate space for OP_TYPEOF
    mov rax, 0
    mov [rbp-336], rax   ; Initialize OP_TYPEOF
    ; Declaration: numeric OP_TYPE_ASSERT
    sub rsp, 8         ; Allocate space for OP_TYPE_ASSERT
    mov rax, 1
    mov [rbp-344], rax   ; Initialize OP_TYPE_ASSERT
    ; Declaration: numeric OP_TYPE_CHECK
    sub rsp, 8         ; Allocate space for OP_TYPE_CHECK
    mov rax, 2
    mov [rbp-352], rax   ; Initialize OP_TYPE_CHECK
    ; Declaration: numeric OP_GET_FIELD
    sub rsp, 8         ; Allocate space for OP_GET_FIELD
    mov rax, 3
    mov [rbp-360], rax   ; Initialize OP_GET_FIELD
    ; Declaration: numeric OP_SET_FIELD
    sub rsp, 8         ; Allocate space for OP_SET_FIELD
    mov rax, 4
    mov [rbp-368], rax   ; Initialize OP_SET_FIELD
    ; Declaration: numeric OP_INVOKE_METHOD
    sub rsp, 8         ; Allocate space for OP_INVOKE_METHOD
    mov rax, 5
    mov [rbp-376], rax   ; Initialize OP_INVOKE_METHOD
    ; Declaration: numeric CAST_DIRECT
    sub rsp, 8         ; Allocate space for CAST_DIRECT
    mov rax, 0
    mov [rbp-384], rax   ; Initialize CAST_DIRECT
    ; Declaration: numeric CAST_WITH_CHECK
    sub rsp, 8         ; Allocate space for CAST_WITH_CHECK
    mov rax, 1
    mov [rbp-392], rax   ; Initialize CAST_WITH_CHECK
    ; Declaration: numeric CAST_DYNAMIC
    sub rsp, 8         ; Allocate space for CAST_DYNAMIC
    mov rax, 2
    mov [rbp-400], rax   ; Initialize CAST_DYNAMIC
    ; Declaration: numeric ACCESS_DIRECT
    sub rsp, 8         ; Allocate space for ACCESS_DIRECT
    mov rax, 0
    mov [rbp-408], rax   ; Initialize ACCESS_DIRECT
    ; Declaration: numeric ACCESS_REFLECTION
    sub rsp, 8         ; Allocate space for ACCESS_REFLECTION
    mov rax, 1
    mov [rbp-416], rax   ; Initialize ACCESS_REFLECTION
    ; Declaration: numeric ACCESS_VIRTUAL
    sub rsp, 8         ; Allocate space for ACCESS_VIRTUAL
    mov rax, 2
    mov [rbp-424], rax   ; Initialize ACCESS_VIRTUAL
    ; Declaration: numeric INVOKE_DIRECT
    sub rsp, 8         ; Allocate space for INVOKE_DIRECT
    mov rax, 0
    mov [rbp-432], rax   ; Initialize INVOKE_DIRECT
    ; Declaration: numeric INVOKE_REFLECTION
    sub rsp, 8         ; Allocate space for INVOKE_REFLECTION
    mov rax, 1
    mov [rbp-440], rax   ; Initialize INVOKE_REFLECTION
    ; Declaration: numeric INVOKE_VIRTUAL
    sub rsp, 8         ; Allocate space for INVOKE_VIRTUAL
    mov rax, 2
    mov [rbp-448], rax   ; Initialize INVOKE_VIRTUAL
    ; Declaration: numeric FN_TYPE_INFO_CREATE
    sub rsp, 8         ; Allocate space for FN_TYPE_INFO_CREATE
    mov rax, 0
    mov [rbp-456], rax   ; Initialize FN_TYPE_INFO_CREATE
    ; Declaration: numeric FN_TYPE_INFO_GET
    sub rsp, 8         ; Allocate space for FN_TYPE_INFO_GET
    mov rax, 1
    mov [rbp-464], rax   ; Initialize FN_TYPE_INFO_GET
    ; Declaration: numeric FN_FIELD_GET
    sub rsp, 8         ; Allocate space for FN_FIELD_GET
    mov rax, 2
    mov [rbp-472], rax   ; Initialize FN_FIELD_GET
    ; Declaration: numeric FN_FIELD_GET_VAL
    sub rsp, 8         ; Allocate space for FN_FIELD_GET_VAL
    mov rax, 3
    mov [rbp-480], rax   ; Initialize FN_FIELD_GET_VAL
    ; Declaration: numeric FN_FIELD_SET_VAL
    sub rsp, 8         ; Allocate space for FN_FIELD_SET_VAL
    mov rax, 4
    mov [rbp-488], rax   ; Initialize FN_FIELD_SET_VAL
    ; Declaration: numeric FN_METHOD_GET
    sub rsp, 8         ; Allocate space for FN_METHOD_GET
    mov rax, 5
    mov [rbp-496], rax   ; Initialize FN_METHOD_GET
    ; Declaration: numeric FN_METHOD_INVOKE
    sub rsp, 8         ; Allocate space for FN_METHOD_INVOKE
    mov rax, 6
    mov [rbp-504], rax   ; Initialize FN_METHOD_INVOKE
    ; Declaration: numeric FN_INSTANCE_TYPE
    sub rsp, 8         ; Allocate space for FN_INSTANCE_TYPE
    mov rax, 7
    mov [rbp-512], rax   ; Initialize FN_INSTANCE_TYPE
    ; Declaration: numeric FN_INSTANCE_CLONE
    sub rsp, 8         ; Allocate space for FN_INSTANCE_CLONE
    mov rax, 8
    mov [rbp-520], rax   ; Initialize FN_INSTANCE_CLONE
    ; Declaration: numeric FN_INSTANCE_EQUALS
    sub rsp, 8         ; Allocate space for FN_INSTANCE_EQUALS
    mov rax, 9
    mov [rbp-528], rax   ; Initialize FN_INSTANCE_EQUALS
    ; Declaration: numeric FN_INSTANCE_HASH
    sub rsp, 8         ; Allocate space for FN_INSTANCE_HASH
    mov rax, 10
    mov [rbp-536], rax   ; Initialize FN_INSTANCE_HASH
    ; Declaration: numeric FN_TYPE_CAST
    sub rsp, 8         ; Allocate space for FN_TYPE_CAST
    mov rax, 11
    mov [rbp-544], rax   ; Initialize FN_TYPE_CAST
    ; Declaration: numeric FN_TYPE_COMPAT
    sub rsp, 8         ; Allocate space for FN_TYPE_COMPAT
    mov rax, 12
    mov [rbp-552], rax   ; Initialize FN_TYPE_COMPAT
    ; Declaration: numeric FN_REGISTRY_REGISTER
    sub rsp, 8         ; Allocate space for FN_REGISTRY_REGISTER
    mov rax, 13
    mov [rbp-560], rax   ; Initialize FN_REGISTRY_REGISTER
    ; Declaration: numeric FN_REGISTRY_LOOKUP
    sub rsp, 8         ; Allocate space for FN_REGISTRY_LOOKUP
    mov rax, 14
    mov [rbp-568], rax   ; Initialize FN_REGISTRY_LOOKUP
    ; Declaration: numeric META_GENERATE_INFO
    sub rsp, 8         ; Allocate space for META_GENERATE_INFO
    mov rax, 0
    mov [rbp-576], rax   ; Initialize META_GENERATE_INFO
    ; Declaration: numeric META_GENERATE_FIELDS
    sub rsp, 8         ; Allocate space for META_GENERATE_FIELDS
    mov rax, 1
    mov [rbp-584], rax   ; Initialize META_GENERATE_FIELDS
    ; Declaration: numeric META_GENERATE_METHODS
    sub rsp, 8         ; Allocate space for META_GENERATE_METHODS
    mov rax, 2
    mov [rbp-592], rax   ; Initialize META_GENERATE_METHODS
    ; Declaration: numeric META_GENERATE_ATTRS
    sub rsp, 8         ; Allocate space for META_GENERATE_ATTRS
    mov rax, 3
    mov [rbp-600], rax   ; Initialize META_GENERATE_ATTRS
    ; Declaration: numeric RT_INIT_REGISTRY
    sub rsp, 8         ; Allocate space for RT_INIT_REGISTRY
    mov rax, 0
    mov [rbp-608], rax   ; Initialize RT_INIT_REGISTRY
    ; Declaration: numeric RT_REGISTER_TYPES
    sub rsp, 8         ; Allocate space for RT_REGISTER_TYPES
    mov rax, 1
    mov [rbp-616], rax   ; Initialize RT_REGISTER_TYPES
    ; Declaration: numeric RT_BUILD_VTABLES
    sub rsp, 8         ; Allocate space for RT_BUILD_VTABLES
    mov rax, 2
    mov [rbp-624], rax   ; Initialize RT_BUILD_VTABLES
    ; Declaration: numeric OPT_CACHE_TYPEINFO
    sub rsp, 8         ; Allocate space for OPT_CACHE_TYPEINFO
    mov rax, 1
    mov [rbp-632], rax   ; Initialize OPT_CACHE_TYPEINFO
    ; Declaration: numeric OPT_INLINE_FIELD_ACCESS
    sub rsp, 8         ; Allocate space for OPT_INLINE_FIELD_ACCESS
    mov rax, 1
    mov [rbp-640], rax   ; Initialize OPT_INLINE_FIELD_ACCESS
    ; Declaration: numeric OPT_ELIMINATE_CHECKS
    sub rsp, 8         ; Allocate space for OPT_ELIMINATE_CHECKS
    mov rax, 1
    mov [rbp-648], rax   ; Initialize OPT_ELIMINATE_CHECKS
    ; Declaration: numeric OPT_DEVIRTUALIZE
    sub rsp, 8         ; Allocate space for OPT_DEVIRTUALIZE
    mov rax, 1
    mov [rbp-656], rax   ; Initialize OPT_DEVIRTUALIZE

    ; Shutdown GC
    call gc_shutdown

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
