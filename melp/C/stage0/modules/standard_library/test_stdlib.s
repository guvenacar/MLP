; MLP Standard Library Module - Generated Assembly
; Target: x86-64 Linux

; Required libraries:
;   -lc (libc - standard C library)
;   -lm (libm - math library)

section .text
    global _start
_start:
    ; Stdlib functions will be linked by external linker
    ; Use: gcc -o program program.o -lc -lm

    mov rax, 60
    xor rdi, rdi
    syscall
