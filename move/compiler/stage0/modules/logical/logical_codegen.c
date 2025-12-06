#include "logical_codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int label_counter = 0;

// Load boolean value
static void load_bool(FILE* output, const char* value, int is_literal) {
    if (is_literal) {
        if (strcmp(value, "true") == 0) {
            fprintf(output, "    mov rax, 1\n");
        } else {
            fprintf(output, "    mov rax, 0\n");
        }
    } else {
        fprintf(output, "    mov rax, [%s]\n", value);
    }
}

// Generate code recursively
static void generate_expr_code(FILE* output, LogicalExpr* expr) {
    if (!expr) return;
    
    // Leaf node
    if (expr->is_literal || (!expr->left && !expr->right)) {
        load_bool(output, expr->value, expr->is_literal);
        return;
    }
    
    // NOT operator (unary)
    if (expr->op == LOG_NOT) {
        generate_expr_code(output, expr->left);
        fprintf(output, "    test rax, rax\n");
        fprintf(output, "    setz al             ; Invert boolean\n");
        fprintf(output, "    movzx rax, al\n");
        return;
    }
    
    // AND operator (short-circuit)
    if (expr->op == LOG_AND) {
        int label_false = label_counter++;
        int label_end = label_counter++;
        
        // Evaluate left
        generate_expr_code(output, expr->left);
        fprintf(output, "    test rax, rax\n");
        fprintf(output, "    jz .logical_false_%d  ; Short-circuit if false\n", label_false);
        
        // Evaluate right
        generate_expr_code(output, expr->right);
        fprintf(output, "    jmp .logical_end_%d\n", label_end);
        
        // False label
        fprintf(output, ".logical_false_%d:\n", label_false);
        fprintf(output, "    mov rax, 0\n");
        
        fprintf(output, ".logical_end_%d:\n", label_end);
        return;
    }
    
    // OR operator (short-circuit)
    if (expr->op == LOG_OR) {
        int label_true = label_counter++;
        int label_end = label_counter++;
        
        // Evaluate left
        generate_expr_code(output, expr->left);
        fprintf(output, "    test rax, rax\n");
        fprintf(output, "    jnz .logical_true_%d   ; Short-circuit if true\n", label_true);
        
        // Evaluate right
        generate_expr_code(output, expr->right);
        fprintf(output, "    jmp .logical_end_%d\n", label_end);
        
        // True label
        fprintf(output, ".logical_true_%d:\n", label_true);
        fprintf(output, "    mov rax, 1\n");
        
        fprintf(output, ".logical_end_%d:\n", label_end);
        return;
    }
}

// Generate logical expression code
void logical_generate_code(FILE* output, LogicalExpr* expr) {
    if (!output || !expr) return;
    
    fprintf(output, "\n    ; Logical expression\n");
    generate_expr_code(output, expr);
    fprintf(output, "    ; Result in rax (0=false, 1=true)\n");
}
