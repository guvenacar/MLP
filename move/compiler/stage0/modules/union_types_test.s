section .data
    ; Union types data section

section .bss
    result resq 1    ; Result instance

section .text
    global main
    extern malloc, free, printf

; Union type definition: Result (kind: 2)
; Variants:
;   Ok (tag=0, type=i32)
;   Err (tag=1, type=String)
; Tag size: 4 bytes

; Union type definition: Option (kind: 2)
; Variants:
;   Some (tag=0, type=T)
;   None (tag=1, type=void)
; Tag size: 4 bytes

main:
    push rbp
    mov rbp, rsp

; Union instance: result of type Result
; Active variant: Ok
    ; Allocate union instance
    mov rdi, 16              ; tag (4 bytes) + data (max size)
    call malloc
    mov [result], rax            ; store pointer
    
    ; Set variant tag
    mov rbx, [result]
    mov dword [rbx], 0      ; write tag for Ok
    
    ; Store variant data
    mov qword [rbx+8], 42    ; store value

; Union operation: 2
    ; Pattern match on: result
    mov rax, [result]        ; load union pointer
    mov edx, [rax]       ; load tag
    
    ; Match arms
    jmp .match_end
    
.match_end:

    ; Clean exit
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret
