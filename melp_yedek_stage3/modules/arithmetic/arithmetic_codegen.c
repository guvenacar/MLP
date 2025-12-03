#include "arithmetic_codegen.h"

// Code generation for arithmetic expressions
// Stage 1: Generate x86-64 assembly for arithmetic operations

void codegen_expression(FILE* f, Expression* expr) {
    if (!expr || !f) return;
    
    if (expr->type == EXPR_NUMBER) {
        // Load immediate number into rax
        fprintf(f, "    mov rax, %ld\n", expr->data.number_value);
    } else if (expr->type == EXPR_VARIABLE) {
        // Load variable value into rax
        fprintf(f, "    mov rax, [var_%s]\n", expr->data.var_name);
    } else if (expr->type == EXPR_BINARY_OP) {
        // Evaluate left side -> rax
        codegen_expression(f, expr->data.binary.left);
        // Push rax to stack
        fprintf(f, "    push rax\n");
        // Evaluate right side -> rax
        codegen_expression(f, expr->data.binary.right);
        // Pop left side to rbx
        fprintf(f, "    pop rbx\n");
        
        // Perform operation: rbx op rax -> rax
        switch (expr->data.binary.op) {
            case BIN_OP_ADD:
                fprintf(f, "    add rax, rbx\n");
                break;
            case BIN_OP_SUB:
                fprintf(f, "    sub rbx, rax\n");
                fprintf(f, "    mov rax, rbx\n");
                break;
            case BIN_OP_MUL:
                fprintf(f, "    imul rax, rbx\n");
                break;
            case BIN_OP_DIV:
                fprintf(f, "    mov rcx, rax\n");
                fprintf(f, "    mov rax, rbx\n");
                fprintf(f, "    cqo\n");
                fprintf(f, "    idiv rcx\n");
                break;
        }
    }
}
