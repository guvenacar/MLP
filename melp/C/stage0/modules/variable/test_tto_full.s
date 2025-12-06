; MLP Variable Module - Generated Assembly
; Target: x86-64 Linux

    ; Variable: small (type: numeric)
    ; TTO: INT64 optimization

section .bss
    var_small: resq 1  ; 64-bit integer

section .text
    mov rax, 5
    mov [var_small], rax
    ; Variable: large (type: numeric)
    ; TTO: BIGDECIMAL (large number)

section .bss
    var_large: resq 2  ; 128-bit for big decimal
    ; Variable: float_num (type: numeric)
    ; TTO: DOUBLE optimization

section .data
    var_float_num: dq 2.71828  ; 64-bit float
    ; Variable: short_str (type: text)
    ; TTO: RODATA optimization (constant)
    var_short_str: db "Hi", 0  ; Read-only data
    ; Variable: long_str (type: text)
    ; TTO: RODATA optimization (constant)
    var_long_str: db "This is a very long string that exceeds 23 bytes limit", 0  ; Read-only data
