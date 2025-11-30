/**
 * MLP Codegen - Expression Code Generation
 * 
 * Handles all expression value generation including:
 * - Literals (number, string, null)
 * - Variables (local, state, captured)
 * - Binary operations (arithmetic, string concat)
 * - Function calls (user-defined, builtin, lambda)
 * - Field access, array indexing
 * - Logical operators
 * - Lambda/closure creation
 * - And more...
 */

#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
int codegen_add_string(Codegen* gen, const char* str);
void codegen_generate_comparison(Codegen* gen, Expression* condition, int false_label);
void codegen_generate_statement(Codegen* gen, Statement* stmt);
int is_builtin_function(const char* name);

// Helper: Check if expression evaluates to string
int is_string_expression(Codegen* gen, Expression* expr) {
    if (expr->type == EXPR_STRING) return 1;
    if (expr->type == EXPR_VARIABLE) {
        // Check state vars first
        for (int i = 0; i < gen->global_var_count; i++) {
            if (strcmp(gen->global_vars[i].name, expr->var_name) == 0) {
                return gen->global_vars[i].type == TYPE_STRING;
            }
        }
        VarInfo* var = gen->variables;
        while (var) {
            if (strcmp(var->name, expr->var_name) == 0) {
                return var->type == TYPE_STRING;
            }
            var = var->next;
        }
    }
    if (expr->type == EXPR_FUNC_CALL) {
        // Some builtins return strings
        if (strcmp(expr->func_call.func_name, "substring") == 0 ||
            strcmp(expr->func_call.func_name, "trim") == 0 ||
            strcmp(expr->func_call.func_name, "to_upper") == 0 ||
            strcmp(expr->func_call.func_name, "to_lower") == 0 ||
            strcmp(expr->func_call.func_name, "replace") == 0 ||
            strcmp(expr->func_call.func_name, "to_text") == 0 ||
            strcmp(expr->func_call.func_name, "input") == 0 ||
            strcmp(expr->func_call.func_name, "read_file") == 0) {
            return 1;
        }
    }
    return 0;
}

// Helper: Check if expression evaluates to double (TTO)
int is_double_expression(Codegen* gen, Expression* expr) {
    if (expr->type == EXPR_NUMBER) {
        return expr->internal_numeric_type == INTERNAL_DOUBLE || expr->has_decimal_point;
    }
    if (expr->type == EXPR_VARIABLE) {
        // Check state vars first
        for (int i = 0; i < gen->global_var_count; i++) {
            if (strcmp(gen->global_vars[i].name, expr->var_name) == 0) {
                return gen->global_vars[i].is_double;
            }
        }
        VarInfo* var = gen->variables;
        while (var) {
            if (strcmp(var->name, expr->var_name) == 0) {
                return var->is_double;
            }
            var = var->next;
        }
    }
    if (expr->type == EXPR_BINARY_OP) {
        return is_double_expression(gen, expr->binary_op.left) ||
               is_double_expression(gen, expr->binary_op.right);
    }
    if (expr->type == EXPR_FUNC_CALL) {
        // Math functions return double
        const char* name = expr->func_call.func_name;
        if (strcmp(name, "sin") == 0 || strcmp(name, "cos") == 0 ||
            strcmp(name, "tan") == 0 || strcmp(name, "sqrt") == 0 ||
            strcmp(name, "pow") == 0 || strcmp(name, "log") == 0 ||
            strcmp(name, "log10") == 0 || strcmp(name, "exp") == 0 ||
            strcmp(name, "floor") == 0 || strcmp(name, "ceil") == 0 ||
            strcmp(name, "round") == 0 || strcmp(name, "fmod") == 0 ||
            strcmp(name, "pi") == 0 || strcmp(name, "e") == 0 ||
            strcmp(name, "parse_float") == 0 || strcmp(name, "to_decimal") == 0) {
            return 1;
        }
    }
    return 0;
}

