; MLP Attributes Module Assembly Output
; Generated from attributes_codegen.c

section .data
    msg_ok: db "Attributes OK!", 10, 0
    msg_ok_len: equ $ - msg_ok

    ; Total attributes: 17

    ; Optimization attributes:
    ;   @inline: 1
    ;   @hot: 1
    ;   @cold: 1

    ; Testing attributes:
    ;   @test: 1
    ;   @bench: 1

    ; Other attributes:
    ;   @deprecated: 1
    ;   @derive: 1
    ;   @serialize: 1
    ;   @deserialize: 1
    ;   @validate: 3
    ;   lints: 1
    ;   @doc: 1
    ;   @cfg: 1
    ;   @repr: 1

    ; Attribute details:
    ;   [0] @inline
    ;   [1] @hot
    ;   [2] @cold -> fast_function
    ;   [3] @deprecated(use new_function instead) -> fast_function
    ;   [4] @test -> old_function
    ;   [5] @bench -> test_example
    ;   [6] @derive(Debug, Clone, Copy) -> benchmark_sort
    ;   [7] @serialize -> Point
    ;   [8] @deserialize -> Point
    ;   [9] @validate -> User
    ;   [10] @required -> User
    ;   [11] @range(0, 100) -> User
    ;   [12] @allow(unused_variables) -> User
    ;   [13] @doc(This is a documented function) -> experimental
    ;   [14] @cfg(target_os, =, linux) -> documented
    ;   [15] @repr(C) -> linux_only
    ;   [16] @must_use -> FFI_Struct

section .bss
    ; Runtime storage for attribute metadata
    attr_flags: resb 64

section .text
    global _start

_start:
    ; Process attributes
    ; Attribute 0: @inline
    ;   -> inline optimization hint
    ; Attribute 1: @hot
    ;   -> hot path optimization
    ; Attribute 2: @cold
    ;   -> cold path optimization
    ; Attribute 3: @deprecated(use new_function instead)
    ;   -> deprecation warning
    ; Attribute 4: @test
    ;   -> mark as test function
    ; Attribute 5: @bench
    ;   -> mark as benchmark
    ; Attribute 6: @derive(Debug, Clone, Copy)
    ;   -> derive macro: Debug, Clone, Copy
    ; Attribute 7: @serialize
    ;   -> serialization support
    ; Attribute 8: @deserialize
    ;   -> deserialization support
    ; Attribute 9: @validate
    ;   -> validation rule
    ; Attribute 10: @required
    ;   -> generic attribute
    ; Attribute 11: @range(0, 100)
    ;   -> generic attribute
    ; Attribute 12: @allow(unused_variables)
    ;   -> lint rule: unused_variables
    ; Attribute 13: @doc(This is a documented function)
    ;   -> documentation: This is a documented function
    ; Attribute 14: @cfg(target_os, =, linux)
    ;   -> conditional compilation: target_os, =, linux
    ; Attribute 15: @repr(C)
    ;   -> memory layout: C
    ; Attribute 16: @must_use
    ;   -> generic attribute

    ; Print success message
    mov rax, 1              ; sys_write
    mov rdi, 1              ; stdout
    mov rsi, msg_ok
    mov rdx, msg_ok_len
    syscall

    ; Exit
    mov rax, 60             ; sys_exit
    xor rdi, rdi            ; exit code 0
    syscall
