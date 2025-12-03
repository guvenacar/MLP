; MLP Result/Option Module Assembly Output
; Generated from result_option_codegen.c

section .data
    msg_ok: db "Result/Option OK!", 10, 0
    msg_ok_len: equ $ - msg_ok

    ; Result types: 2
    ; Result r: Ok<T> | Err<E>
    ; Result e: Ok<T> | Err<E>

    ; Option types: 2
    ; Option o: Some<T> | None
    ; Option n: Some<T> | None

    ; Ok constructors: 1
    ; r = Ok(42)

    ; Err constructors: 1
    ; e = Err(error)

    ; Some constructors: 1
    ; o = Some(hello)

    ; None constructors: 1

    ; Operations:
    ;   unwrap: 1
    ;   expect: 1
    ;   unwrap_or: 1
    ;   map: 1
    ;   and_then: 1
    ;   or_else: 1
    ;   ?: 1
    ;   is_ok: 1
    ;   is_err: 1
    ;   is_some: 1
    ;   is_none: 1

section .bss
    ; Runtime storage for Result/Option values
    result_r: resq 2    ; [tag, value/error]
    result_e: resq 2    ; [tag, value/error]
    option_o: resq 2    ; [tag, value]
    option_n: resq 2    ; [tag, value]

section .text
    global _start

_start:
    ; Result/Option operations
    ; Op 0: Result<numeric, text> r
    ; Op 1: r = Ok(42)
    ; Op 2: Result<numeric, text> e
    ; Op 3: e = Err(error)
    ; Op 4: Option<text> o
    ; Op 5: o = Some(hello)
    ; Op 6: Option<numeric> n
    ; Op 7: n = None
    ; Op 8: r.unwrap()
    ; Op 9: o.expect(msg)
    ; Op 10: r.unwrap_or(0)
    ; Op 11: unknown
    ; Op 12: r.map(| x | x * 2)
    ; Op 13: o.and_then(| s | Some ( s . len ()
    ; Op 14: e.or_else(| _ | Ok ( 99)
    ; Op 15: r.is_ok()
    ; Op 16: e.is_err()
    ; Op 17: o.is_some()
    ; Op 18: n.is_none()
    ; Op 19: value?

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