void codegen_generate_expression_value(Codegen* gen, Expression* expr) {
    char buffer[512];
    
    if (expr->type == EXPR_NUMBER) {
        // TTO: Generate code based on internal numeric type
        if (expr->internal_numeric_type == INTERNAL_DOUBLE || expr->has_decimal_point) {
            codegen_emit(gen, "    ; TTO: Loading double value");
            union { double d; unsigned long bits; } converter;
            converter.d = expr->double_value;
            snprintf(buffer, sizeof(buffer), "    mov rax, 0x%lx    ; double %.15g bit pattern", 
                     converter.bits, expr->double_value);
            codegen_emit(gen, buffer);
        } else {
            snprintf(buffer, sizeof(buffer), "    mov rax, %ld", expr->number_value);
            codegen_emit(gen, buffer);
        }
    } else if (expr->type == EXPR_NULL) {
        codegen_emit(gen, "    mov rax, 0");
    } else if (expr->type == EXPR_STRING) {
        int str_id = codegen_add_string(gen, expr->string_value);
        snprintf(buffer, sizeof(buffer), "    mov rax, str_%d", str_id);
        codegen_emit(gen, buffer);
    } else if (expr->type == EXPR_VARIABLE) {
        // Check state variable first
        int is_state_var = 0;
        for (int i = 0; i < gen->global_var_count; i++) {
            if (strcmp(gen->global_vars[i].name, expr->var_name) == 0) {
                is_state_var = 1;
                snprintf(buffer, sizeof(buffer), "    mov rax, [state_%s]", expr->var_name);
                codegen_emit(gen, buffer);
                break;
            }
        }
        
        if (!is_state_var) {
            VarInfo* var = codegen_get_variable(gen, expr->var_name);
            
            if (var->stack_offset < 0 && var->stack_offset <= -1000) {
                // Captured variable from closure
                int capture_index = -(var->stack_offset + 1000);
                int env_offset = 8;
                snprintf(buffer, sizeof(buffer), 
                        "    ; Load captured variable %s from environment[%d]", 
                        expr->var_name, capture_index);
                codegen_emit(gen, buffer);
                snprintf(buffer, sizeof(buffer), "    mov r13, [rbp-%d]   ; Environment pointer", env_offset);
                codegen_emit(gen, buffer);
                snprintf(buffer, sizeof(buffer), "    mov rax, [r13+%d]", capture_index * 8);
                codegen_emit(gen, buffer);
            } else {
                snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]", var->stack_offset);
                codegen_emit(gen, buffer);
            }
        }
    } else if (expr->type == EXPR_BINARY_OP) {
        // Check string concatenation
        if (expr->binary_op.op == BIN_OP_ADD && 
            (is_string_expression(gen, expr->binary_op.left) || 
             is_string_expression(gen, expr->binary_op.right))) {
            int left_is_string = is_string_expression(gen, expr->binary_op.left);
            int right_is_string = is_string_expression(gen, expr->binary_op.right);
            
            codegen_generate_expression_value(gen, expr->binary_op.left);
            if (!left_is_string) {
                codegen_emit(gen, "    mov rdi, rax");
                codegen_emit(gen, "    call int_to_string");
            }
            codegen_emit(gen, "    push rax");
            
            codegen_generate_expression_value(gen, expr->binary_op.right);
            if (!right_is_string) {
                codegen_emit(gen, "    mov rdi, rax");
                codegen_emit(gen, "    call int_to_string");
            }
            codegen_emit(gen, "    mov rsi, rax");
            codegen_emit(gen, "    pop rdi");
            codegen_emit(gen, "    call string_concat");
        } else {
            // TTO: Check for double arithmetic
            int left_is_double = is_double_expression(gen, expr->binary_op.left);
            int right_is_double = is_double_expression(gen, expr->binary_op.right);
            int use_double = left_is_double || right_is_double;
            
            if (use_double) {
                codegen_emit(gen, "    ; TTO: Double arithmetic");
                codegen_generate_expression_value(gen, expr->binary_op.left);
                if (!left_is_double) {
                    codegen_emit(gen, "    cvtsi2sd xmm0, rax");
                } else {
                    codegen_emit(gen, "    movq xmm0, rax");
                }
                codegen_emit(gen, "    sub rsp, 8");
                codegen_emit(gen, "    movsd [rsp], xmm0");
                
                codegen_generate_expression_value(gen, expr->binary_op.right);
                if (!right_is_double) {
                    codegen_emit(gen, "    cvtsi2sd xmm1, rax");
                } else {
                    codegen_emit(gen, "    movq xmm1, rax");
                }
                codegen_emit(gen, "    movsd xmm0, [rsp]");
                codegen_emit(gen, "    add rsp, 8");
                
                switch (expr->binary_op.op) {
                    case BIN_OP_ADD: codegen_emit(gen, "    addsd xmm0, xmm1"); break;
                    case BIN_OP_SUB: codegen_emit(gen, "    subsd xmm0, xmm1"); break;
                    case BIN_OP_MUL: codegen_emit(gen, "    mulsd xmm0, xmm1"); break;
                    case BIN_OP_DIV: codegen_emit(gen, "    divsd xmm0, xmm1"); break;
                }
                codegen_emit(gen, "    movq rax, xmm0");
            } else {
                // Integer operations
                codegen_generate_expression_value(gen, expr->binary_op.left);
                codegen_emit(gen, "    push rax");
                codegen_generate_expression_value(gen, expr->binary_op.right);
                codegen_emit(gen, "    mov rbx, rax");
                codegen_emit(gen, "    pop rax");
                
                switch (expr->binary_op.op) {
                    case BIN_OP_ADD: codegen_emit(gen, "    add rax, rbx"); break;
                    case BIN_OP_SUB: codegen_emit(gen, "    sub rax, rbx"); break;
                    case BIN_OP_MUL: codegen_emit(gen, "    imul rax, rbx"); break;
                    case BIN_OP_DIV:
                        codegen_emit(gen, "    cqo");
                        codegen_emit(gen, "    idiv rbx");
                        break;
                }
            }
        }
    } else if (expr->type == EXPR_FUNC_CALL) {
        codegen_generate_func_call(gen, expr);
    } else if (expr->type == EXPR_FIELD_ACCESS) {
        codegen_generate_field_access(gen, expr);
    } else if (expr->type == EXPR_COMPARISON) {
        codegen_generate_expression_value(gen, expr->comparison.left);
        codegen_emit(gen, "    push rax");
        codegen_generate_expression_value(gen, expr->comparison.right);
        codegen_emit(gen, "    mov rbx, rax");
        codegen_emit(gen, "    pop rax");
        codegen_emit(gen, "    cmp rax, rbx");
        codegen_emit(gen, "    mov rax, 0");
        codegen_emit(gen, "    mov rbx, 1");
        switch (expr->comparison.op) {
            case CMP_EQUAL: codegen_emit(gen, "    cmove rax, rbx"); break;
            case CMP_NOT_EQUAL: codegen_emit(gen, "    cmovne rax, rbx"); break;
            case CMP_LESS: codegen_emit(gen, "    cmovl rax, rbx"); break;
            case CMP_LESS_EQUAL: codegen_emit(gen, "    cmovle rax, rbx"); break;
            case CMP_GREATER: codegen_emit(gen, "    cmovg rax, rbx"); break;
            case CMP_GREATER_EQUAL: codegen_emit(gen, "    cmovge rax, rbx"); break;
        }
    } else if (expr->type == EXPR_TERNARY) {
        int false_label = gen->label_counter++;
        int end_label = gen->label_counter++;
        codegen_emit(gen, "    ; Ternary operator");
        if (expr->ternary.condition->type == EXPR_COMPARISON) {
            codegen_generate_comparison(gen, expr->ternary.condition, false_label);
        } else {
            codegen_generate_expression_value(gen, expr->ternary.condition);
            codegen_emit(gen, "    test rax, rax");
            snprintf(buffer, sizeof(buffer), "    jz .L%d", false_label);
            codegen_emit(gen, buffer);
        }
        codegen_generate_expression_value(gen, expr->ternary.true_expr);
        snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
        codegen_emit(gen, buffer);
        snprintf(buffer, sizeof(buffer), ".L%d:", false_label);
        codegen_emit(gen, buffer);
        codegen_generate_expression_value(gen, expr->ternary.false_expr);
        snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
        codegen_emit(gen, buffer);
    } else if (expr->type == EXPR_ARRAY_INDEX) {
        codegen_generate_array_index(gen, expr);
    } else if (expr->type == EXPR_ADDRESS_OF) {
        codegen_generate_address_of(gen, expr);
    } else if (expr->type == EXPR_DEREFERENCE) {
        codegen_generate_expression_value(gen, expr->unary_operand);
        codegen_emit(gen, "    mov rax, [rax]");
    } else if (expr->type == EXPR_LOGICAL_AND) {
        codegen_generate_logical_and(gen, expr);
    } else if (expr->type == EXPR_LOGICAL_OR) {
        codegen_generate_logical_or(gen, expr);
    } else if (expr->type == EXPR_LOGICAL_NOT) {
        codegen_generate_expression_value(gen, expr->logical_not_operand);
        codegen_emit(gen, "    test rax, rax");
        codegen_emit(gen, "    setz al");
        codegen_emit(gen, "    movzx rax, al");
    } else if (expr->type == EXPR_LAMBDA) {
        codegen_generate_lambda(gen, expr);
    } else if (expr->type == EXPR_AWAIT) {
        codegen_emit(gen, "    ; Await (sync for now)");
        codegen_generate_expression_value(gen, expr->await_expr.awaited_expr);
    } else if (expr->type == EXPR_OPTIONAL_CHAIN) {
        codegen_generate_optional_chain(gen, expr);
    } else if (expr->type == EXPR_NULL_COALESCE) {
        codegen_generate_null_coalesce(gen, expr);
    }
}

