#include "function_codegen.h"
#include <stdio.h>

// Generate function prologue (System V AMD64 ABI)
void codegen_function_prologue(FILE* output, Function* func) {
    fprintf(output, "\n; Function: %s\n", func->name);
    fprintf(output, "%s:\n", func->name);
    
    // Setup stack frame
    fprintf(output, "    push rbp\n");
    fprintf(output, "    mov rbp, rsp\n");
    
    // Allocate local variables space (calculated based on body analysis)
    // TODO: Calculate actual stack space needed
    fprintf(output, "    sub rsp, 64  ; Reserve space for locals\n");
    
    // Save parameter registers to stack
    // System V AMD64 ABI: rdi, rsi, rdx, rcx, r8, r9 for first 6 integer args
    if (func->param_count > 0) {
        fprintf(output, "    ; Save parameters to stack\n");
        const char* param_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
        for (int i = 0; i < func->param_count && i < 6; i++) {
            fprintf(output, "    mov [rbp-%d], %s  ; %s\n", 
                    (i+1)*8, param_regs[i], func->parameters[i]->name);
        }
    }
}

// Generate function epilogue
void codegen_function_epilogue(FILE* output, Function* func) {
    fprintf(output, "    ; Function epilogue\n");
    fprintf(output, "    mov rsp, rbp\n");
    fprintf(output, "    pop rbp\n");
    fprintf(output, "    ret\n");
}

// Generate function body
void codegen_function_body(FILE* output, Function* func) {
    fprintf(output, "    ; Function body\n");
    // TODO: Generate code for each statement in body
    // This requires full statement codegen infrastructure
    
    if (!func->has_return) {
        // Implicit return for void functions
        fprintf(output, "    xor rax, rax  ; Return 0 by default\n");
    }
}

// Generate function call
void codegen_function_call(FILE* output, FunctionCall* call) {
    fprintf(output, "    ; Call function: %s\n", call->function_name);
    
    // Evaluate arguments and place in registers
    // System V AMD64 ABI: rdi, rsi, rdx, rcx, r8, r9
    const char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    
    for (int i = 0; i < call->argument_count && i < 6; i++) {
        fprintf(output, "    ; TODO: Evaluate argument %d\n", i);
        fprintf(output, "    mov %s, rax  ; Move result to arg register\n", arg_regs[i]);
    }
    
    // Handle stack arguments (beyond 6th parameter)
    if (call->argument_count > 6) {
        fprintf(output, "    ; TODO: Push stack arguments\n");
    }
    
    // Align stack to 16-byte boundary (required by System V ABI)
    fprintf(output, "    and rsp, -16\n");
    
    // Make the call
    fprintf(output, "    call %s\n", call->function_name);
    
    // Result is in rax
}

// Generate return statement
void codegen_return_statement(FILE* output, struct Expression* return_expr) {
    fprintf(output, "    ; Return statement\n");
    
    if (return_expr) {
        // TODO: Evaluate return expression
        fprintf(output, "    ; TODO: Evaluate return expression\n");
        fprintf(output, "    ; Result should be in rax\n");
    } else {
        fprintf(output, "    xor rax, rax  ; Return 0 (void/no value)\n");
    }
    
    // Jump to epilogue (or inline epilogue here)
    fprintf(output, "    mov rsp, rbp\n");
    fprintf(output, "    pop rbp\n");
    fprintf(output, "    ret\n");
}
