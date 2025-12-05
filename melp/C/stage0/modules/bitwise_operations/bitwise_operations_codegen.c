#include "bitwise_operations_codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Generate x86-64 assembly for bitwise operations
void codegen_bitwise_operation(FILE* out, BitwiseExpr* expr) {
    if (!expr) return;

    fprintf(out, "; Bitwise operation\n");

    // Load left operand into rax
    if (expr->is_literal) {
        fprintf(out, "    mov rax, %s\n", expr->value);
    } else {
        fprintf(out, "    mov rax, [%s]\n", expr->value);
    }

    // If we have a right operand, load it into rbx
    if (expr->right) {
        if (expr->right->is_literal) {
            fprintf(out, "    mov rbx, %s\n", expr->right->value);
        } else {
            fprintf(out, "    mov rbx, [%s]\n", expr->right->value);
        }
    }

    // Perform the operation
    switch (expr->op) {
        case BITWISE_AND:
            fprintf(out, "    and rax, rbx    ; Bitwise AND\n");
            break;
        case BITWISE_OR:
            fprintf(out, "    or rax, rbx     ; Bitwise OR\n");
            break;
        case BITWISE_XOR:
            fprintf(out, "    xor rax, rbx    ; Bitwise XOR\n");
            break;
        case BITWISE_NOT:
            fprintf(out, "    not rax         ; Bitwise NOT\n");
            break;
        case BITWISE_LSHIFT:
            fprintf(out, "    mov rcx, rbx\n");
            fprintf(out, "    shl rax, cl     ; Left shift\n");
            break;
        case BITWISE_RSHIFT:
            fprintf(out, "    mov rcx, rbx\n");
            fprintf(out, "    shr rax, cl     ; Right shift\n");
            break;
        case BITWISE_ARSHIFT:
            fprintf(out, "    mov rcx, rbx\n");
            fprintf(out, "    sar rax, cl     ; Arithmetic right shift\n");
            break;
    }

    // Result is in rax
    fprintf(out, "    ; Result in rax\n\n");
}