// Separate helper functions for complex expression types
void codegen_generate_func_call(Codegen* gen, Expression* expr) {
    char buffer[256];
    // Simplified - full version in original codegen.c
    static const char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    
    // Push args to stack first
    for (int i = 0; i < expr->func_call.arg_count; i++) {
        codegen_generate_expression_value(gen, expr->func_call.args[i]);
        codegen_emit(gen, "    push rax");
    }
    
    // Pop into registers
    for (int i = expr->func_call.arg_count - 1; i >= 0 && i < 6; i--) {
        snprintf(buffer, sizeof(buffer), "    pop %s", arg_regs[expr->func_call.arg_count - 1 - i]);
        codegen_emit(gen, buffer);
    }
    
    // Check if builtin
    if (is_builtin_function(expr->func_call.func_name)) {
        snprintf(buffer, sizeof(buffer), "    call %s", expr->func_call.func_name);
    } else {
        snprintf(buffer, sizeof(buffer), "    call func_%s", expr->func_call.func_name);
    }
    codegen_emit(gen, buffer);
}

void codegen_generate_field_access(Codegen* gen, Expression* expr) {
    char buffer[256];
    // Check enum first
    EnumValue* ev = gen->enums;
    while (ev) {
        if (strcmp(ev->enum_name, expr->field_access.object_name) == 0 &&
            strcmp(ev->member_name, expr->field_access.field_name) == 0) {
            snprintf(buffer, sizeof(buffer), "    mov rax, %ld", ev->value);
            codegen_emit(gen, buffer);
            return;
        }
        ev = ev->next;
    }
    
    // Struct field access
    VarInfo* var = codegen_get_variable(gen, expr->field_access.object_name);
    if (var && var->struct_name) {
        StructInfo* si = codegen_find_struct(gen, var->struct_name);
        if (si) {
            for (int i = 0; i < si->field_count; i++) {
                if (strcmp(si->field_names[i], expr->field_access.field_name) == 0) {
                    int offset = var->stack_offset - si->field_offsets[i];
                    snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]", offset);
                    codegen_emit(gen, buffer);
                    return;
                }
            }
        }
    }
}

