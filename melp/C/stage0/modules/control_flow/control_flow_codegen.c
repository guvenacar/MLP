#include "control_flow_codegen.h"
#include "../comparison/comparison.h"
#include "../comparison/comparison_codegen.h"
#include <stdio.h>

static int label_counter = 0;

void control_flow_generate_if(FILE* output, IfStatement* stmt) {
    if (!output || !stmt) return;
    
    // TIER 1: Handle expression-based if
    if (stmt->type == CTRL_IF_EXPR) {
        fprintf(output, "\n    ; TIER 1: Expression-based if (result = if cond then val1 else val2)\n");
        fprintf(output, "    ; TODO: Implement expression-based if codegen\n");
        return;
    }
    
    int label_else = label_counter++;
    int label_end = label_counter++;
    
    fprintf(output, "\n    ; If statement\n");
    
    // Load and compare condition
    ComparisonExpr* cond = (ComparisonExpr*)stmt->condition;
    comparison_generate_code(output, cond);
    
    // Test result
    fprintf(output, "    test rax, rax\n");
    if (stmt->has_else) {
        fprintf(output, "    jz .if_else_%d\n", label_else);
    } else {
        fprintf(output, "    jz .if_end_%d\n", label_end);
    }
    
    // Then body
    fprintf(output, "    ; Then body\n");
    
    if (stmt->has_else) {
        fprintf(output, "    jmp .if_end_%d\n", label_end);
        fprintf(output, ".if_else_%d:\n", label_else);
        fprintf(output, "    ; Else body\n");
    }
    
    fprintf(output, ".if_end_%d:\n", label_end);
}

void control_flow_generate_while(FILE* output, WhileStatement* stmt) {
    if (!output || !stmt) return;
    
    int label_start = label_counter++;
    int label_end = label_counter++;
    
    fprintf(output, "\n    ; While loop\n");
    fprintf(output, ".while_start_%d:\n", label_start);
    
    // Condition
    ComparisonExpr* cond = (ComparisonExpr*)stmt->condition;
    comparison_generate_code(output, cond);
    
    fprintf(output, "    test rax, rax\n");
    fprintf(output, "    jz .while_end_%d\n", label_end);
    
    // Body
    fprintf(output, "    ; Loop body\n");
    fprintf(output, "    jmp .while_start_%d\n", label_start);
    fprintf(output, ".while_end_%d:\n", label_end);
}

void control_flow_generate_for(FILE* output, ForStatement* stmt) {
    if (!output || !stmt) return;
    
    fprintf(output, "\n    ; For loop\n");
    
    // TIER 1: Handle labeled loops
    if (stmt->label) {
        fprintf(output, "    ; TIER 1: Loop label '%s'\n", stmt->label);
        fprintf(output, ".loop_label_%s:\n", stmt->label);
    }
    
    fprintf(output, "    ; Iterator: %s\n", stmt->iterator ? stmt->iterator : "none");
}
