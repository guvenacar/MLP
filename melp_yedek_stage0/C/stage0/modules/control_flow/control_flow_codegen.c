#include "control_flow_codegen.h"
#include <stdio.h>

// Generate assembly for if-then-else statement
void codegen_if_statement(FILE* output, IfStatement* if_stmt, int* label_counter) {
    int label_id = (*label_counter)++;
    int else_label = label_id * 2;
    int end_label = label_id * 2 + 1;
    
    fprintf(output, "    ; if-then-else statement\n");
    
    // TODO: Evaluate condition (requires expression codegen)
    fprintf(output, "    ; TODO: evaluate condition\n");
    fprintf(output, "    test rax, rax\n");
    
    if (if_stmt->else_body) {
        // If we have an else clause
        fprintf(output, "    jz .L_else_%d\n", else_label);
        
        // Then block
        fprintf(output, "    ; then block\n");
        // TODO: Generate code for then_body statements
        fprintf(output, "    jmp .L_end_if_%d\n", end_label);
        
        // Else block
        fprintf(output, ".L_else_%d:\n", else_label);
        fprintf(output, "    ; else block\n");
        // TODO: Generate code for else_body statements
        
        fprintf(output, ".L_end_if_%d:\n", end_label);
    } else {
        // No else clause
        fprintf(output, "    jz .L_end_if_%d\n", end_label);
        
        // Then block
        fprintf(output, "    ; then block\n");
        // TODO: Generate code for then_body statements
        
        fprintf(output, ".L_end_if_%d:\n", end_label);
    }
}

// Generate assembly for while loop
void codegen_while_statement(FILE* output, WhileStatement* while_stmt, int* label_counter) {
    int label_id = (*label_counter)++;
    int start_label = label_id * 2;
    int end_label = label_id * 2 + 1;
    
    fprintf(output, "    ; while loop\n");
    fprintf(output, ".L_while_start_%d:\n", start_label);
    
    // TODO: Evaluate condition
    fprintf(output, "    ; TODO: evaluate condition\n");
    fprintf(output, "    test rax, rax\n");
    fprintf(output, "    jz .L_while_end_%d\n", end_label);
    
    // Loop body
    fprintf(output, "    ; loop body\n");
    // TODO: Generate code for body statements
    
    fprintf(output, "    jmp .L_while_start_%d\n", start_label);
    fprintf(output, ".L_while_end_%d:\n", end_label);
}

// Generate assembly for for loop
void codegen_for_statement(FILE* output, ForStatement* for_stmt, int* label_counter) {
    int label_id = (*label_counter)++;
    int start_label = label_id * 2;
    int end_label = label_id * 2 + 1;
    
    fprintf(output, "    ; for loop: %s = start to end\n", for_stmt->variable);
    
    // Initialize loop variable
    fprintf(output, "    ; TODO: initialize %s with start value\n", for_stmt->variable);
    
    fprintf(output, ".L_for_start_%d:\n", start_label);
    
    // Check condition: variable <= end
    fprintf(output, "    ; TODO: compare %s with end value\n", for_stmt->variable);
    fprintf(output, "    jg .L_for_end_%d\n", end_label);
    
    // Loop body
    fprintf(output, "    ; loop body\n");
    // TODO: Generate code for body statements
    
    // Increment loop variable
    fprintf(output, "    ; TODO: increment %s\n", for_stmt->variable);
    
    fprintf(output, "    jmp .L_for_start_%d\n", start_label);
    fprintf(output, ".L_for_end_%d:\n", end_label);
}
