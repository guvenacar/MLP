; MLP Variable Module - Generated Assembly
; Target: x86-64 Linux

    ; Variable: small_int (type: numeric)
    ; TTO: INT64 optimization

section .bss
    var_small_int: resq 1  ; 64-bit integer

section .text
    mov rax, 42
    mov [var_small_int], rax
    ; Variable: large_int (type: numeric)
    ; TTO: BIGDECIMAL (large number)

section .bss
    var_large_int: resq 2  ; 128-bit for big decimal
    ; Variable: float_val (type: numeric)
    ; TTO: DOUBLE optimization

section .data
    var_float_val: dq 3.14159  ; 64-bit float
    ; Variable: short_str (type: text)
    ; TTO: RODATA optimization (constant)
    var_short_str: db "Hi", 0  ; Read-only data
    ; Variable: medium_str (type: text)
    ; TTO: RODATA optimization (constant)
    var_medium_str: db "Hello World MLP", 0  ; Read-only data
    ; Variable: long_str (type: text)
    ; TTO: RODATA optimization (constant)
    var_long_str: db "This is a very long string that definitely exceeds twenty-three bytes", 0  ; Read-only data
    ; Variable: sum_int (type: numeric)
    ; TTO: INT64 optimization

section .bss
    var_sum_int: resq 1  ; 64-bit integer

section .text
    mov rax, 5
    mov [var_sum_int], rax
