; Program Module - Generated code
section .data
    _prog_msg: db 'Program OK!',10,0
section .bss
section .text
    global _start

; Init routine
_prog_init:
    ret

; Cleanup routine
_prog_cleanup:
    ret

; Entry point
_start:
    call _prog_init
    ; Main program output
    mov rax, 1
    mov rdi, 1
    lea rsi, [_prog_msg]
    mov rdx, 12
    syscall
    call _prog_cleanup
    ; Exit
    mov rax, 60
    xor rdi, rdi
    syscall