void codegen_generate_array_index(Codegen* gen, Expression* expr) {
    char buffer[256];
    VarInfo* var = codegen_get_variable(gen, expr->array_index.array_name);
    codegen_generate_expression_value(gen, expr->array_index.index);
    
    if (var->is_dynamic_array) {
        codegen_emit(gen, "    push rax");
        snprintf(buffer, sizeof(buffer), "    mov rbx, [rbp-%d]", var->stack_offset);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    pop rax");
        codegen_emit(gen, "    imul rax, 8");
        codegen_emit(gen, "    add rbx, rax");
        codegen_emit(gen, "    mov rax, [rbx]");
    } else {
        codegen_emit(gen, "    imul rax, 8");
        snprintf(buffer, sizeof(buffer), "    mov rbx, %d", var->stack_offset);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    sub rbx, rax");
        codegen_emit(gen, "    neg rbx");
        codegen_emit(gen, "    mov rax, [rbp + rbx]");
    }
}

void codegen_generate_address_of(Codegen* gen, Expression* expr) {
    char buffer[256];
    if (expr->unary_operand->type == EXPR_VARIABLE) {
        int offset = codegen_find_variable(gen, expr->unary_operand->var_name);
        snprintf(buffer, sizeof(buffer), "    lea rax, [rbp-%d]", offset);
        codegen_emit(gen, buffer);
    }
}

