section .data
    ; Iterator data section

section .bss
    iter_ptr resq 1      ; iterator pointer
    iter_idx resq 1      ; current index
    iter_len resq 1      ; length
    range_iter_ptr resq 1      ; iterator pointer
    range_iter_idx resq 1      ; current index
    range_iter_len resq 1      ; length
    result_array resq 1

section .text
    global main
    extern malloc, free, printf

main:
    push rbp
    mov rbp, rsp

; Iterator declaration: iter (type: 0)
    ; Array iterator for: numbers
    mov rax, [numbers]     ; load array pointer
    mov [iter_ptr], rax ; store in iterator
    mov qword [iter_idx], 0  ; initialize index

; Iterator declaration: range_iter (type: 1)
    ; Range iterator: 0..10
    ; Parse range bounds and create iterator
    mov qword [range_iter_current], 0
    mov qword [range_iter_end], 10

; Iterator operation: 3 on iter
    ; Map transformation: double
    ; Apply function to each element
.map_loop_iter:
    ; Get next element
    mov rax, [iter_ptr]
    mov rbx, [iter_idx]
    mov rdi, [rax + rbx*8]
    ; Call transform function
    call double
    ; Store result
    inc qword [iter_idx]

; Iterator operation: 4 on iter
    ; Filter with predicate: is_even
.filter_loop_iter:
    ; Get next element
    mov rax, [iter_ptr]
    mov rbx, [iter_idx]
    mov rdi, [rax + rbx*8]
    ; Call predicate function
    call is_even
    test rax, rax
    jz .filter_skip_iter
    ; Include element
.filter_skip_iter:
    inc qword [iter_idx]

; Iterator operation: 6 on iter
    ; Collect into collection
    ; Allocate result array
    mov rdi, [iter_len]
    shl rdi, 3           ; multiply by 8
    call malloc
    mov [result_array], rax

    ; Clean exit
    mov rax, 0
    mov rsp, rbp
    pop rbp
    ret
