/**
 * MLP Codegen - Switch Statement Code Generation
 * 
 * Handles switch/case statements including:
 * - Regular case matching
 * - Range patterns (a to b)
 * - Type matching
 * - Guard conditions
 * - Default case
 */

#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
void codegen_generate_expression_value(Codegen* gen, Expression* expr);
void codegen_generate_statement(Codegen* gen, Statement* stmt);
void codegen_generate_comparison(Codegen* gen, Expression* condition, int false_label);
int codegen_add_string(Codegen* gen, const char* str);

void codegen_generate_switch(Codegen* gen, Statement* stmt) {
    char buffer[256];
    int end_label = gen->label_counter++;
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Switch statement");
    
    // Evaluate switch value once and store in rax
    codegen_generate_expression_value(gen, stmt->switch_stmt.value);
    codegen_emit(gen, "    push rax           ; Save switch value");
    
    // Generate code for each case
    for (int i = 0; i < stmt->switch_stmt.case_count; i++) {
        int next_case_label = gen->label_counter++;
        
        if (stmt->switch_stmt.cases[i].is_default) {
            // Default case - always matches
            codegen_emit(gen, "    ; case default:");
        } else if (stmt->switch_stmt.cases[i].is_range) {
            // Range case (a to b)
            codegen_emit(gen, "    ; case range pattern:");
            codegen_emit(gen, "    mov rax, [rsp]      ; Load switch value");
            
            // Check lower bound
            codegen_generate_expression_value(gen, stmt->switch_stmt.cases[i].range_start);
            codegen_emit(gen, "    mov rbx, rax        ; Lower bound in rbx");
            codegen_emit(gen, "    mov rax, [rsp]      ; Switch value");
            codegen_emit(gen, "    cmp rax, rbx");
            snprintf(buffer, sizeof(buffer), "    jl .L%d            ; Skip if value < lower", next_case_label);
            codegen_emit(gen, buffer);
            
            // Check upper bound
            codegen_generate_expression_value(gen, stmt->switch_stmt.cases[i].range_end);
            codegen_emit(gen, "    mov rbx, rax        ; Upper bound in rbx");
            codegen_emit(gen, "    mov rax, [rsp]      ; Switch value");
            codegen_emit(gen, "    cmp rax, rbx");
            snprintf(buffer, sizeof(buffer), "    jg .L%d            ; Skip if value > upper", next_case_label);
            codegen_emit(gen, buffer);
        } else if (stmt->switch_stmt.cases[i].type_name) {
            // Type match case
            codegen_emit(gen, "    ; case type match:");
            // For now, type matching compares struct type tags
            // This would need runtime type information (RTTI) for full implementation
            int str_id = codegen_add_string(gen, stmt->switch_stmt.cases[i].type_name);
            snprintf(buffer, sizeof(buffer), "    ; Type match for: %s (str_%d)", 
                     stmt->switch_stmt.cases[i].type_name, str_id);
            codegen_emit(gen, buffer);
            // TODO: Implement RTTI-based type checking
            snprintf(buffer, sizeof(buffer), "    jmp .L%d            ; Type matching not fully implemented", next_case_label);
            codegen_emit(gen, buffer);
        } else {
            // Value match case(s)
            if (stmt->switch_stmt.cases[i].value_count == 1) {
                // Single value
                codegen_emit(gen, "    ; case single value:");
                codegen_generate_expression_value(gen, stmt->switch_stmt.cases[i].values[0]);
                codegen_emit(gen, "    mov rbx, rax        ; Case value");
                codegen_emit(gen, "    mov rax, [rsp]      ; Switch value");
                codegen_emit(gen, "    cmp rax, rbx");
                snprintf(buffer, sizeof(buffer), "    jne .L%d           ; Skip if not equal", next_case_label);
                codegen_emit(gen, buffer);
            } else {
                // Multiple values (OR logic)
                codegen_emit(gen, "    ; case multiple values:");
                int match_label = gen->label_counter++;
                
                for (int v = 0; v < stmt->switch_stmt.cases[i].value_count; v++) {
                    codegen_generate_expression_value(gen, stmt->switch_stmt.cases[i].values[v]);
                    codegen_emit(gen, "    mov rbx, rax        ; Case value");
                    codegen_emit(gen, "    mov rax, [rsp]      ; Switch value");
                    codegen_emit(gen, "    cmp rax, rbx");
                    snprintf(buffer, sizeof(buffer), "    je .L%d            ; Match found", match_label);
                    codegen_emit(gen, buffer);
                }
                // No match - skip to next case
                snprintf(buffer, sizeof(buffer), "    jmp .L%d", next_case_label);
                codegen_emit(gen, buffer);
                
                // Match label
                snprintf(buffer, sizeof(buffer), ".L%d:", match_label);
                codegen_emit(gen, buffer);
            }
        }
        
        // Check guard condition if present
        if (stmt->switch_stmt.cases[i].guard) {
            codegen_emit(gen, "    ; Guard condition:");
            
            if (stmt->switch_stmt.cases[i].guard->type == EXPR_COMPARISON) {
                codegen_generate_comparison(gen, stmt->switch_stmt.cases[i].guard, next_case_label);
            } else {
                codegen_generate_expression_value(gen, stmt->switch_stmt.cases[i].guard);
                codegen_emit(gen, "    test rax, rax");
                snprintf(buffer, sizeof(buffer), "    jz .L%d            ; Guard failed", next_case_label);
                codegen_emit(gen, buffer);
            }
        }
        
        // Generate case body
        for (int b = 0; b < stmt->switch_stmt.cases[i].body_count; b++) {
            codegen_generate_statement(gen, stmt->switch_stmt.cases[i].body[b]);
        }
        
        // Jump to end (no fall-through by default in MLP)
        snprintf(buffer, sizeof(buffer), "    jmp .L%d           ; End of case", end_label);
        codegen_emit(gen, buffer);
        
        // Next case label
        snprintf(buffer, sizeof(buffer), ".L%d:", next_case_label);
        codegen_emit(gen, buffer);
    }
    
    // End of switch
    snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
    codegen_emit(gen, buffer);
    codegen_emit(gen, "    add rsp, 8          ; Pop switch value");
    codegen_emit(gen, "");
}
