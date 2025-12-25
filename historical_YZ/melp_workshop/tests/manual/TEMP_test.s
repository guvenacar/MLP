.section .rodata
.div_zero_msg:
    .string "Division by zero is not allowed!"

.text


# Function: main
.global main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $8, %rsp
    # Variable: result at -8(%rbp)
    movq $0, %r8  # Literal value
    movq %r8, -8(%rbp)  # Initialize result

    # For loop (desugared to while)
    movq $10, %r8  # Init loop var i
    movq %r8, -16(%rbp)
.for_start_0:
    movq -16(%rbp), %r8  # Load i
    movq $1, %r9  # End value
    cmp %r8, %r9
    movq $0, %rax
    setle %al  # end <= i
    test %rax, %rax
    jz .loop_end_0
    # Assignment: result = ...

    # Arithmetic expression
    movq -8(%rbp), %r8  # Load result
    movq -16(%rbp), %r9  # Load i
    addq %r9, %r8
    jo .overflow_detected_0
    jmp .no_overflow_0
.overflow_detected_0:
    # Overflow detected - promote to BigDecimal
    pushq %rdi
    pushq %rsi
    movq %r8, %rdi
    call sto_bigdec_from_int64
    movq %rax, %rdi # First BigDecimal
    movq %r9, %rsi
    call sto_bigdec_from_int64
    movq %rax, %rsi # Second BigDecimal
    call sto_bigdec_add
    popq %rsi
    popq %rdi
    movq %rax, %r8 # BigDecimal pointer in result register
.no_overflow_0:
    # Result in r8 (integer) or xmm0 (float)
    movq %r8, -8(%rbp)  # Store to result
    movq -16(%rbp), %r8  # Load i
    subq $1, %r8  # i--
    movq %r8, -16(%rbp)  # Store i
    jmp .for_start_0
.loop_end_0:
    movq -8(%rbp), %r8  # Load result
    movq %r8, %rax  # Return value
    jmp .Lmain_return

.Lmain_return:
    movq %rbp, %rsp
    popq %rbp
    ret
