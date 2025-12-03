// ============================================================================
// MELP Modular Codegen - codegen_return.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// İçerik: codegen_generate_return
// Return ifadelerini işler (single, multiple, struct return)

void codegen_generate_return(Codegen* gen, Statement* stmt) {
    if (!gen->in_function) {
        fprintf(stderr, "Codegen error: 'return' outside function\n");
        exit(1);
    }
    
    char buffer[256];
    
    if (stmt->return_stmt.value_count > 0) {
        // Generate code for multiple return values
        // For now, support single return value (multiple returns will use stack)
        if (stmt->return_stmt.value_count == 1) {
            // Check if returning a struct variable
            Expression* ret_expr = stmt->return_stmt.values[0];
            if (ret_expr->type == EXPR_VARIABLE) {
                VarInfo* var = codegen_get_variable(gen, ret_expr->var_name);
                if (var && var->struct_name) {
                    // Returning a struct - load all fields into rax, rdx, etc.
                    StructInfo* si = codegen_find_struct(gen, var->struct_name);
                    if (si && si->field_count >= 1) {
                        // First field in rax
                        int field0_offset = var->stack_offset - si->field_offsets[0];
                        snprintf(buffer, sizeof(buffer), 
                                 "    mov rax, [rbp-%d]   ; Load %s.%s for return", 
                                 field0_offset, ret_expr->var_name, si->field_names[0]);
                        codegen_emit(gen, buffer);
                        
                        if (si->field_count >= 2) {
                            // Second field in rdx
                            int field1_offset = var->stack_offset - si->field_offsets[1];
                            snprintf(buffer, sizeof(buffer), 
                                     "    mov rdx, [rbp-%d]   ; Load %s.%s for return", 
                                     field1_offset, ret_expr->var_name, si->field_names[1]);
                            codegen_emit(gen, buffer);
                        }
                    }
                } else {
                    // Regular variable - put in rax
                    codegen_generate_expression_value(gen, ret_expr);
                }
            } else {
                // Single return value - put in rax
                codegen_generate_expression_value(gen, ret_expr);
            }
        } else {
            // Multiple return values - store on stack
            // Return values will be accessed by caller via stack offsets
            // For simplicity: rax = first value, rdx = second value, rest on stack
            char buffer[256];
            
            // First return value in rax
            codegen_generate_expression_value(gen, stmt->return_stmt.values[0]);
            
            if (stmt->return_stmt.value_count >= 2) {
                // Second return value in rdx
                codegen_emit(gen, "    push rax           ; Save first return value");
                codegen_generate_expression_value(gen, stmt->return_stmt.values[1]);
                codegen_emit(gen, "    mov rdx, rax       ; Second return in rdx");
                codegen_emit(gen, "    pop rax            ; Restore first return in rax");
            }
            
            // Additional values (if any) would go in rcx, r8, r9, etc.
            // For now, we'll support up to 2 return values
            if (stmt->return_stmt.value_count > 2) {
                fprintf(stderr, "Codegen warning: More than 2 return values not fully supported yet\n");
            }
        }
    } else {
        // Void return
        codegen_emit(gen, "    xor rax, rax");
    }
    
    // Function epilogue
    codegen_emit(gen, "    mov rsp, rbp");
    codegen_emit(gen, "    pop rbp");
    codegen_emit(gen, "    ret");
}

