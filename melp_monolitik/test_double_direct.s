; Direct double test
section .data
    fmt: db "Double: %.15g", 10, 0
    
section .text
global main
extern printf

main:
    push rbp
    mov rbp, rsp
    sub rsp, 16       ; Align stack to 16-byte boundary
    
    ; Load pi bit pattern directly
    mov rax, 0x400921f9f01b866e    ; 3.14159 as IEEE 754
    
    ; Move to xmm0 for double parameter
    movq xmm0, rax
    
    ; Set up printf call
    ; For printf with floating point: need to set eax = number of vector registers used
    mov rdi, fmt      ; format string
    mov eax, 1        ; 1 xmm register used (IMPORTANT for variadic functions!)
    
    call printf
    
    ; Clean up
    add rsp, 16
    pop rbp
    
    ; Return 0
    xor eax, eax
    ret
