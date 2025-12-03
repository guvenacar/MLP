.section .rodata
msg_router: .string "=== MELP Compiler Router ===\n"
msg_input: .string "Input: "
msg_output: .string "Output: "
msg_lexer: .string "Running lexer on: "
msg_parser: .string "Running parser on tokens\n"
msg_memory: .string "Running memory analysis\n"
msg_codegen: .string "Running codegen to: "
msg_success: .string "\n=== Compilation successful ===\n"
msg_newline: .string "\n"
test_file: .string "test_router.mlp"
output_file: .string "test_router.s"

.section .text
.global main

print_str:
    push %rbp
    mov %rsp, %rbp
    push %rdi
    # Calculate string length
    mov %rdi, %rsi
    xor %rdx, %rdx
.strlen_loop:
    cmpb $0, (%rsi,%rdx,1)
    je .strlen_done
    inc %rdx
    jmp .strlen_loop
.strlen_done:
    # write(1, str, len)
    mov $1, %rax      # sys_write
    mov $1, %rdi      # stdout
    pop %rsi          # string
    # rdx already has length
    syscall
    pop %rbp
    ret

run_lexer:
    push %rbp
    mov %rsp, %rbp
    push %rbx
    mov %rdi, %rbx
    lea msg_lexer(%rip), %rdi
    call print_str
    mov %rbx, %rdi
    call print_str
    mov $10, %rax  # Return 10 tokens
    pop %rbx
    pop %rbp
    ret

run_parser:
    push %rbp
    mov %rsp, %rbp
    lea msg_parser(%rip), %rdi
    call print_str
    mov $5, %rax  # Return 5 nodes
    pop %rbp
    ret

run_memory:
    push %rbp
    mov %rsp, %rbp
    lea msg_memory(%rip), %rdi
    call print_str
    mov $3, %rax  # Return 3 lifetimes
    pop %rbp
    ret

run_codegen:
    push %rbp
    mov %rsp, %rbp
    push %r12
    mov %rdx, %r12
    lea msg_codegen(%rip), %rdi
    call print_str
    mov %r12, %rdi
    call print_str
    xor %rax, %rax  # Return 0 (success)
    pop %r12
    pop %rbp
    ret

compile_file:
    push %rbp
    mov %rsp, %rbp
    push %rbx
    push %r12
    push %r13
    mov %rdi, %rbx  # input_file
    mov %rsi, %r12  # output_file
    
    # Print header
    lea msg_router(%rip), %rdi
    call print_str
    lea msg_input(%rip), %rdi
    call print_str
    mov %rbx, %rdi
    call print_str
    lea msg_output(%rip), %rdi
    call print_str
    mov %r12, %rdi
    call print_str
    lea msg_newline(%rip), %rdi
    call print_str
    
    # Run lexer
    mov %rbx, %rdi
    call run_lexer
    mov %rax, %r13  # Save token count
    
    # Run parser
    mov %r13, %rdi
    call run_parser
    mov %rax, %r13  # Save AST node count
    
    # Run memory
    mov %r13, %rdi
    call run_memory
    
    # Run codegen
    mov %r13, %rdi
    mov %rax, %rsi
    mov %r12, %rdx
    call run_codegen
    
    # Print success
    lea msg_success(%rip), %rdi
    call print_str
    
    xor %rax, %rax  # Return 0
    pop %r13
    pop %r12
    pop %rbx
    pop %rbp
    ret

main:
    push %rbp
    mov %rsp, %rbp
    lea test_file(%rip), %rdi
    lea output_file(%rip), %rsi
    call compile_file
    pop %rbp
    ret