void codegen_generate_logical_and(Codegen* gen, Expression* expr) {
    char buffer[256];
    int false_label = gen->label_counter++;
    int end_label = gen->label_counter++;
    
    codegen_generate_expression_value(gen, expr->logical_binary.left);
    codegen_emit(gen, "    test rax, rax");
    snprintf(buffer, sizeof(buffer), "    jz .L%d", false_label);
    codegen_emit(gen, buffer);
    
    codegen_generate_expression_value(gen, expr->logical_binary.right);
    codegen_emit(gen, "    test rax, rax");
    snprintf(buffer, sizeof(buffer), "    jz .L%d", false_label);
    codegen_emit(gen, buffer);
    
    codegen_emit(gen, "    mov rax, 1");
    snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
    codegen_emit(gen, buffer);
    
    snprintf(buffer, sizeof(buffer), ".L%d:", false_label);
    codegen_emit(gen, buffer);
    codegen_emit(gen, "    mov rax, 0");
    
    snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
    codegen_emit(gen, buffer);
}

void codegen_generate_logical_or(Codegen* gen, Expression* expr) {
    char buffer[256];
    int true_label = gen->label_counter++;
    int end_label = gen->label_counter++;
    
    codegen_generate_expression_value(gen, expr->logical_binary.left);
    codegen_emit(gen, "    test rax, rax");
    snprintf(buffer, sizeof(buffer), "    jnz .L%d", true_label);
    codegen_emit(gen, buffer);
    
    codegen_generate_expression_value(gen, expr->logical_binary.right);
    codegen_emit(gen, "    test rax, rax");
    snprintf(buffer, sizeof(buffer), "    jnz .L%d", true_label);
    codegen_emit(gen, buffer);
    
    codegen_emit(gen, "    mov rax, 0");
    snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
    codegen_emit(gen, buffer);
    
    snprintf(buffer, sizeof(buffer), ".L%d:", true_label);
    codegen_emit(gen, buffer);
    codegen_emit(gen, "    mov rax, 1");
    
    snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
    codegen_emit(gen, buffer);
}

void codegen_generate_lambda(Codegen* gen, Expression* expr) {
    char buffer[256];
    char lambda_name[64];
    int skip_label = gen->label_counter++;
    
    snprintf(lambda_name, sizeof(lambda_name), "__lambda_%d", gen->label_counter++);
    
    snprintf(buffer, sizeof(buffer), "    jmp __lambda_skip_%d", skip_label);
    codegen_emit(gen, buffer);
    
    // Lambda body generation - simplified
    snprintf(buffer, sizeof(buffer), "%s:", lambda_name);
    codegen_emit(gen, buffer);
    codegen_emit(gen, "    push rbp");
    codegen_emit(gen, "    mov rbp, rsp");
    codegen_generate_expression_value(gen, expr->lambda.body);
    codegen_emit(gen, "    leave");
    codegen_emit(gen, "    ret");
    
    snprintf(buffer, sizeof(buffer), "__lambda_skip_%d:", skip_label);
    codegen_emit(gen, buffer);
    
    snprintf(buffer, sizeof(buffer), "    lea rax, [rel %s]", lambda_name);
    codegen_emit(gen, buffer);
}

void codegen_generate_optional_chain(Codegen* gen, Expression* expr) {
    char buffer[256];
    int null_label = gen->label_counter++;
    int end_label = gen->label_counter++;
    
    codegen_generate_expression_value(gen, expr->optional_chain.object);
    codegen_emit(gen, "    test rax, rax");
    snprintf(buffer, sizeof(buffer), "    jz .L%d", null_label);
    codegen_emit(gen, buffer);
    
    // Access field if not null (simplified)
    snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
    codegen_emit(gen, buffer);
    
    snprintf(buffer, sizeof(buffer), ".L%d:", null_label);
    codegen_emit(gen, buffer);
    codegen_emit(gen, "    mov rax, 0");
    
    snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
    codegen_emit(gen, buffer);
}

void codegen_generate_null_coalesce(Codegen* gen, Expression* expr) {
    char buffer[256];
    int default_label = gen->label_counter++;
    int end_label = gen->label_counter++;
    
    codegen_generate_expression_value(gen, expr->null_coalesce.left);
    codegen_emit(gen, "    test rax, rax");
    snprintf(buffer, sizeof(buffer), "    jz .L%d", default_label);
    codegen_emit(gen, buffer);
    
    snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
    codegen_emit(gen, buffer);
    
    snprintf(buffer, sizeof(buffer), ".L%d:", default_label);
    codegen_emit(gen, buffer);
    codegen_generate_expression_value(gen, expr->null_coalesce.right);
    
    snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
    codegen_emit(gen, buffer);
}
