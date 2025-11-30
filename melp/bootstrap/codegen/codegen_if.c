// MELP Modular Codegen - If/Else Generation
// If/else kodu üretimi

#include "codegen.h"

// Forward declarations
extern void codegen_generate_expression_value(Codegen* gen, Expression* expr);
extern void codegen_generate_comparison(Codegen* gen, Expression* condition, int false_label);
extern void codegen_generate_statement(Codegen* gen, Statement* stmt);

// ============================================================================
// If/Else Code Generation
// ============================================================================

void codegen_generate_if(Codegen* gen, Statement* stmt) {
    char buffer[256];
    int else_label = gen->label_counter++;
    int end_label = gen->label_counter++;
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; If statement");
    
    // Generate condition check
    if (stmt->if_stmt.condition->type == EXPR_COMPARISON) {
        codegen_generate_comparison(gen, stmt->if_stmt.condition, else_label);
    } else if (stmt->if_stmt.condition->type == EXPR_LOGICAL_AND ||
               stmt->if_stmt.condition->type == EXPR_LOGICAL_OR ||
               stmt->if_stmt.condition->type == EXPR_LOGICAL_NOT) {
        // Logical expression (and, or, not)
        codegen_generate_expression_value(gen, stmt->if_stmt.condition);
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jz .L%d", else_label);
        codegen_emit(gen, buffer);
    } else {
        // Other expressions (function call, variable, etc.) - evaluate and check if non-zero
        codegen_generate_expression_value(gen, stmt->if_stmt.condition);
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jz .L%d", else_label);
        codegen_emit(gen, buffer);
    }
    
    // Then body
    codegen_emit(gen, "    ; Then body");
    for (int i = 0; i < stmt->if_stmt.then_count; i++) {
        codegen_generate_statement(gen, stmt->if_stmt.then_body[i]);
    }
    
    if (stmt->if_stmt.else_count > 0) {
        snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
        codegen_emit(gen, buffer);
    }
    
    // Else label
    snprintf(buffer, sizeof(buffer), ".L%d:", else_label);
    codegen_emit(gen, buffer);
    
    // Else body (if exists)
    if (stmt->if_stmt.else_count > 0) {
        codegen_emit(gen, "    ; Else body");
        for (int i = 0; i < stmt->if_stmt.else_count; i++) {
            codegen_generate_statement(gen, stmt->if_stmt.else_body[i]);
        }
        
        snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
        codegen_emit(gen, buffer);
    }
}
