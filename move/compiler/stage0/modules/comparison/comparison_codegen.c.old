#include "comparison_codegen.h"

// Code generation for comparison expressions
// Stage 1: Generate x86-64 assembly for comparison operations

void codegen_comparison(FILE* f, ComparisonExpr* cmp) {
    if (!cmp || !f) return;
    
    // Comparison result: 1 (true) or 0 (false) in rax
    // Assumes left expression result in rbx, right in rax (from expression codegen)
    
    fprintf(f, "    ; Compare rbx with rax\n");
    fprintf(f, "    cmp rbx, rax\n");
    
    switch (cmp->op) {
        case CMP_EQUAL:
            fprintf(f, "    sete al          ; Set if equal\n");
            break;
        case CMP_NOT_EQUAL:
            fprintf(f, "    setne al         ; Set if not equal\n");
            break;
        case CMP_LESS:
            fprintf(f, "    setl al          ; Set if less\n");
            break;
        case CMP_GREATER:
            fprintf(f, "    setg al          ; Set if greater\n");
            break;
        case CMP_LESS_EQUAL:
            fprintf(f, "    setle al         ; Set if less or equal\n");
            break;
        case CMP_GREATER_EQUAL:
            fprintf(f, "    setge al         ; Set if greater or equal\n");
            break;
    }
    
    fprintf(f, "    movzx rax, al    ; Zero-extend to 64-bit\n");
}
