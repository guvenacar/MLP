// ============================================================================
// MELP Modular Codegen - codegen_expr.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// İçerik: is_double_expression, codegen_generate_expression_value
// İfade değerlerini hesaplar (arithmetic, call, array, lambda, struct, pointer, etc.)
// ~1400 satır - en büyük modül

// TTO: Helper - Determine if expression is double type
int is_double_expression(Codegen* gen, Expression* expr) {
    if (expr->type == EXPR_NUMBER) {
        return expr->internal_numeric_type == INTERNAL_DOUBLE || expr->has_decimal_point;
    }
    if (expr->type == EXPR_VARIABLE) {
        // Check state variables
        for (int i = 0; i < gen->global_var_count; i++) {
            if (strcmp(gen->global_vars[i].name, expr->var_name) == 0) {
                // State variables currently don't track TTO type
                return 0;
            }
        }
        VarInfo* var = codegen_get_variable(gen, expr->var_name);
        return var->internal_numeric_type == INTERNAL_DOUBLE;
    }
    // For binary operations, if either operand is double, result is double
    if (expr->type == EXPR_BINARY_OP) {
        return is_double_expression(gen, expr->binary_op.left) || 
               is_double_expression(gen, expr->binary_op.right);
    }
    return 0;
}

void codegen_generate_expression_value(Codegen* gen, Expression* expr) {
    char buffer[512];
    
    if (expr->type == EXPR_NUMBER) {
        // TTO: Generate code based on internal numeric type
        if (expr->internal_numeric_type == INTERNAL_DOUBLE || expr->has_decimal_point) {
            // Double value - use XMM registers
            // Store double in data section and load it
            codegen_emit(gen, "    ; TTO: Loading double value");
            
            // Convert double to its bit representation for movabs
            union {
                double d;
                unsigned long bits;
            } converter;
            converter.d = expr->double_value;
            
            snprintf(buffer, sizeof(buffer), "    mov rax, 0x%lx    ; double %.15g bit pattern", 
                     converter.bits, expr->double_value);
            codegen_emit(gen, buffer);
            // Note: For now we keep doubles in RAX as bit patterns
            // Full floating point support would need XMM registers
        } else {
            // TTO: int64 - fastest path
            snprintf(buffer, sizeof(buffer), "    mov rax, %ld", expr->number_value);
            codegen_emit(gen, buffer);
        }
    } else if (expr->type == EXPR_NULL) {
        // null literal - represented as 0
        codegen_emit(gen, "    mov rax, 0");
    } else if (expr->type == EXPR_STRING) {
        // String literal - add to data section and reference it
        int str_id = codegen_add_string(gen, expr->string_value);
        snprintf(buffer, sizeof(buffer), "    mov rax, str_%d", str_id);
        codegen_emit(gen, buffer);
    } else if (expr->type == EXPR_VARIABLE) {
        // Phase 18: First check if this is a state variable
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
                // Captured variable from closure environment
                // offset = -(1000 + index), so index = -(offset + 1000)
                int capture_index = -(var->stack_offset + 1000);
                int env_offset = 8;  // Closure env pushed first (push rdi)
                
                snprintf(buffer, sizeof(buffer), 
                        "    ; Load captured variable %s from environment[%d]", 
                        expr->var_name, capture_index);
                codegen_emit(gen, buffer);
                snprintf(buffer, sizeof(buffer), "    mov r13, [rbp-%d]   ; Environment pointer", env_offset);
                codegen_emit(gen, buffer);
                snprintf(buffer, sizeof(buffer), "    mov rax, [r13+%d]", capture_index * 8);
                codegen_emit(gen, buffer);
            } else {
                // Check if this is an array variable (for destructuring)
                // If the variable is declared as array, we need the address, not the value
                // This is a heuristic: if offset is large (>8), likely an array
                // Better: check VarInfo for is_array flag (but we don't have that)
                // For now, just return the value - destructuring will handle this differently
                
                // Regular stack variable
                snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]", var->stack_offset);
                codegen_emit(gen, buffer);
            }
        }
    } else if (expr->type == EXPR_BINARY_OP) {
        // Check if this is string concatenation
        if (expr->binary_op.op == BIN_OP_ADD && 
            (is_string_expression(gen, expr->binary_op.left) || 
             is_string_expression(gen, expr->binary_op.right))) {
            // String concatenation
            int left_is_string = is_string_expression(gen, expr->binary_op.left);
            int right_is_string = is_string_expression(gen, expr->binary_op.right);
            
            // Evaluate left side
            codegen_generate_expression_value(gen, expr->binary_op.left);
            // If left is numeric, convert to string
            if (!left_is_string) {
                codegen_emit(gen, "    mov rdi, rax");
                codegen_emit(gen, "    call int_to_string");
            }
            codegen_emit(gen, "    push rax");
            
            // Evaluate right side
            codegen_generate_expression_value(gen, expr->binary_op.right);
            // If right is numeric, convert to string
            if (!right_is_string) {
                codegen_emit(gen, "    mov rdi, rax");
                codegen_emit(gen, "    call int_to_string");
            }
            codegen_emit(gen, "    mov rsi, rax");  // second arg
            codegen_emit(gen, "    pop rdi");       // first arg
            
            // Call string_concat(rdi, rsi) -> returns new string in rax
            codegen_emit(gen, "    call string_concat");
            // Result pointer is in rax
        } else {
            // Phase 16: Check for struct operator overloading first
            char* left_struct_type = NULL;
            
            // Check if left operand is a struct variable (skip for state variables)
            if (expr->binary_op.left->type == EXPR_VARIABLE) {
                // Phase 18: Check if state variable first
                int is_state = 0;
                for (int i = 0; i < gen->global_var_count; i++) {
                    if (strcmp(gen->global_vars[i].name, expr->binary_op.left->var_name) == 0) {
                        is_state = 1;
                        break;
                    }
                }
                if (!is_state) {
                    VarInfo* var = codegen_get_variable(gen, expr->binary_op.left->var_name);
                    if (var && var->struct_name != NULL) {
                        left_struct_type = var->struct_name;
                    }
                }
            }
            
            // Check if we have an operator overload for this struct type
            OperatorOverload* struct_overload = NULL;
            if (left_struct_type && !gen->in_operator_overload) {
                const char* op_str = NULL;
                switch (expr->binary_op.op) {
                    case BIN_OP_ADD: op_str = "+"; break;
                    case BIN_OP_SUB: op_str = "-"; break;
                    case BIN_OP_MUL: op_str = "*"; break;
                    case BIN_OP_DIV: op_str = "/"; break;
                    default: break;
                }
                if (op_str) {
                    struct_overload = codegen_find_operator_overload(gen, op_str, left_struct_type);
                }
            }
            
            if (struct_overload) {
                // Phase 16: Call operator overload function for struct
                codegen_emit(gen, "    ; Struct operator overload call");
                
                // Get left operand address (pointer to struct)
                if (expr->binary_op.left->type == EXPR_VARIABLE) {
                    // Phase 18: Check if it's a state variable first
                    int is_state_left = 0;
                    for (int i = 0; i < gen->global_var_count; i++) {
                        if (strcmp(gen->global_vars[i].name, expr->binary_op.left->var_name) == 0) {
                            is_state_left = 1;
                            snprintf(buffer, sizeof(buffer), "    lea rdi, [state_%s]   ; Address of state left operand", expr->binary_op.left->var_name);
                            codegen_emit(gen, buffer);
                            break;
                        }
                    }
                    if (!is_state_left) {
                        VarInfo* var = codegen_get_variable(gen, expr->binary_op.left->var_name);
                        snprintf(buffer, sizeof(buffer), "    lea rdi, [rbp-%d]   ; Address of left operand", var->stack_offset);
                        codegen_emit(gen, buffer);
                    }
                }
                codegen_emit(gen, "    push rdi");
                
                // Get right operand address
                if (expr->binary_op.right->type == EXPR_VARIABLE) {
                    // Phase 18: Check if it's a state variable first
                    int is_state_right = 0;
                    for (int i = 0; i < gen->global_var_count; i++) {
                        if (strcmp(gen->global_vars[i].name, expr->binary_op.right->var_name) == 0) {
                            is_state_right = 1;
                            snprintf(buffer, sizeof(buffer), "    lea rsi, [state_%s]   ; Address of state right operand", expr->binary_op.right->var_name);
                            codegen_emit(gen, buffer);
                            break;
                        }
                    }
                    if (!is_state_right) {
                        VarInfo* var = codegen_get_variable(gen, expr->binary_op.right->var_name);
                        snprintf(buffer, sizeof(buffer), "    lea rsi, [rbp-%d]   ; Address of right operand", var->stack_offset);
                        codegen_emit(gen, buffer);
                    }
                }
                codegen_emit(gen, "    pop rdi");
                
                // Call operator function
                snprintf(buffer, sizeof(buffer), "    call %s", struct_overload->func_name);
                codegen_emit(gen, buffer);
                // Result in rax
            } else {
                // Check for operator overloading (only if not already in operator overload function)
                char overload_func[256];
                const char* op_name = NULL;
                FuncInfo* overload = NULL;
            
            if (!gen->in_operator_overload) {
                // For now, only support numeric operator overloading
                switch (expr->binary_op.op) {
                    case BIN_OP_ADD: op_name = "add"; break;
                    case BIN_OP_SUB: op_name = "sub"; break;
                    case BIN_OP_MUL: op_name = "mul"; break;
                    case BIN_OP_DIV: op_name = "div"; break;
                }
                
                snprintf(overload_func, sizeof(overload_func), "numeric_operator_%s", op_name);
                
                // Check if operator overload function exists
                overload = codegen_find_function(gen, overload_func);
            }
            
            if (overload && overload->param_count == 2) {
                // Use operator overload function
                // Evaluate left side
                codegen_generate_expression_value(gen, expr->binary_op.left);
                codegen_emit(gen, "    push rax");
                
                // Evaluate right side
                codegen_generate_expression_value(gen, expr->binary_op.right);
                codegen_emit(gen, "    push rax");
                
                // Call overload function
                snprintf(buffer, sizeof(buffer), "    call func_%s", overload_func);
                codegen_emit(gen, buffer);
                codegen_emit(gen, "    add rsp, 16   ; Clean up parameters");
                // Result is in rax
            } else {
                // TTO: Check if this is double arithmetic
                int left_is_double = is_double_expression(gen, expr->binary_op.left);
                int right_is_double = is_double_expression(gen, expr->binary_op.right);
                int use_double = left_is_double || right_is_double;
                
                if (use_double) {
                    // TTO: Double arithmetic using SSE
                    codegen_emit(gen, "    ; TTO: Double arithmetic");
                    
                    // Evaluate left side
                    codegen_generate_expression_value(gen, expr->binary_op.left);
                    if (!left_is_double) {
                        // Convert int64 to double
                        codegen_emit(gen, "    cvtsi2sd xmm0, rax   ; Convert int to double");
                    } else {
                        codegen_emit(gen, "    movq xmm0, rax        ; Load double bit pattern");
                    }
                    codegen_emit(gen, "    sub rsp, 8");
                    codegen_emit(gen, "    movsd [rsp], xmm0      ; Save left operand");
                    
                    // Evaluate right side
                    codegen_generate_expression_value(gen, expr->binary_op.right);
                    if (!right_is_double) {
                        // Convert int64 to double
                        codegen_emit(gen, "    cvtsi2sd xmm1, rax   ; Convert int to double");
                    } else {
                        codegen_emit(gen, "    movq xmm1, rax        ; Load double bit pattern");
                    }
                    codegen_emit(gen, "    movsd xmm0, [rsp]      ; Restore left operand");
                    codegen_emit(gen, "    add rsp, 8");
                    
                    // Perform double operation
                    switch (expr->binary_op.op) {
                        case BIN_OP_ADD:
                            codegen_emit(gen, "    addsd xmm0, xmm1     ; Double add");
                            break;
                        case BIN_OP_SUB:
                            codegen_emit(gen, "    subsd xmm0, xmm1     ; Double subtract");
                            break;
                        case BIN_OP_MUL:
                            codegen_emit(gen, "    mulsd xmm0, xmm1     ; Double multiply");
                            break;
                        case BIN_OP_DIV:
                            codegen_emit(gen, "    divsd xmm0, xmm1     ; Double divide");
                            break;
                    }
                    
                    // Store result as bit pattern in rax (for consistency)
                    codegen_emit(gen, "    movq rax, xmm0         ; Result to rax as bit pattern");
                } else {
                    // Integer operations (default)
                    // Evaluate left side
                    codegen_generate_expression_value(gen, expr->binary_op.left);
                    codegen_emit(gen, "    push rax");
                    
                    // Evaluate right side
                    codegen_generate_expression_value(gen, expr->binary_op.right);
                    codegen_emit(gen, "    mov rbx, rax");
                    codegen_emit(gen, "    pop rax");
                    
                    // Perform integer operation
                    switch (expr->binary_op.op) {
                        case BIN_OP_ADD:
                            codegen_emit(gen, "    add rax, rbx");
                            break;
                        case BIN_OP_SUB:
                            codegen_emit(gen, "    sub rax, rbx");
                            break;
                        case BIN_OP_MUL:
                            codegen_emit(gen, "    imul rax, rbx");
                            break;
                        case BIN_OP_DIV:
                            codegen_emit(gen, "    cqo");  // Sign extend rax to rdx:rax
                            codegen_emit(gen, "    idiv rbx");
                            break;
                    }
                }
            }
            }
        }
    } else if (expr->type == EXPR_FUNC_CALL) {
        // Function call
        char buffer[256];
        
        // Phase 13: Check if this is a generic function call
        char* actual_func_name = expr->func_call.func_name;
        if (expr->func_call.type_arg_count > 0) {
            // Generic call - need to instantiate or use existing instance
            GenericFunc* gf = codegen_find_generic_func(gen, expr->func_call.func_name);
            if (gf) {
                // Check if this instance already exists
                GenericInstance* gi = codegen_find_generic_instance(gen, 
                    expr->func_call.func_name, 
                    expr->func_call.type_args, 
                    expr->func_call.type_arg_count);
                
                if (!gi) {
                    // Need to register this instance for later generation
                    char* mangled = codegen_mangle_generic_name(expr->func_call.func_name,
                        expr->func_call.type_args, expr->func_call.type_arg_count);
                    
                    // Register instance (will be generated in codegen_generate)
                    codegen_add_generic_instance(gen, expr->func_call.func_name, mangled,
                        expr->func_call.type_args, expr->func_call.type_arg_count);
                    
                    actual_func_name = mangled;
                } else {
                    actual_func_name = gi->mangled_name;
                }
            }
        }
        
        // Check for built-in functions
        int is_builtin = 0;
        const char* builtin_name = NULL;
        
        if (strcmp(expr->func_call.func_name, "length") == 0) {
            is_builtin = 1;
            builtin_name = "string_length";
            
            // Check if argument is array (use mlp_array_length) or string
            // For now, assume string_length - array version will be mlp_array_length
            if (expr->func_call.arg_count > 0) {
                // TODO: Determine if arg is array or string at compile time
                // For simplicity, use string_length for now
            }
        } else if (strcmp(expr->func_call.func_name, "malloc") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_array_alloc";
        } else if (strcmp(expr->func_call.func_name, "free") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_array_free";
        } else if (strcmp(expr->func_call.func_name, "resize") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_array_resize";
        } else if (strcmp(expr->func_call.func_name, "read_file") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_file_read";
        } else if (strcmp(expr->func_call.func_name, "write_file") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_file_write";
        } else if (strcmp(expr->func_call.func_name, "file_exists") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_file_exists";
        } else if (strcmp(expr->func_call.func_name, "append_file") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_file_append";
        } else if (strcmp(expr->func_call.func_name, "substring") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_substring";
        } else if (strcmp(expr->func_call.func_name, "indexOf") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_indexOf";
        } else if (strcmp(expr->func_call.func_name, "charAt") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_charAt";
        } else if (strcmp(expr->func_call.func_name, "str_length") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_string_length";
        } else if (strcmp(expr->func_call.func_name, "get_argc") == 0 ||
                   strcmp(expr->func_call.func_name, "mlp_get_argc") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_get_argc";
        } else if (strcmp(expr->func_call.func_name, "range") == 0) {
            // Phase 14: range() built-in for iterators
            is_builtin = 1;
            if (expr->func_call.arg_count == 1) {
                builtin_name = "mlp_range1";      // range(end)
            } else if (expr->func_call.arg_count == 2) {
                builtin_name = "mlp_range2";      // range(start, end)
            } else {
                builtin_name = "mlp_range";       // range(start, end, step)
            }
        // Phase 19: GC built-in functions
        } else if (strcmp(expr->func_call.func_name, "gc_collect") == 0) {
            is_builtin = 1;
            builtin_name = "gc_collect";
        } else if (strcmp(expr->func_call.func_name, "gc_full_collect") == 0) {
            is_builtin = 1;
            builtin_name = "gc_full_collect";
        } else if (strcmp(expr->func_call.func_name, "gc_get_object_count") == 0) {
            is_builtin = 1;
            builtin_name = "gc_get_object_count";
        } else if (strcmp(expr->func_call.func_name, "gc_get_total_bytes") == 0) {
            is_builtin = 1;
            builtin_name = "gc_get_total_bytes";
        } else if (strcmp(expr->func_call.func_name, "gc_get_collections") == 0) {
            is_builtin = 1;
            builtin_name = "gc_get_collections";
        } else if (strcmp(expr->func_call.func_name, "gc_enable") == 0) {
            is_builtin = 1;
            builtin_name = "gc_enable";
        } else if (strcmp(expr->func_call.func_name, "gc_disable") == 0) {
            is_builtin = 1;
            builtin_name = "gc_disable";
        // Phase 20: Type conversion functions
        } else if (strcmp(expr->func_call.func_name, "to_numeric") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_to_numeric";
        } else if (strcmp(expr->func_call.func_name, "to_string") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_to_string";
        } else if (strcmp(expr->func_call.func_name, "to_decimal") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_to_decimal";
        } else if (strcmp(expr->func_call.func_name, "parse_int") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_parse_int";
        } else if (strcmp(expr->func_call.func_name, "parse_float") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_parse_float";
        // Phase 20: Input functions
        } else if (strcmp(expr->func_call.func_name, "input") == 0) {
            is_builtin = 1;
            if (expr->func_call.arg_count == 0) {
                builtin_name = "mlp_input";
            } else {
                builtin_name = "mlp_input_prompt";
            }
        // Phase 20: Math functions
        } else if (strcmp(expr->func_call.func_name, "sin") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_sin";
        } else if (strcmp(expr->func_call.func_name, "cos") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_cos";
        } else if (strcmp(expr->func_call.func_name, "tan") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_tan";
        } else if (strcmp(expr->func_call.func_name, "sqrt") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_sqrt";
        } else if (strcmp(expr->func_call.func_name, "pow") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_pow";
        } else if (strcmp(expr->func_call.func_name, "abs") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_abs_int";  // For integer abs
        } else if (strcmp(expr->func_call.func_name, "floor") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_floor";
        } else if (strcmp(expr->func_call.func_name, "ceil") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_ceil";
        } else if (strcmp(expr->func_call.func_name, "round") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_round";
        } else if (strcmp(expr->func_call.func_name, "log") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_log";
        } else if (strcmp(expr->func_call.func_name, "log10") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_log10";
        } else if (strcmp(expr->func_call.func_name, "exp") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_exp";
        } else if (strcmp(expr->func_call.func_name, "fmod") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_fmod";
        } else if (strcmp(expr->func_call.func_name, "pi") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_pi";
        } else if (strcmp(expr->func_call.func_name, "e") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_e";
        // Phase 20: String functions
        } else if (strcmp(expr->func_call.func_name, "trim") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_trim";
        } else if (strcmp(expr->func_call.func_name, "replace") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_replace";
        } else if (strcmp(expr->func_call.func_name, "to_upper") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_to_upper";
        } else if (strcmp(expr->func_call.func_name, "to_lower") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_to_lower";
        } else if (strcmp(expr->func_call.func_name, "starts_with") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_starts_with";
        } else if (strcmp(expr->func_call.func_name, "ends_with") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_ends_with";
        } else if (strcmp(expr->func_call.func_name, "contains") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_contains";
        // Phase 20: Assert function
        } else if (strcmp(expr->func_call.func_name, "assert") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_assert";
        // Phase 20: Bitwise operations
        } else if (strcmp(expr->func_call.func_name, "band") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_band";
        } else if (strcmp(expr->func_call.func_name, "bor") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_bor";
        } else if (strcmp(expr->func_call.func_name, "bxor") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_bxor";
        } else if (strcmp(expr->func_call.func_name, "bnot") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_bnot";
        } else if (strcmp(expr->func_call.func_name, "shl") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_shl";
        } else if (strcmp(expr->func_call.func_name, "shr") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_shr";
        } else if (strcmp(expr->func_call.func_name, "ushr") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_ushr";
        }
        
        if (is_builtin) {
            // Built-in function - use System V AMD64 ABI calling convention
            // Arguments go in: rdi, rsi, rdx, rcx, r8, r9
            // FIXED: Push all args to stack first to avoid overwriting registers
            
            // Evaluate all arguments and push to stack
            for (int i = 0; i < expr->func_call.arg_count; i++) {
                codegen_generate_expression_value(gen, expr->func_call.args[i]);
                codegen_emit(gen, "    push rax");
            }
            
            // Now pop them into the correct registers (in reverse order)
            if (expr->func_call.arg_count > 2) {
                codegen_emit(gen, "    pop rdx");  // Third arg
            }
            if (expr->func_call.arg_count > 1) {
                codegen_emit(gen, "    pop rsi");  // Second arg
            }
            if (expr->func_call.arg_count > 0) {
                codegen_emit(gen, "    pop rdi");  // First arg
            }
            
            snprintf(buffer, sizeof(buffer), "    call %s", builtin_name);
            codegen_emit(gen, buffer);
            // Result is in rax
        } else {
            // User-defined function OR lambda call
            // Check if func_name is a variable (lambda)
            VarInfo* lambda_var = NULL;
            VarInfo* v = gen->variables;
            while (v) {
                if (strcmp(v->name, expr->func_call.func_name) == 0) {
                    lambda_var = v;
                    break;
                }
                v = v->next;
            }
            
            if (lambda_var) {
                // Lambda call: variable holds closure structure OR function pointer
                codegen_emit(gen, "    ; Lambda/closure call with runtime detection");
                
                // Push arguments to stack (System V ABI: rdi, rsi, rdx, rcx)
                const char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx"};
                
                // Load closure pointer/function pointer
                snprintf(buffer, sizeof(buffer), "    mov r15, [rbp-%d]   ; Load closure/lambda", 
                         lambda_var->stack_offset);
                codegen_emit(gen, buffer);
                
                // Save r15 (closure pointer)
                codegen_emit(gen, "    push r15");
                
                // Evaluate arguments and save to temp storage
                for (int i = 0; i < expr->func_call.arg_count; i++) {
                    codegen_generate_expression_value(gen, expr->func_call.args[i]);
                    codegen_emit(gen, "    push rax");
                }
                
                // Restore closure pointer
                snprintf(buffer, sizeof(buffer), "    mov r15, [rsp+%d]", expr->func_call.arg_count * 8);
                codegen_emit(gen, buffer);
                
                // Runtime closure detection: check magic number
                codegen_emit(gen, "    ; Check if closure by testing magic number (0xC105UR3 = 202182115)");
                codegen_emit(gen, "    mov r14, [r15]      ; Load first qword");
                codegen_emit(gen, "    cmp r14, 202182115  ; Magic number for closures");
                
                // Generate unique labels
                int closure_label = gen->label_counter++;
                int done_label = gen->label_counter++;
                
                snprintf(buffer, sizeof(buffer), "    je .L_closure_%d    ; Jump if closure", closure_label);
                codegen_emit(gen, buffer);
                
                // Plain function pointer path
                codegen_emit(gen, "    ; Plain function pointer call");
                
                // Pop arguments into registers (use all 4: rdi, rsi, rdx, rcx)
                for (int i = expr->func_call.arg_count - 1; i >= 0; i--) {
                    if (i < 4) {
                        snprintf(buffer, sizeof(buffer), "    pop %s", arg_regs[i]);
                        codegen_emit(gen, buffer);
                    } else {
                        codegen_emit(gen, "    pop rax");  // Clean stack
                    }
                }
                
                // Remove saved function pointer from stack
                codegen_emit(gen, "    add rsp, 8");
                
                // Call function directly (r15 is function pointer)
                codegen_emit(gen, "    call r15");
                
                snprintf(buffer, sizeof(buffer), "    jmp .L_done_%d", done_label);
                codegen_emit(gen, buffer);
                
                // Closure path
                snprintf(buffer, sizeof(buffer), ".L_closure_%d:", closure_label);
                codegen_emit(gen, buffer);
                codegen_emit(gen, "    ; Closure call: [magic, func_ptr, env_ptr]");
                
                // Load function pointer and environment pointer
                codegen_emit(gen, "    mov r14, [r15+8]    ; Function pointer");
                codegen_emit(gen, "    mov rdi, [r15+16]   ; Environment pointer (first arg)");
                
                // Pop arguments into registers (skip rdi, it's for environment)
                for (int i = expr->func_call.arg_count - 1; i >= 0; i--) {
                    if (i < 3) {  // rsi, rdx, rcx available (rdi used for env)
                        snprintf(buffer, sizeof(buffer), "    pop %s", arg_regs[i + 1]);
                        codegen_emit(gen, buffer);
                    } else {
                        codegen_emit(gen, "    pop rax");  // Clean stack
                    }
                }
                
                // Remove saved closure pointer from stack
                codegen_emit(gen, "    add rsp, 8");
                
                // Call function
                codegen_emit(gen, "    call r14");
                
                snprintf(buffer, sizeof(buffer), ".L_done_%d:", done_label);
                codegen_emit(gen, buffer);
                
                // Result is in rax
            } else {
                // Regular user-defined or builtin function
                
                // Check if it's a builtin function
                int is_builtin = 0;
                char* func_label = NULL;
                
                if (strchr(actual_func_name, '.') != NULL) {
                    // Module qualified call: Math.add -> Math_add
                    func_label = malloc(strlen(actual_func_name) + 1);
                    strcpy(func_label, actual_func_name);
                    // Replace dot with underscore
                    for (char* p = func_label; *p; p++) {
                        if (*p == '.') *p = '_';
                    }
                } else if (is_builtin_function(actual_func_name)) {
                    // Builtin/runtime function - use as-is without func_ prefix
                    func_label = malloc(strlen(actual_func_name) + 1);
                    strcpy(func_label, actual_func_name);
                    is_builtin = 1;
                } else {
                    // Regular user function call: func_name -> func_func_name
                    func_label = malloc(strlen(actual_func_name) + 6);
                    sprintf(func_label, "func_%s", actual_func_name);
                }
                
                if (is_builtin) {
                    // Builtin functions use System V ABI (rdi, rsi, rdx, rcx, r8, r9)
                    // Generate arguments and store in registers
                    static const char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
                    
                    // Evaluate arguments right-to-left and store in temp locations
                    for (int i = expr->func_call.arg_count - 1; i >= 0; i--) {
                        codegen_generate_expression_value(gen, expr->func_call.args[i]);
                        codegen_emit(gen, "    push rax");
                    }
                    
                    // Pop arguments into registers (left-to-right)
                    for (int i = 0; i < expr->func_call.arg_count && i < 6; i++) {
                        snprintf(buffer, sizeof(buffer), "    pop %s", arg_regs[i]);
                        codegen_emit(gen, buffer);
                    }
                    
                    // Call function
                    snprintf(buffer, sizeof(buffer), "    call %s", func_label);
                    codegen_emit(gen, buffer);
                    
                    // Result is in rax
                } else {
                    // User-defined function - push arguments in reverse order (right to left)
                    for (int i = expr->func_call.arg_count - 1; i >= 0; i--) {
                        codegen_generate_expression_value(gen, expr->func_call.args[i]);
                        codegen_emit(gen, "    push rax");
                    }
                    
                    // Call function
                    snprintf(buffer, sizeof(buffer), "    call %s", func_label);
                    codegen_emit(gen, buffer);
                    
                    // Clean up stack (pop arguments)
                    if (expr->func_call.arg_count > 0) {
                        snprintf(buffer, sizeof(buffer), "    add rsp, %d", expr->func_call.arg_count * 8);
                        codegen_emit(gen, buffer);
                    }
                }
                
                free(func_label);
                
                // Result is in rax
            }
        }
    } else if (expr->type == EXPR_FIELD_ACCESS) {
        // Field access: object.field OR Enum.Member OR Module.function
        // First check if it's an enum value
        EnumValue* ev = gen->enums;
        int found_enum = 0;
        while (ev) {
            if (strcmp(ev->enum_name, expr->field_access.object_name) == 0 &&
                strcmp(ev->member_name, expr->field_access.field_name) == 0) {
                snprintf(buffer, sizeof(buffer), "    mov rax, %ld   ; %s.%s", 
                         ev->value, expr->field_access.object_name, expr->field_access.field_name);
                codegen_emit(gen, buffer);
                found_enum = 1;
                break;
            }
            ev = ev->next;
        }
        
        if (!found_enum) {
            // Check if it's a module function reference (e.g., Math.add)
            // Module function references are just identifiers, not actual values
            // They will be used in function calls like: Math.add(5, 3)
            // For now, we can't load a function as a value, so this is likely
            // used in a function call expression which will be handled separately
            
            // Try to find as struct field access
            VarInfo* var_info = codegen_get_variable(gen, expr->field_access.object_name);
            
            if (!var_info) {
                // Not a variable, could be a module name
                // For now, emit a comment and leave rax unchanged (module.func will be handled in EXPR_FUNC_CALL)
                snprintf(buffer, sizeof(buffer), "    ; Module reference: %s.%s (used in function call)", 
                         expr->field_access.object_name, expr->field_access.field_name);
                codegen_emit(gen, buffer);
                // This case shouldn't normally be reached as module.func is parsed as part of function call
            } else if (!var_info->struct_name) {
                fprintf(stderr, "Codegen error: '%s' is not a struct or enum\n", expr->field_access.object_name);
                exit(1);
            } else {
                // Struct field access
                StructInfo* struct_info = codegen_find_struct(gen, var_info->struct_name);
                if (!struct_info) {
                    fprintf(stderr, "Codegen error: Undefined struct type '%s'\n", var_info->struct_name);
                    exit(1);
                }
                
                // Find field in struct
                int field_index = -1;
                for (int i = 0; i < struct_info->field_count; i++) {
                    if (strcmp(struct_info->field_names[i], expr->field_access.field_name) == 0) {
                        field_index = i;
                        break;
                    }
                }
                
                if (field_index == -1) {
                    fprintf(stderr, "Codegen error: Struct '%s' has no field '%s'\n", 
                            var_info->struct_name, expr->field_access.field_name);
                    exit(1);
                }
                
                // Calculate actual offset: base_offset - field_offset
                int actual_offset = var_info->stack_offset - struct_info->field_offsets[field_index];
                snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]   ; Load %s.%s", 
                         actual_offset, expr->field_access.object_name, expr->field_access.field_name);
                codegen_emit(gen, buffer);
            }
        }
    } else if (expr->type == EXPR_COMPARISON) {
        // Comparison expression - evaluate to 1 (true) or 0 (false)
        codegen_generate_expression_value(gen, expr->comparison.left);
        codegen_emit(gen, "    push rax");
        codegen_generate_expression_value(gen, expr->comparison.right);
        codegen_emit(gen, "    mov rbx, rax");
        codegen_emit(gen, "    pop rax");
        codegen_emit(gen, "    cmp rax, rbx");
        
        // Use conditional move to set rax to 1 or 0
        int label = gen->label_counter++;
        switch (expr->comparison.op) {
            case CMP_EQUAL:
                codegen_emit(gen, "    mov rax, 0      ; Default false");
                codegen_emit(gen, "    mov rbx, 1      ; True value");
                codegen_emit(gen, "    cmove rax, rbx  ; If equal, set rax=1");
                break;
            case CMP_NOT_EQUAL:
                codegen_emit(gen, "    mov rax, 0");
                codegen_emit(gen, "    mov rbx, 1");
                codegen_emit(gen, "    cmovne rax, rbx");
                break;
            case CMP_LESS:
                codegen_emit(gen, "    mov rax, 0");
                codegen_emit(gen, "    mov rbx, 1");
                codegen_emit(gen, "    cmovl rax, rbx");
                break;
            case CMP_LESS_EQUAL:
                codegen_emit(gen, "    mov rax, 0");
                codegen_emit(gen, "    mov rbx, 1");
                codegen_emit(gen, "    cmovle rax, rbx");
                break;
            case CMP_GREATER:
                codegen_emit(gen, "    mov rax, 0");
                codegen_emit(gen, "    mov rbx, 1");
                codegen_emit(gen, "    cmovg rax, rbx");
                break;
            case CMP_GREATER_EQUAL:
                codegen_emit(gen, "    mov rax, 0");
                codegen_emit(gen, "    mov rbx, 1");
                codegen_emit(gen, "    cmovge rax, rbx");
                break;
        }
    } else if (expr->type == EXPR_TERNARY) {
        // Ternary operator: condition ? true_expr : false_expr
        int false_label = gen->label_counter++;
        int end_label = gen->label_counter++;
        
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Ternary operator");
        
        // Evaluate condition
        if (expr->ternary.condition->type == EXPR_COMPARISON) {
            // Use comparison generation
            codegen_generate_comparison(gen, expr->ternary.condition, false_label);
            
            // True branch
            codegen_generate_expression_value(gen, expr->ternary.true_expr);
            snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
            codegen_emit(gen, buffer);
            
            // False branch
            snprintf(buffer, sizeof(buffer), ".L%d:", false_label);
            codegen_emit(gen, buffer);
            codegen_generate_expression_value(gen, expr->ternary.false_expr);
            
            snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
            codegen_emit(gen, buffer);
        } else {
            // Non-comparison condition (treat 0 as false, non-zero as true)
            codegen_generate_expression_value(gen, expr->ternary.condition);
            codegen_emit(gen, "    test rax, rax");
            snprintf(buffer, sizeof(buffer), "    jz .L%d", false_label);
            codegen_emit(gen, buffer);
            
            // True branch
            codegen_generate_expression_value(gen, expr->ternary.true_expr);
            snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
            codegen_emit(gen, buffer);
            
            // False branch
            snprintf(buffer, sizeof(buffer), ".L%d:", false_label);
            codegen_emit(gen, buffer);
            codegen_generate_expression_value(gen, expr->ternary.false_expr);
            
            snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
            codegen_emit(gen, buffer);
        }
    } else if (expr->type == EXPR_OPTIONAL_CHAIN) {
        // Phase 15: Optional chaining: object?.field
        // Returns null (0) if object is null, otherwise object.field
        int null_label = gen->label_counter++;
        int end_label = gen->label_counter++;
        
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Optional chaining (?.)");
        
        // Get struct info from the object's variable
        if (expr->optional_chain.object->type == EXPR_VARIABLE) {
            VarInfo* var_info = codegen_get_variable(gen, expr->optional_chain.object->var_name);
            
            if (var_info && var_info->struct_name) {
                // Find struct definition
                StructInfo* struct_info = codegen_find_struct(gen, var_info->struct_name);
                if (struct_info) {
                    // Find field offset
                    int field_index = -1;
                    for (int i = 0; i < struct_info->field_count; i++) {
                        if (strcmp(struct_info->field_names[i], expr->optional_chain.field_name) == 0) {
                            field_index = i;
                            break;
                        }
                    }
                    
                    if (field_index >= 0) {
                        // For stack-allocated structs, calculate the address first
                        // Load the struct base address into rax
                        snprintf(buffer, sizeof(buffer), "    lea rax, [rbp-%d]   ; Get struct address", 
                                 var_info->stack_offset);
                        codegen_emit(gen, buffer);
                        
                        // Check if null (for pointer types, the value would be 0)
                        // For stack-allocated structs, we check if the first field is 0
                        // as a null indicator (convention: null struct has first field = 0)
                        codegen_emit(gen, "    mov rbx, [rax]      ; Load first field for null check");
                        codegen_emit(gen, "    test rbx, rbx       ; Check if null");
                        snprintf(buffer, sizeof(buffer), "    jz .L_null_%d       ; If null, skip field access", null_label);
                        codegen_emit(gen, buffer);
                        
                        // Not null - access the field
                        // Calculate actual offset: base_offset - field_offset
                        int actual_offset = var_info->stack_offset - struct_info->field_offsets[field_index];
                        snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]   ; Load %s.%s", 
                                 actual_offset, expr->optional_chain.object->var_name, 
                                 expr->optional_chain.field_name);
                        codegen_emit(gen, buffer);
                        
                        snprintf(buffer, sizeof(buffer), "    jmp .L_end_%d", end_label);
                        codegen_emit(gen, buffer);
                        
                        // Null path - return null (0)
                        snprintf(buffer, sizeof(buffer), ".L_null_%d:", null_label);
                        codegen_emit(gen, buffer);
                        codegen_emit(gen, "    mov rax, 0          ; Return null");
                        
                        snprintf(buffer, sizeof(buffer), ".L_end_%d:", end_label);
                        codegen_emit(gen, buffer);
                    } else {
                        fprintf(stderr, "Codegen error: Unknown field '%s' in optional chain\n", 
                                expr->optional_chain.field_name);
                        exit(1);
                    }
                } else {
                    fprintf(stderr, "Codegen error: Undefined struct type '%s'\n", var_info->struct_name);
                    exit(1);
                }
            } else {
                // Not a struct variable - just evaluate and check for null
                codegen_generate_expression_value(gen, expr->optional_chain.object);
                codegen_emit(gen, "    test rax, rax       ; Check if null");
                snprintf(buffer, sizeof(buffer), "    jz .L_null_%d", null_label);
                codegen_emit(gen, buffer);
                
                // Try to access field (this would be for pointer-to-struct types)
                // For now, just return the value
                snprintf(buffer, sizeof(buffer), "    jmp .L_end_%d", end_label);
                codegen_emit(gen, buffer);
                
                snprintf(buffer, sizeof(buffer), ".L_null_%d:", null_label);
                codegen_emit(gen, buffer);
                codegen_emit(gen, "    mov rax, 0          ; Return null");
                
                snprintf(buffer, sizeof(buffer), ".L_end_%d:", end_label);
                codegen_emit(gen, buffer);
            }
        } else {
            // Non-variable expression (e.g., function call result)
            codegen_generate_expression_value(gen, expr->optional_chain.object);
            codegen_emit(gen, "    test rax, rax       ; Check if null");
            snprintf(buffer, sizeof(buffer), "    jz .L_null_%d", null_label);
            codegen_emit(gen, buffer);
            
            snprintf(buffer, sizeof(buffer), "    jmp .L_end_%d", end_label);
            codegen_emit(gen, buffer);
            
            snprintf(buffer, sizeof(buffer), ".L_null_%d:", null_label);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    mov rax, 0          ; Return null");
            
            snprintf(buffer, sizeof(buffer), ".L_end_%d:", end_label);
            codegen_emit(gen, buffer);
        }
    } else if (expr->type == EXPR_NULL_COALESCE) {
        // Phase 15: Null coalescing: value ?? default
        // Returns value if not null, otherwise default
        int use_default_label = gen->label_counter++;
        int end_label = gen->label_counter++;
        
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Null coalescing (??)");
        
        // Evaluate left side (value that might be null)
        codegen_generate_expression_value(gen, expr->null_coalesce.left);
        
        // Check if null (0)
        codegen_emit(gen, "    test rax, rax       ; Check if null");
        snprintf(buffer, sizeof(buffer), "    jz .L_default_%d    ; If null, use default", use_default_label);
        codegen_emit(gen, buffer);
        
        // Not null - keep the value (already in rax)
        snprintf(buffer, sizeof(buffer), "    jmp .L_end_%d", end_label);
        codegen_emit(gen, buffer);
        
        // Null - use default value
        snprintf(buffer, sizeof(buffer), ".L_default_%d:", use_default_label);
        codegen_emit(gen, buffer);
        codegen_generate_expression_value(gen, expr->null_coalesce.right);
        
        snprintf(buffer, sizeof(buffer), ".L_end_%d:", end_label);
        codegen_emit(gen, buffer);
    } else if (expr->type == EXPR_ARRAY_INDEX) {
        // Array indexing: arr[index]
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Array indexing");
        
        // Get array variable info
        VarInfo* var_info = codegen_get_variable(gen, expr->array_index.array_name);
        
        // Evaluate index expression
        codegen_generate_expression_value(gen, expr->array_index.index);
        
        if (var_info->is_dynamic_array) {
            // Dynamic array: load pointer, then index into it
            codegen_emit(gen, "    push rax            ; Save index");
            snprintf(buffer, sizeof(buffer), "    mov rbx, [rbp-%d]   ; Load array pointer", var_info->stack_offset);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    pop rax             ; Restore index");
            codegen_emit(gen, "    mov rcx, 8          ; Element size");
            codegen_emit(gen, "    imul rax, rcx       ; index * 8");
            codegen_emit(gen, "    add rbx, rax        ; ptr + (index * 8)");
            codegen_emit(gen, "    mov rax, [rbx]      ; Load array[index]");
        } else {
            // Fixed-size array: stack-based addressing
            int base_offset = var_info->stack_offset;
            
            // Calculate element address: rbp - (base - index*8)
            codegen_emit(gen, "    mov rbx, 8          ; Element size");
            codegen_emit(gen, "    imul rax, rbx       ; index * 8");
            snprintf(buffer, sizeof(buffer), "    mov rbx, %d         ; Base offset", base_offset);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    sub rbx, rax        ; base - (index * 8)");
            codegen_emit(gen, "    neg rbx             ; -(base - index*8)");
            codegen_emit(gen, "    mov rax, [rbp + rbx] ; Load array[index]");
        }
    } else if (expr->type == EXPR_ADDRESS_OF) {
        // Phase 10: Address-of operator: &variable
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Address-of operator");
        
        if (expr->unary_operand->type == EXPR_VARIABLE) {
            // Get address of variable
            int offset = codegen_find_variable(gen, expr->unary_operand->var_name);
            snprintf(buffer, sizeof(buffer), "    lea rax, [rbp-%d]   ; &%s", 
                     offset, expr->unary_operand->var_name);
            codegen_emit(gen, buffer);
        } else if (expr->unary_operand->type == EXPR_ARRAY_INDEX) {
            // Get address of array element: &arr[index]
            VarInfo* var_info = codegen_get_variable(gen, expr->unary_operand->array_index.array_name);
            
            // Evaluate index expression
            codegen_generate_expression_value(gen, expr->unary_operand->array_index.index);
            
            if (var_info->is_dynamic_array) {
                // Dynamic array: load pointer, then calculate element address
                codegen_emit(gen, "    push rax            ; Save index");
                snprintf(buffer, sizeof(buffer), "    mov rbx, [rbp-%d]   ; Load array pointer", var_info->stack_offset);
                codegen_emit(gen, buffer);
                codegen_emit(gen, "    pop rax             ; Restore index");
                codegen_emit(gen, "    mov rcx, 8          ; Element size");
                codegen_emit(gen, "    imul rax, rcx       ; index * 8");
                codegen_emit(gen, "    add rax, rbx        ; ptr + (index * 8) = element address");
            } else {
                // Fixed-size array: calculate stack address
                int base_offset = var_info->stack_offset;
                codegen_emit(gen, "    mov rbx, 8          ; Element size");
                codegen_emit(gen, "    imul rax, rbx       ; index * 8");
                snprintf(buffer, sizeof(buffer), "    mov rbx, %d         ; Base offset", base_offset);
                codegen_emit(gen, buffer);
                codegen_emit(gen, "    sub rbx, rax        ; base - (index * 8)");
                codegen_emit(gen, "    neg rbx             ; -(base - index*8)");
                codegen_emit(gen, "    lea rax, [rbp + rbx] ; Calculate element address");
            }
        } else {
            fprintf(stderr, "Codegen error: Address-of operator can only be applied to variables or array elements\n");
            exit(1);
        }
    } else if (expr->type == EXPR_DEREFERENCE) {
        // Phase 10: Dereference operator: *pointer
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Dereference operator");
        
        // Evaluate pointer expression (should result in an address)
        codegen_generate_expression_value(gen, expr->unary_operand);
        
        // Load value at that address
        codegen_emit(gen, "    mov rax, [rax]      ; Dereference pointer");
    } else if (expr->type == EXPR_LOGICAL_AND) {
        // Logical AND with short-circuit evaluation
        int false_label = gen->label_counter++;
        int end_label = gen->label_counter++;
        
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Logical AND");
        
        // Evaluate left side
        codegen_generate_expression_value(gen, expr->logical_binary.left);
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jz .L%d          ; Short-circuit if false", false_label);
        codegen_emit(gen, buffer);
        
        // Evaluate right side (only if left was true)
        codegen_generate_expression_value(gen, expr->logical_binary.right);
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jz .L%d          ; Check right side", false_label);
        codegen_emit(gen, buffer);
        
        // Both true
        codegen_emit(gen, "    mov rax, 1          ; Result: true");
        snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
        codegen_emit(gen, buffer);
        
        // At least one false
        snprintf(buffer, sizeof(buffer), ".L%d:", false_label);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    mov rax, 0          ; Result: false");
        
        snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
        codegen_emit(gen, buffer);
    } else if (expr->type == EXPR_LOGICAL_OR) {
        // Logical OR with short-circuit evaluation
        int true_label = gen->label_counter++;
        int end_label = gen->label_counter++;
        
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Logical OR");
        
        // Evaluate left side
        codegen_generate_expression_value(gen, expr->logical_binary.left);
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jnz .L%d          ; Short-circuit if true", true_label);
        codegen_emit(gen, buffer);
        
        // Evaluate right side (only if left was false)
        codegen_generate_expression_value(gen, expr->logical_binary.right);
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jnz .L%d          ; Check right side", true_label);
        codegen_emit(gen, buffer);
        
        // Both false
        codegen_emit(gen, "    mov rax, 0          ; Result: false");
        snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
        codegen_emit(gen, buffer);
        
        // At least one true
        snprintf(buffer, sizeof(buffer), ".L%d:", true_label);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    mov rax, 1          ; Result: true");
        
        snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
        codegen_emit(gen, buffer);
    } else if (expr->type == EXPR_LOGICAL_NOT) {
        // Logical NOT
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Logical NOT");
        
        // Evaluate operand
        codegen_generate_expression_value(gen, expr->logical_not_operand);
        
        // Flip boolean: 0 -> 1, non-zero -> 0
        codegen_emit(gen, "    test rax, rax");
        codegen_emit(gen, "    setz al             ; Set AL to 1 if zero, 0 otherwise");
        codegen_emit(gen, "    movzx rax, al       ; Zero-extend AL to RAX");
    } else if (expr->type == EXPR_LIST_LITERAL) {
        // Phase 22: List literal: (a, b, c) - heterojen, mutable, heap-allocated
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; List literal (heterojen, mutable)");
        
        int count = expr->list_literal.count;
        
        if (count == 0) {
            // Empty list: just return NULL pointer
            codegen_emit(gen, "    xor rax, rax        ; Empty list = NULL");
        } else {
            // Allocate space for list header (count + capacity + data pointer) + elements
            // Layout: [count:8][capacity:8][element0:8][element1:8]...
            int header_size = 16;  // count + capacity
            int data_size = count * 8;
            snprintf(buffer, sizeof(buffer), "    mov rdi, %d         ; List size: header + %d elements",
                     header_size + data_size, count);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    call gc_alloc       ; Allocate list on heap");
            codegen_emit(gen, "    push rax            ; Save list pointer");
            
            // Store count
            snprintf(buffer, sizeof(buffer), "    mov qword [rax], %d ; list.count = %d", count, count);
            codegen_emit(gen, buffer);
            
            // Store capacity (same as count initially)
            snprintf(buffer, sizeof(buffer), "    mov qword [rax+8], %d ; list.capacity = %d", count, count);
            codegen_emit(gen, buffer);
            
            // Store elements starting at offset 16
            for (int i = 0; i < count; i++) {
                codegen_emit(gen, "    push rax            ; Save list pointer");
                codegen_generate_expression_value(gen, expr->list_literal.elements[i]);
                codegen_emit(gen, "    mov rbx, rax        ; Element value");
                codegen_emit(gen, "    pop rax             ; Restore list pointer");
                snprintf(buffer, sizeof(buffer), "    mov [rax+%d], rbx   ; list[%d] = value",
                         16 + i * 8, i);
                codegen_emit(gen, buffer);
            }
            
            codegen_emit(gen, "    pop rax             ; Return list pointer");
        }
    } else if (expr->type == EXPR_TUPLE_LITERAL) {
        // Phase 22: Tuple literal: <a, b, c> - heterojen, immutable, stack-allocated
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Tuple literal (heterojen, immutable, stack)");
        
        int count = expr->tuple_literal.count;
        
        if (count == 0) {
            // Empty tuple: return 0
            codegen_emit(gen, "    xor rax, rax        ; Empty tuple");
        } else {
            // Tuple is stack-allocated for performance
            // Allocate stack space for tuple
            int tuple_size = count * 8;
            snprintf(buffer, sizeof(buffer), "    sub rsp, %d         ; Allocate tuple (%d elements)",
                     tuple_size, count);
            codegen_emit(gen, buffer);
            
            // Store elements on stack (in reverse order for proper layout)
            for (int i = count - 1; i >= 0; i--) {
                codegen_generate_expression_value(gen, expr->tuple_literal.elements[i]);
                snprintf(buffer, sizeof(buffer), "    mov [rsp+%d], rax   ; tuple[%d] = value",
                         i * 8, i);
                codegen_emit(gen, buffer);
            }
            
            // Return pointer to tuple on stack
            codegen_emit(gen, "    mov rax, rsp        ; Return tuple pointer (stack)");
        }
    } else if (expr->type == EXPR_LAMBDA) {
        // Lambda functions: inline generation with JMP skip
        char buffer[256];
        char lambda_name[64];
        char skip_label_name[64];
        int skip_label = gen->label_counter++;
        
        snprintf(lambda_name, sizeof(lambda_name), "__lambda_%d", gen->label_counter++);
        snprintf(skip_label_name, sizeof(skip_label_name), "__lambda_skip_%d", skip_label);
        
        // Jump over lambda definition
        snprintf(buffer, sizeof(buffer), "    jmp %s     ; Skip lambda definition", skip_label_name);
        codegen_emit(gen, buffer);
        
        // Store current context
        int old_in_function = gen->in_function;
        VarInfo* old_variables = gen->variables;
        int old_stack_offset = gen->stack_offset;
        
        // Lambda function definition
        gen->in_function = 1;
        gen->variables = NULL;
        gen->stack_offset = 0;
        
        codegen_emit(gen, "");
        snprintf(buffer, sizeof(buffer), "; Lambda function: %s (captures %d variables)", 
                lambda_name, expr->lambda.captured_count);
        codegen_emit(gen, buffer);
        snprintf(buffer, sizeof(buffer), "%s:", lambda_name);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    push rbp");
        codegen_emit(gen, "    mov rbp, rsp");
        
        // If lambda has closure, first parameter (rdi) is closure environment pointer
        int param_offset = 0;
        const char* param_regs[] = {"rdi", "rsi", "rdx", "rcx"};
        
        if (expr->lambda.captured_count > 0) {
            // Store closure environment pointer
            codegen_emit(gen, "    ; Store closure environment pointer");
            gen->stack_offset += 8;
            codegen_emit(gen, "    push rdi    ; closure environment");
            int closure_offset = gen->stack_offset;
            
            // Add captured variables to symbol table
            for (int i = 0; i < expr->lambda.captured_count; i++) {
                VarInfo* var = malloc(sizeof(VarInfo));
                var->name = malloc(strlen(expr->lambda.captured_vars[i]) + 1);
                strcpy(var->name, expr->lambda.captured_vars[i]);
                var->stack_offset = -(1000 + i);  // Special marker: negative 1000+ index
                var->type = TYPE_NUMERIC;
                var->struct_name = NULL;
                var->is_dynamic_array = 0;
                var->is_pointer = 0;
    var->is_closure = 0;
                var->next = gen->variables;
                gen->variables = var;
                
                snprintf(buffer, sizeof(buffer), 
                        "    ; Captured variable: %s at environment[%d]", 
                        expr->lambda.captured_vars[i], i);
                codegen_emit(gen, buffer);
            }
            
            param_offset = 1;  // First actual parameter is in rsi, not rdi
        }
        
        // Allocate stack for parameters
        int total_stack = expr->lambda.param_count * 8;
        if (total_stack > 0) {
            snprintf(buffer, sizeof(buffer), "    sub rsp, %d", total_stack);
            codegen_emit(gen, buffer);
        }
        
        // Store parameters from registers
        for (int i = 0; i < expr->lambda.param_count && (i + param_offset) < 4; i++) {
            gen->stack_offset += 8;
            VarInfo* var = malloc(sizeof(VarInfo));
            var->name = malloc(strlen(expr->lambda.param_names[i]) + 1);
            strcpy(var->name, expr->lambda.param_names[i]);
            var->stack_offset = gen->stack_offset;
            var->type = TYPE_NUMERIC;
            var->struct_name = NULL;
            var->is_dynamic_array = 0;
            var->is_pointer = 0;
    var->is_closure = 0;
            var->next = gen->variables;
            gen->variables = var;
            
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], %s", 
                    gen->stack_offset, param_regs[i + param_offset]);
            codegen_emit(gen, buffer);
        }
        
        // Generate lambda body
        codegen_emit(gen, "    ; Lambda body");
        codegen_generate_expression_value(gen, expr->lambda.body);
        
        // Return
        codegen_emit(gen, "    leave");
        codegen_emit(gen, "    ret");
        
        // Skip label - closure creation code runs HERE in outer function scope
        snprintf(buffer, sizeof(buffer), "%s:", skip_label_name);
        codegen_emit(gen, buffer);
        
        // Create closure if needed (BEFORE restoring context, while captured vars accessible)
        if (expr->lambda.captured_count > 0) {
            codegen_emit(gen, "");
            codegen_emit(gen, "    ; Allocate closure environment");
            int env_size = expr->lambda.captured_count * 8;
            snprintf(buffer, sizeof(buffer), "    mov rdi, %d", env_size);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    call malloc");
            codegen_emit(gen, "    mov r12, rax    ; Save environment pointer");
            
            // Copy captured variable values to environment (use OLD context)
            for (int i = 0; i < expr->lambda.captured_count; i++) {
                VarInfo* var = old_variables;
                while (var) {
                    if (strcmp(var->name, expr->lambda.captured_vars[i]) == 0) {
                        snprintf(buffer, sizeof(buffer), 
                                "    ; Store %s in environment[%d]", 
                                expr->lambda.captured_vars[i], i);
                        codegen_emit(gen, buffer);
                        
                        snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]", var->stack_offset);
                        codegen_emit(gen, buffer);
                        snprintf(buffer, sizeof(buffer), "    mov [r12+%d], rax", i * 8);
                        codegen_emit(gen, buffer);
                        break;
                    }
                    var = var->next;
                }
            }
            
            // Create closure: allocate structure with magic + function pointer + environment
            codegen_emit(gen, "    ; Create closure structure [magic, func_ptr, env_ptr]");
            codegen_emit(gen, "    mov rdi, 24     ; 3 pointers (magic + func_ptr + env_ptr)");
            codegen_emit(gen, "    call malloc");
            codegen_emit(gen, "    mov qword [rax], 202182115   ; Magic number 0xC105UR3");
            snprintf(buffer, sizeof(buffer), "    lea r13, [rel %s]", lambda_name);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    mov [rax+8], r13     ; Store function pointer");
            codegen_emit(gen, "    mov [rax+16], r12    ; Store environment pointer");
            codegen_emit(gen, "    ; rax now holds closure pointer");
        } else {
            // No closures: just load lambda address into rax
            snprintf(buffer, sizeof(buffer), "    lea rax, [rel %s]", lambda_name);
            codegen_emit(gen, buffer);
        }
        
        // Restore context AFTER closure creation
        gen->in_function = old_in_function;
        VarInfo* temp_vars = gen->variables;
        gen->variables = old_variables;
        gen->stack_offset = old_stack_offset;
        
        // Free lambda variables
        while (temp_vars) {
            VarInfo* next = temp_vars->next;
            free(temp_vars->name);
            free(temp_vars);
            temp_vars = next;
        }
    } else if (expr->type == EXPR_AWAIT) {
        // Await expression: for now just evaluate the awaited expression
        // Full async runtime would suspend here and resume later
        codegen_emit(gen, "    ; Await expression (compiled as sync call for now)");
        codegen_generate_expression_value(gen, expr->await_expr.awaited_expr);
        // Result is already in rax
    }
}

