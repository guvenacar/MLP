#include "logical_codegen.h"

// Code generation for logical expressions
// Stage 1: Generate x86-64 assembly for logical operations

static int label_counter = 0;

void codegen_logical(FILE* f, LogicalExpr* logical) {
    if (!logical || !f) return;
    
    // Logical operations with short-circuit evaluation
    // Result: 1 (true) or 0 (false) in rax
    
    if (logical->op == LOG_NOT) {
        // Unary NOT: evaluate operand, then invert
        // Operand result in rax (0 or 1)
        fprintf(f, "    ; Logical NOT\n");
        fprintf(f, "    xor rax, 1       ; Flip bit: 0->1, 1->0\n");
    } else if (logical->op == LOG_AND) {
        // Binary AND with short-circuit: left && right
        // If left is false (0), skip right evaluation
        int label_end = label_counter++;
        int label_false = label_counter++;
        
        fprintf(f, "    ; Logical AND (short-circuit)\n");
        // Left side already evaluated -> rax
        fprintf(f, "    test rax, rax\n");
        fprintf(f, "    jz .L_false_%d   ; Left is false, result is false\n", label_false);
        // Left is true, evaluate right side -> rax
        fprintf(f, "    test rax, rax\n");
        fprintf(f, "    jz .L_false_%d   ; Right is false, result is false\n", label_false);
        fprintf(f, "    mov rax, 1       ; Both true, result is true\n");
        fprintf(f, "    jmp .L_end_%d\n", label_end);
        fprintf(f, ".L_false_%d:\n", label_false);
        fprintf(f, "    xor rax, rax     ; Result is false\n");
        fprintf(f, ".L_end_%d:\n", label_end);
    } else if (logical->op == LOG_OR) {
        // Binary OR with short-circuit: left || right
        // If left is true (1), skip right evaluation
        int label_end = label_counter++;
        int label_true = label_counter++;
        
        fprintf(f, "    ; Logical OR (short-circuit)\n");
        // Left side already evaluated -> rax
        fprintf(f, "    test rax, rax\n");
        fprintf(f, "    jnz .L_true_%d   ; Left is true, result is true\n", label_true);
        // Left is false, evaluate right side -> rax
        fprintf(f, "    test rax, rax\n");
        fprintf(f, "    jnz .L_true_%d   ; Right is true, result is true\n", label_true);
        fprintf(f, "    xor rax, rax     ; Both false, result is false\n");
        fprintf(f, "    jmp .L_end_%d\n", label_end);
        fprintf(f, ".L_true_%d:\n", label_true);
        fprintf(f, "    mov rax, 1       ; Result is true\n");
        fprintf(f, ".L_end_%d:\n", label_end);
    }
}
