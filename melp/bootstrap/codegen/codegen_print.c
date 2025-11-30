// MELP Modular Codegen - Print Generation
// Print kodu üretimi

#include "codegen.h"

// Forward declarations
extern void codegen_generate_expression_value(Codegen* gen, Expression* expr);

// ============================================================================
// Print Code Generation
// ============================================================================

void codegen_generate_print(Codegen* gen, Expression* expr) {
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Print statement");
    
    // Evaluate expression to rax
    codegen_generate_expression_value(gen, expr);
    
    // Call appropriate print function based on expression type
    if (expr->type == EXPR_STRING) {
        codegen_emit(gen, "    mov rdi, rax");
        codegen_emit(gen, "    call print_string");
    } else if (expr->type == EXPR_NUMBER) {
        // TTO: Check if double or integer
        if (expr->internal_numeric_type == INTERNAL_DOUBLE || expr->has_decimal_point) {
            codegen_emit(gen, "    movq xmm0, rax    ; TTO: double bit pattern to xmm0");
            codegen_emit(gen, "    push rbx          ; Save for alignment calculation");
            codegen_emit(gen, "    mov rbx, rsp      ; Save current stack");
            codegen_emit(gen, "    and rsp, -16      ; Align stack to 16 bytes");
            codegen_emit(gen, "    call print_double");
            codegen_emit(gen, "    mov rsp, rbx      ; Restore stack");
            codegen_emit(gen, "    pop rbx           ; Restore rbx");
        } else {
            codegen_emit(gen, "    mov rdi, rax");
            codegen_emit(gen, "    call print_number");
        }
    } else if (expr->type == EXPR_VARIABLE) {
        codegen_generate_print_variable(gen, expr);
    } else {
        codegen_emit(gen, "    mov rdi, rax");
        codegen_emit(gen, "    call print_number");
    }
}

// ============================================================================
// Print Variable Helper
// ============================================================================

void codegen_generate_print_variable(Codegen* gen, Expression* expr) {
    // Check if this is a state variable first
    int is_state_var = 0;
    for (int i = 0; i < gen->global_var_count; i++) {
        if (strcmp(gen->global_vars[i].name, expr->var_name) == 0) {
            is_state_var = 1;
            codegen_emit(gen, "    mov rdi, rax");
            if (gen->global_vars[i].type == TYPE_DECIMAL) {
                codegen_emit(gen, "    movq xmm0, rax");
                codegen_emit(gen, "    push rbx");
                codegen_emit(gen, "    mov rbx, rsp");
                codegen_emit(gen, "    and rsp, -16");
                codegen_emit(gen, "    call print_double");
                codegen_emit(gen, "    mov rsp, rbx");
                codegen_emit(gen, "    pop rbx");
            } else {
                codegen_emit(gen, "    call print_number");
            }
            break;
        }
    }
    
    if (!is_state_var) {
        VarInfo* var = codegen_get_variable(gen, expr->var_name);
        if (var->type == TYPE_STRING) {
            codegen_emit(gen, "    mov rdi, rax");
            codegen_emit(gen, "    call print_string");
        } else if (var->internal_numeric_type == INTERNAL_DOUBLE) {
            codegen_emit(gen, "    movq xmm0, rax    ; TTO: Load double for print");
            codegen_emit(gen, "    push rbx          ; Save for alignment");
            codegen_emit(gen, "    mov rbx, rsp      ; Save stack");
            codegen_emit(gen, "    and rsp, -16      ; Align to 16 bytes");
            codegen_emit(gen, "    call print_double");
            codegen_emit(gen, "    mov rsp, rbx      ; Restore stack");
            codegen_emit(gen, "    pop rbx           ; Restore rbx");
        } else {
            codegen_emit(gen, "    mov rdi, rax");
            codegen_emit(gen, "    call print_number");
        }
    }
}

// ============================================================================
// Helper: Is String Expression?
// ============================================================================

int is_string_expression(Codegen* gen, Expression* expr) {
    if (expr->type == EXPR_STRING) {
        return 1;
    }
    if (expr->type == EXPR_VARIABLE) {
        for (int i = 0; i < gen->global_var_count; i++) {
            if (strcmp(gen->global_vars[i].name, expr->var_name) == 0) {
                return gen->global_vars[i].type == TYPE_STRING;
            }
        }
        VarInfo* var = codegen_get_variable(gen, expr->var_name);
        return var->type == TYPE_STRING;
    }
    if (expr->type == EXPR_BINARY_OP) {
        return is_string_expression(gen, expr->binary_op.left) || 
               is_string_expression(gen, expr->binary_op.right);
    }
    return 0;
}

// ============================================================================
// TTO Helper: Is Double Expression?
// ============================================================================

int is_double_expression(Codegen* gen, Expression* expr) {
    if (expr->type == EXPR_NUMBER) {
        return expr->internal_numeric_type == INTERNAL_DOUBLE || expr->has_decimal_point;
    }
    if (expr->type == EXPR_VARIABLE) {
        for (int i = 0; i < gen->global_var_count; i++) {
            if (strcmp(gen->global_vars[i].name, expr->var_name) == 0) {
                return 0;  // State variables don't track TTO type yet
            }
        }
        VarInfo* var = codegen_get_variable(gen, expr->var_name);
        return var->internal_numeric_type == INTERNAL_DOUBLE;
    }
    if (expr->type == EXPR_BINARY_OP) {
        return is_double_expression(gen, expr->binary_op.left) || 
               is_double_expression(gen, expr->binary_op.right);
    }
    return 0;
}
