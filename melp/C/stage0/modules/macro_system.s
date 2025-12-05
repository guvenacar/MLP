; MLP Macro System - Generated Assembly
; Module #56: Procedural, Derive, Attribute Macros

section .data
    macro_msg db "Macro expanded", 10, 0

section .bss
    ; Macro expansion storage

section .text
    global _start

_start:
    ; Macro system initialized

    ; Declarative macros (macro_rules!):
    ; say_hello: 1 patterns

    ; Derive macro: Debug for Point
    ; Generated: func debug_Point() { print("Point") }

    ; Derive macro: Clone for Point
    ; Generated: func clone_Point() { -- clone implementation }

    ; Derive macro: Copy for Point
    ; Generated: -- Copy trait (bitwise copy)

    ; Attribute: #[test]
    ; Target: test_addition (function)
    ; Transform: -- Test function: test_addition

    ; Attribute: #[inline]
    ; Target: fast_function (function)
    ; Transform: -- Inline: fast_function

    ; Attribute: #[deprecated]
    ; Target: old_function (function)
    ; Transform: -- DEPRECATED: old_function ("Use new_function instead")

    ; Macro invocation: say_hello!()
    ; Type: function-like
    ; Expanded: Macro 'say_hello!()' not found or pattern mismatch

    ; Macro invocation: print!(__LINE__)
    ; Type: function-like
    ; Expanded: Macro 'print!(__LINE__)' not found or pattern mismatch

    ; Macro invocation: print!(__FILE__)
    ; Type: function-like
    ; Expanded: Macro 'print!(__FILE__)' not found or pattern mismatch

    ; Built-in macros:
    ; __LINE__ = 1
    ; __FILE__ = test.mlp
    ; __DATE__ = Dec  3 2025
    ; __TIME__ = 12:00:00

    ; Exit program
    mov rax, 60
    xor rdi, rdi
    syscall
