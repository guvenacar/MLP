// ============================================================================
// MELP Modular Codegen - codegen_stmt.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// İçerik: codegen_generate_statement
// Ana statement dispatch fonksiyonu - tüm statement tiplerini yönlendirir

void codegen_generate_statement(Codegen* gen, Statement* stmt) {
    if (stmt->type == STMT_STRUCT_DEF) {
        // Register struct type (no assembly code generated)
        VarType* field_types = malloc(sizeof(VarType) * stmt->struct_def.field_count);
        char** field_names = malloc(sizeof(char*) * stmt->struct_def.field_count);
        
        for (int i = 0; i < stmt->struct_def.field_count; i++) {
            field_types[i] = stmt->struct_def.fields[i].type;
            field_names[i] = stmt->struct_def.fields[i].name;
        }
        
        codegen_add_struct(gen, stmt->struct_def.struct_name, 
                          stmt->struct_def.field_count, field_types, field_names,
                          stmt->struct_def.implements, stmt->struct_def.implements_count);
        
        free(field_types);
        free(field_names);
    } else if (stmt->type == STMT_INTERFACE_DEF) {
        // Register interface (no assembly code generated)
        codegen_add_interface(gen, stmt->interface_def.interface_name,
                             stmt->interface_def.methods, stmt->interface_def.method_count);
        
        // Validate that all implementing structs have required methods
        // This is done after all definitions are parsed
    } else if (stmt->type == STMT_ENUM_DEF) {
        // Register enum values (no assembly code generated)
        for (int i = 0; i < stmt->enum_def.member_count; i++) {
            codegen_add_enum_value(gen, stmt->enum_def.enum_name,
                                  stmt->enum_def.members[i].name,
                                  stmt->enum_def.members[i].value);
        }
    } else if (stmt->type == STMT_TYPE_ALIAS) {
        // Register type alias (no assembly code generated)
        codegen_add_type_alias(gen, stmt->type_alias.alias_name,
                              stmt->type_alias.base_type,
                              stmt->type_alias.struct_name);
    } else if (stmt->type == STMT_FUNC_DEF) {
        codegen_generate_func_def(gen, stmt);
    } else if (stmt->type == STMT_RETURN) {
        codegen_generate_return(gen, stmt);
    } else if (stmt->type == STMT_DECLARATION) {
        codegen_generate_declaration(gen, stmt->declaration);
    } else if (stmt->type == STMT_MULTI_DECLARATION) {
        // Phase 21: Multiple typed declarations (numeric, string, boolean a, b, c)
        for (int i = 0; i < stmt->multi_decl.count; i++) {
            codegen_generate_declaration(gen, stmt->multi_decl.declarations[i]);
        }
    } else if (stmt->type == STMT_ASSIGNMENT) {
        codegen_generate_assignment(gen, stmt->assignment.var_name, stmt->assignment.field_name, stmt->assignment.array_index, stmt->assignment.dereference_target, stmt->assignment.value);
    } else if (stmt->type == STMT_MULTI_ASSIGNMENT) {
        // Multi-assignment: a, b = func()
        if (!stmt->multi_assignment.func_call) {
            fprintf(stderr, "Codegen error: NULL function call in multi-assignment\n");
            exit(1);
        }
        
        // Call function (result in rax for first value, rdx for second)
        codegen_generate_expression_value(gen, stmt->multi_assignment.func_call);
        
        // Assign values to variables
        // First variable gets rax
        if (stmt->multi_assignment.var_count >= 1) {
            int offset = codegen_find_variable(gen, stmt->multi_assignment.var_names[0]);
            if (offset == -1) {
                fprintf(stderr, "Codegen error: Variable '%s' not found\n", stmt->multi_assignment.var_names[0]);
                exit(1);
            }
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; %s = first return value",
                     offset, stmt->multi_assignment.var_names[0]);
            codegen_emit(gen, buffer);
        }
        
        // Second variable gets rdx
        if (stmt->multi_assignment.var_count >= 2) {
            int offset = codegen_find_variable(gen, stmt->multi_assignment.var_names[1]);
            if (offset == -1) {
                fprintf(stderr, "Codegen error: Variable '%s' not found\n", stmt->multi_assignment.var_names[1]);
                exit(1);
            }
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rdx   ; %s = second return value",
                     offset, stmt->multi_assignment.var_names[1]);
            codegen_emit(gen, buffer);
        }
        
        // Additional variables (if supported in future)
        if (stmt->multi_assignment.var_count > 2) {
            fprintf(stderr, "Codegen warning: More than 2 values in multi-assignment not fully supported\n");
        }
    } else if (stmt->type == STMT_DESTRUCTURE) {
        // Destructuring: numeric a, b, c = arr
        char buffer[256];
        
        // Get source variable (must be an array for now)
        if (stmt->destructure.source->type != EXPR_VARIABLE) {
            fprintf(stderr, "Codegen error: Destructuring source must be a variable\n");
            exit(1);
        }
        
        VarInfo* src_var = codegen_get_variable(gen, stmt->destructure.source->var_name);
        if (!src_var) {
            fprintf(stderr, "Codegen error: Destructuring source '%s' not found\n",
                    stmt->destructure.source->var_name);
            exit(1);
        }
        
        // For fixed-size arrays on stack, we need to calculate the base address
        // Array is stored at [rbp-offset] to [rbp-offset+size]
        // We want the address of the last element (highest address)
        // which is [rbp-offset+size-8] for numeric arrays
        
        // Calculate array base address (address of first element, which has highest offset)
        snprintf(buffer, sizeof(buffer), "    lea rax, [rbp-%d]   ; Load address of %s",
                 src_var->stack_offset, stmt->destructure.source->var_name);
        codegen_emit(gen, buffer);
        
        // Allocate space for each variable and load from array
        for (int i = 0; i < stmt->destructure.var_count; i++) {
            gen->stack_offset += 8;
            codegen_add_variable(gen, stmt->destructure.var_names[i],
                               gen->stack_offset, stmt->destructure.var_type, NULL);
            
            snprintf(buffer, sizeof(buffer), "    sub rsp, 8");
            codegen_emit(gen, buffer);
            
            // Load array[i] into variable
            // Array base address is in rax
            // Element i is at [rax + i*8]
            snprintf(buffer, sizeof(buffer), "    push rax   ; Save array pointer");
            codegen_emit(gen, buffer);
            
            snprintf(buffer, sizeof(buffer), "    mov rbx, [rax+%d]   ; Load array[%d]", i * 8, i);
            codegen_emit(gen, buffer);
            
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rbx   ; Store to %s",
                     gen->stack_offset, stmt->destructure.var_names[i]);
            codegen_emit(gen, buffer);
            
            snprintf(buffer, sizeof(buffer), "    pop rax   ; Restore array pointer");
            codegen_emit(gen, buffer);
        }
    } else if (stmt->type == STMT_PRINT) {
        codegen_generate_print(gen, stmt->print_stmt.expr);
    } else if (stmt->type == STMT_IF) {
        codegen_generate_if(gen, stmt);
    } else if (stmt->type == STMT_FOR) {
        codegen_generate_for(gen, stmt);
    } else if (stmt->type == STMT_FOR_IN) {
        codegen_generate_for_in(gen, stmt);
    } else if (stmt->type == STMT_YIELD) {
        // Phase 14: Yield statement for generators
        // For simple generator pattern: append value to internal result array
        // The generator function should have initialized a result array
        char buffer[256];
        codegen_emit(gen, "    ; Yield statement");
        
        // Evaluate the yield value
        codegen_generate_expression_value(gen, stmt->yield_stmt.value);
        codegen_emit(gen, "    push rax           ; Save yield value");
        
        // Find generator internal variables
        int arr_offset = codegen_find_variable(gen, "__gen_array__");
        int idx_offset = codegen_find_variable(gen, "__gen_index__");
        
        if (arr_offset == -1 || idx_offset == -1) {
            fprintf(stderr, "Codegen error: yield outside generator function\n");
            exit(1);
        }
        
        // Store value in generator's result array
        snprintf(buffer, sizeof(buffer), "    mov rdi, [rbp-%d]   ; Load result array pointer", arr_offset);
        codegen_emit(gen, buffer);
        snprintf(buffer, sizeof(buffer), "    mov rsi, [rbp-%d]   ; Load current index", idx_offset);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    pop rax             ; Restore yield value");
        codegen_emit(gen, "    mov [rdi + rsi*8], rax   ; Store yielded value");
        snprintf(buffer, sizeof(buffer), "    inc qword [rbp-%d]   ; Increment index", idx_offset);
        codegen_emit(gen, buffer);
    } else if (stmt->type == STMT_WHILE) {
        codegen_generate_while(gen, stmt);
    } else if (stmt->type == STMT_DO_WHILE) {
        codegen_generate_do_while(gen, stmt);
    } else if (stmt->type == STMT_SWITCH) {
        codegen_generate_switch(gen, stmt);
    } else if (stmt->type == STMT_TRY_CATCH) {
        // Real exception handling with setjmp/longjmp
        char buffer[256];
        int try_label = gen->label_counter++;
        int end_label = gen->label_counter++;
        int finally_label = gen->label_counter++;
        
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Try-catch-finally block (setjmp/longjmp)");
        
        // Push exception handler
        codegen_emit(gen, "    ; Push exception handler");
        codegen_emit(gen, "    call mlp_exception_push");
        codegen_emit(gen, "    mov r15, rax        ; Save handler pointer");
        
        // setjmp(handler->jump_buffer)
        codegen_emit(gen, "    ; setjmp - returns 0 on first call, 1 on longjmp");
        codegen_emit(gen, "    mov rdi, r15");
        codegen_emit(gen, "    call setjmp");
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jnz .L%d_catch    ; Jump to catch if exception thrown", try_label);
        codegen_emit(gen, buffer);
        
        // Try body
        snprintf(buffer, sizeof(buffer), ".L%d_try:", try_label);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    ; Try body");
        for (int i = 0; i < stmt->try_catch.try_count; i++) {
            codegen_generate_statement(gen, stmt->try_catch.try_body[i]);
        }
        
        // No exception - pop handler and jump to finally
        codegen_emit(gen, "    ; Try succeeded - pop handler");
        codegen_emit(gen, "    call mlp_exception_pop");
        if (stmt->try_catch.finally_count > 0) {
            snprintf(buffer, sizeof(buffer), "    jmp .L%d_finally", finally_label);
        } else {
            snprintf(buffer, sizeof(buffer), "    jmp .L%d_end", end_label);
        }
        codegen_emit(gen, buffer);
        
        // Catch blocks
        snprintf(buffer, sizeof(buffer), ".L%d_catch:", try_label);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    ; Catch block(s)");
        
        int rethrow_label = gen->label_counter++;
        
        for (int c = 0; c < stmt->try_catch.catch_count; c++) {
            int next_catch_label = (c < stmt->try_catch.catch_count - 1) ? 
                                    gen->label_counter++ : rethrow_label;
            
            codegen_emit(gen, "");
            snprintf(buffer, sizeof(buffer), "    ; Catch block %d", c);
            codegen_emit(gen, buffer);
            
            // Type check if specified
            if (stmt->try_catch.catch_blocks[c].exception_type) {
                int str_id = codegen_add_string(gen, stmt->try_catch.catch_blocks[c].exception_type);
                
                codegen_emit(gen, "    ; Check exception type");
                codegen_emit(gen, "    call mlp_exception_type");
                snprintf(buffer, sizeof(buffer), "    mov rdi, str_%d    ; Expected type", str_id);
                codegen_emit(gen, buffer);
                codegen_emit(gen, "    mov rsi, rax       ; Actual type");
                codegen_emit(gen, "    call strcmp");
                codegen_emit(gen, "    test rax, rax");
                snprintf(buffer, sizeof(buffer), "    jnz .L%d          ; Type mismatch, try next catch", next_catch_label);
                codegen_emit(gen, buffer);
            }
            
            // Store exception variable if specified
            if (stmt->try_catch.catch_blocks[c].exception_var) {
                codegen_emit(gen, "    ; Store exception in variable");
                gen->stack_offset += 8;
                
                VarInfo* var = malloc(sizeof(VarInfo));
                var->name = malloc(strlen(stmt->try_catch.catch_blocks[c].exception_var) + 1);
                strcpy(var->name, stmt->try_catch.catch_blocks[c].exception_var);
                var->stack_offset = gen->stack_offset;
                var->type = TYPE_STRING;  // Exception is string pointer
                var->struct_name = NULL;
                var->is_dynamic_array = 0;
                var->is_pointer = 0;
    var->is_closure = 0;
                var->next = gen->variables;
                gen->variables = var;
                
                codegen_emit(gen, "    call mlp_exception_message");
                snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax", gen->stack_offset);
                codegen_emit(gen, buffer);
            }
            
            // Generate catch body
            for (int i = 0; i < stmt->try_catch.catch_blocks[c].body_count; i++) {
                codegen_generate_statement(gen, stmt->try_catch.catch_blocks[c].body[i]);
            }
            
            // Pop handler after successful catch
            codegen_emit(gen, "    ; Caught - pop handler");
            codegen_emit(gen, "    call mlp_exception_pop");
            
            // Jump to finally or end
            if (stmt->try_catch.finally_count > 0) {
                snprintf(buffer, sizeof(buffer), "    jmp .L%d_finally", finally_label);
            } else {
                snprintf(buffer, sizeof(buffer), "    jmp .L%d_end", end_label);
            }
            codegen_emit(gen, buffer);
            
            // Next catch label
            if (c < stmt->try_catch.catch_count - 1) {
                snprintf(buffer, sizeof(buffer), ".L%d:", next_catch_label);
                codegen_emit(gen, buffer);
            }
        }
        
        // No catch matched - re-throw or uncaught
        snprintf(buffer, sizeof(buffer), ".L%d:  ; Re-throw/Uncaught", rethrow_label);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    ; No catch matched - check if parent handler exists (before popping)");
        
        // Check for parent handler WITHOUT popping yet
        codegen_emit(gen, "    call mlp_exception_has_parent_handler");
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jnz .L%d_do_rethrow", rethrow_label);
        codegen_emit(gen, buffer);
        
        // No parent handler - uncaught exception, terminate (DON'T pop, data still valid)
        snprintf(buffer, sizeof(buffer), ".L%d_uncaught:", rethrow_label);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    ; Uncaught exception - print and exit");
        
        // Print "Uncaught exception: " prefix
        int uncaught_str = codegen_add_string(gen, "Uncaught exception: ");
        snprintf(buffer, sizeof(buffer), "    mov rdi, str_%d", uncaught_str);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    call print_string");
        
        // Print exception type
        codegen_emit(gen, "    call mlp_exception_type");
        codegen_emit(gen, "    mov rdi, rax");
        codegen_emit(gen, "    call print_string");
        
        // Print " - "
        int sep_str = codegen_add_string(gen, " - ");
        snprintf(buffer, sizeof(buffer), "    mov rdi, str_%d", sep_str);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    call print_string");
        
        // Print exception message
        codegen_emit(gen, "    call mlp_exception_message");
        codegen_emit(gen, "    mov rdi, rax");
        codegen_emit(gen, "    call print_string");
        
        codegen_emit(gen, "    mov rax, 60       ; sys_exit");
        codegen_emit(gen, "    mov rdi, 1        ; error code");
        codegen_emit(gen, "    syscall");
        
        // Has parent handler - pop and re-throw
        snprintf(buffer, sizeof(buffer), ".L%d_do_rethrow:", rethrow_label);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    ; Re-throw to parent handler");
        codegen_emit(gen, "    call mlp_exception_type");
        codegen_emit(gen, "    push rax");
        codegen_emit(gen, "    call mlp_exception_message");
        codegen_emit(gen, "    push rax");
        codegen_emit(gen, "    call mlp_exception_code");
        codegen_emit(gen, "    mov rdx, rax");
        codegen_emit(gen, "    pop rsi");
        codegen_emit(gen, "    pop rdi");
        codegen_emit(gen, "    call mlp_exception_pop    ; Pop AFTER saving data");
        codegen_emit(gen, "    call mlp_throw");
        
        // Finally block
        if (stmt->try_catch.finally_count > 0) {
            snprintf(buffer, sizeof(buffer), ".L%d_finally:", finally_label);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    ; Finally block");
            for (int i = 0; i < stmt->try_catch.finally_count; i++) {
                codegen_generate_statement(gen, stmt->try_catch.finally_body[i]);
            }
        }
        
        // End label
        snprintf(buffer, sizeof(buffer), ".L%d_end:", end_label);
        codegen_emit(gen, buffer);
    } else if (stmt->type == STMT_THROW) {
        // Real throw with mlp_throw runtime
        char buffer[256];
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Throw exception");
        
        // Get exception type (or default "Error")
        if (stmt->throw_stmt.error_type) {
            int str_id = codegen_add_string(gen, stmt->throw_stmt.error_type);
            snprintf(buffer, sizeof(buffer), "    mov rdi, str_%d    ; Exception type", str_id);
            codegen_emit(gen, buffer);
        } else {
            int str_id = codegen_add_string(gen, "Error");
            snprintf(buffer, sizeof(buffer), "    mov rdi, str_%d    ; Default type: Error", str_id);
            codegen_emit(gen, buffer);
        }
        
        // Evaluate message expression
        if (stmt->throw_stmt.error_message) {
            codegen_emit(gen, "    push rdi           ; Save type");
            codegen_generate_expression_value(gen, stmt->throw_stmt.error_message);
            codegen_emit(gen, "    mov rsi, rax       ; Message in rsi");
            codegen_emit(gen, "    pop rdi            ; Restore type");
        } else {
            int str_id = codegen_add_string(gen, "Unknown error");
            snprintf(buffer, sizeof(buffer), "    mov rsi, str_%d    ; Default message", str_id);
            codegen_emit(gen, buffer);
        }
        
        // Exception code (default 1)
        codegen_emit(gen, "    mov rdx, 1         ; Error code");
        
        // Call mlp_throw (does not return - longjmp to handler)
        codegen_emit(gen, "    call mlp_throw");
    } else if (stmt->type == STMT_EXIT) {
        if (!gen->in_loop) {
            fprintf(stderr, "Codegen error: 'exit' outside loop\n");
            exit(1);
        }
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "    jmp .L%d           ; exit", gen->loop_end_label);
        codegen_emit(gen, buffer);
    } else if (stmt->type == STMT_CONTINUE) {
        if (!gen->in_loop) {
            fprintf(stderr, "Codegen error: 'continue' outside loop\n");
            exit(1);
        }
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "    jmp .L%d           ; continue", gen->loop_continue_label);
        codegen_emit(gen, buffer);
    } else if (stmt->type == STMT_EXPR_STMT) {
        // Expression statement (e.g., function call for side effects)
        codegen_generate_expression_value(gen, stmt->print_stmt.expr);
        // Result in rax, but we don't use it
    } else if (stmt->type == STMT_MODULE_DEF) {
        // Module definition: Generate code for module body with namespace prefix
        
        // Check if module already processed (deduplication)
        if (codegen_module_seen(gen, stmt->module_def.module_name)) {
            // Skip duplicate module definition
            char comment[256];
            snprintf(comment, sizeof(comment), "; Skipping duplicate module: %s", 
                     stmt->module_def.module_name);
            codegen_emit(gen, comment);
            return;
        }
        
        // Mark module as seen
        codegen_mark_module_seen(gen, stmt->module_def.module_name);
        
        codegen_emit(gen, "");
        char comment[256];
        snprintf(comment, sizeof(comment), "; Module: %s", stmt->module_def.module_name);
        codegen_emit(gen, comment);
        
        // Set current module context
        gen->current_module = stmt->module_def.module_name;
        
        // Generate code for all statements in module body
        for (int i = 0; i < stmt->module_def.body_count; i++) {
            codegen_generate_statement(gen, stmt->module_def.body[i]);
        }
        
        // Clear module context
        gen->current_module = NULL;
        
        codegen_emit(gen, "; End of module");
        codegen_emit(gen, "");
    } else if (stmt->type == STMT_OPERATOR_DEF) {
        // Phase 16: Operator overloading - generate operator function
        char buffer[256];
        char func_name[256];
        
        // Generate unique function name for operator
        // Format: __op_TYPE_OPERATOR (e.g., __op_Vector_add for Vector +)
        const char* op_name;
        if (strcmp(stmt->operator_def.op, "+") == 0) op_name = "add";
        else if (strcmp(stmt->operator_def.op, "-") == 0) op_name = "sub";
        else if (strcmp(stmt->operator_def.op, "*") == 0) op_name = "mul";
        else if (strcmp(stmt->operator_def.op, "/") == 0) op_name = "div";
        else if (strcmp(stmt->operator_def.op, "%") == 0) op_name = "mod";
        else if (strcmp(stmt->operator_def.op, "==") == 0) op_name = "eq";
        else if (strcmp(stmt->operator_def.op, "!=") == 0) op_name = "neq";
        else if (strcmp(stmt->operator_def.op, "<") == 0) op_name = "lt";
        else if (strcmp(stmt->operator_def.op, ">") == 0) op_name = "gt";
        else if (strcmp(stmt->operator_def.op, "<=") == 0) op_name = "lte";
        else if (strcmp(stmt->operator_def.op, ">=") == 0) op_name = "gte";
        else op_name = "op";
        
        snprintf(func_name, sizeof(func_name), "__op_%s_%s", 
                 stmt->operator_def.type_name, op_name);
        
        // Register operator overload
        codegen_add_operator_overload(gen, stmt->operator_def.op, 
                                     stmt->operator_def.type_name, func_name);
        
        codegen_emit(gen, "");
        snprintf(buffer, sizeof(buffer), "; Operator overload: %s %s", 
                 stmt->operator_def.type_name, stmt->operator_def.op);
        codegen_emit(gen, buffer);
        
        // Generate function label
        snprintf(buffer, sizeof(buffer), "%s:", func_name);
        codegen_emit(gen, buffer);
        
        // Function prologue
        codegen_emit(gen, "    push rbp");
        codegen_emit(gen, "    mov rbp, rsp");
        
        // Get struct size for parameters
        StructInfo* struct_info = codegen_find_struct(gen, stmt->operator_def.type_name);
        int struct_size = struct_info ? struct_info->total_size : 8;
        
        // Calculate stack space needed - parameters + local vars
        int stack_space = struct_size * 2 + 64; // Two struct params + extra
        snprintf(buffer, sizeof(buffer), "    sub rsp, %d", stack_space);
        codegen_emit(gen, buffer);
        
        // Save original stack offset
        int saved_stack_offset = gen->stack_offset;
        gen->stack_offset = 0;
        gen->in_operator_overload = 1;
        gen->in_function = 1;
        
        // First parameter (passed by pointer in rdi)
        // Copy struct from rdi to local stack
        gen->stack_offset += struct_size;
        codegen_add_variable(gen, stmt->operator_def.param1_name, 
                            gen->stack_offset, TYPE_NUMERIC, 
                            stmt->operator_def.type_name);
        
        // Copy struct fields from pointer to stack
        codegen_emit(gen, "    ; Copy first parameter struct to stack");
        for (int i = 0; i < (struct_size / 8); i++) {
            snprintf(buffer, sizeof(buffer), "    mov rax, [rdi+%d]", i * 8);
            codegen_emit(gen, buffer);
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax", gen->stack_offset - i * 8);
            codegen_emit(gen, buffer);
        }
        
        // Second parameter (for binary operators, passed by pointer in rsi)
        if (stmt->operator_def.param2_name) {
            gen->stack_offset += struct_size;
            codegen_add_variable(gen, stmt->operator_def.param2_name, 
                                gen->stack_offset, TYPE_NUMERIC, 
                                stmt->operator_def.type_name);
            
            codegen_emit(gen, "    ; Copy second parameter struct to stack");
            for (int i = 0; i < (struct_size / 8); i++) {
                snprintf(buffer, sizeof(buffer), "    mov rax, [rsi+%d]", i * 8);
                codegen_emit(gen, buffer);
                snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax", gen->stack_offset - i * 8);
                codegen_emit(gen, buffer);
            }
        }
        
        // Generate body statements
        for (int i = 0; i < stmt->operator_def.body_count; i++) {
            codegen_generate_statement(gen, stmt->operator_def.body[i]);
        }
        
        // Cleanup and return (return value should be set by return statement in body)
        codegen_emit(gen, "    mov rsp, rbp");
        codegen_emit(gen, "    pop rbp");
        codegen_emit(gen, "    ret");
        codegen_emit(gen, "");
        
        // Restore state
        gen->stack_offset = saved_stack_offset;
        gen->in_operator_overload = 0;
        gen->in_function = 0;
        
    } else if (stmt->type == STMT_MATCH) {
        // Phase 17: Pattern matching - similar to switch-case but with patterns
        char buffer[256];
        int end_label = gen->label_counter++;
        
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Pattern match statement");
        
        // Evaluate match value once and store in rax
        codegen_generate_expression_value(gen, stmt->match_stmt.value);
        codegen_emit(gen, "    push rax           ; Save match value");
        
        // Generate code for each case
        for (int i = 0; i < stmt->match_stmt.case_count; i++) {
            int next_case_label = gen->label_counter++;
            
            if (stmt->match_stmt.cases[i].is_wildcard) {
                // Wildcard pattern (_) - always matches, like default
                codegen_emit(gen, "    ; case _ (wildcard/default)");
            } else if (stmt->match_stmt.cases[i].is_range) {
                // Range pattern (a to b)
                codegen_emit(gen, "    ; case range pattern");
                codegen_emit(gen, "    mov rax, [rsp]      ; Load match value");
                
                // Check lower bound
                codegen_generate_expression_value(gen, stmt->match_stmt.cases[i].range_start);
                codegen_emit(gen, "    mov rbx, rax        ; Lower bound in rbx");
                codegen_emit(gen, "    mov rax, [rsp]      ; Match value");
                codegen_emit(gen, "    cmp rax, rbx");
                snprintf(buffer, sizeof(buffer), "    jl .L%d            ; Skip if value < lower", next_case_label);
                codegen_emit(gen, buffer);
                
                // Check upper bound
                codegen_generate_expression_value(gen, stmt->match_stmt.cases[i].range_end);
                codegen_emit(gen, "    mov rbx, rax        ; Upper bound in rbx");
                codegen_emit(gen, "    mov rax, [rsp]      ; Match value");
                codegen_emit(gen, "    cmp rax, rbx");
                snprintf(buffer, sizeof(buffer), "    jg .L%d            ; Skip if value > upper", next_case_label);
                codegen_emit(gen, buffer);
            } else {
                // Regular pattern(s) - check each pattern value
                codegen_emit(gen, "    ; case pattern(s)");
                
                if (stmt->match_stmt.cases[i].pattern_count == 1) {
                    // Single pattern
                    codegen_generate_expression_value(gen, stmt->match_stmt.cases[i].patterns[0]);
                    codegen_emit(gen, "    mov rbx, rax        ; Pattern value");
                    codegen_emit(gen, "    mov rax, [rsp]      ; Match value");
                    codegen_emit(gen, "    cmp rax, rbx");
                    snprintf(buffer, sizeof(buffer), "    jne .L%d           ; Skip if not equal", next_case_label);
                    codegen_emit(gen, buffer);
                } else {
                    // Multiple patterns (OR logic) - match if any pattern matches
                    int match_label = gen->label_counter++;
                    
                    for (int p = 0; p < stmt->match_stmt.cases[i].pattern_count; p++) {
                        codegen_generate_expression_value(gen, stmt->match_stmt.cases[i].patterns[p]);
                        codegen_emit(gen, "    mov rbx, rax        ; Pattern value");
                        codegen_emit(gen, "    mov rax, [rsp]      ; Match value");
                        codegen_emit(gen, "    cmp rax, rbx");
                        snprintf(buffer, sizeof(buffer), "    je .L%d            ; Match found", match_label);
                        codegen_emit(gen, buffer);
                    }
                    // No pattern matched, skip to next case
                    snprintf(buffer, sizeof(buffer), "    jmp .L%d", next_case_label);
                    codegen_emit(gen, buffer);
                    
                    // Match label - at least one pattern matched
                    snprintf(buffer, sizeof(buffer), ".L%d:", match_label);
                    codegen_emit(gen, buffer);
                }
            }
            
            // Generate case body
            for (int b = 0; b < stmt->match_stmt.cases[i].body_count; b++) {
                codegen_generate_statement(gen, stmt->match_stmt.cases[i].body[b]);
            }
            
            // Jump to end (no fall-through)
            snprintf(buffer, sizeof(buffer), "    jmp .L%d           ; End of match", end_label);
            codegen_emit(gen, buffer);
            
            // Next case label
            snprintf(buffer, sizeof(buffer), ".L%d:", next_case_label);
            codegen_emit(gen, buffer);
        }
        
        // End of match
        snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    add rsp, 8          ; Pop match value");
        codegen_emit(gen, "");
        
    } else if (stmt->type == STMT_STATE_DECL) {
        // Phase 18: State Management - Global state declaration
        char buffer[512];
        
        // Add to global variables list (similar to regular declaration but global scope)
        // State variables are stored in .bss section (uninitialized) or .data section (initialized)
        
        // Find or add variable in global scope
        int found = 0;
        for (int i = 0; i < gen->global_var_count; i++) {
            if (strcmp(gen->global_vars[i].name, stmt->state_decl.name) == 0) {
                found = 1;
                break;
            }
        }
        
        if (!found && gen->global_var_count < MAX_STATE_VARS) {
            // Add new global state variable
            char* name_copy = malloc(strlen(stmt->state_decl.name) + 1);
            strcpy(name_copy, stmt->state_decl.name);
            gen->global_vars[gen->global_var_count].name = name_copy;
            gen->global_vars[gen->global_var_count].type = stmt->state_decl.type;
            gen->global_vars[gen->global_var_count].is_global = 1;
            gen->global_vars[gen->global_var_count].is_state = 1;  // Mark as state variable
            gen->global_vars[gen->global_var_count].init_expr = stmt->state_decl.initial_value;  // Store init expr for later
            gen->global_var_count++;
        }
        
    } else if (stmt->type == STMT_DEBUG_LABEL) {
        // Debug label: Generate assembly label
        char label[256];
        snprintf(label, sizeof(label), ".debug_%s:", stmt->debug_label.label_name);
        codegen_emit(gen, "");
        codegen_emit(gen, label);
    } else if (stmt->type == STMT_DEBUG_GOTO) {
        // Debug goto: Generate jump to debug label
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "    jmp .debug_%s           ; debug goto", 
                 stmt->debug_goto.target_label);
        codegen_emit(gen, buffer);
    } else if (stmt->type == STMT_DEBUG_IF) {
        // Debug if: Conditional debug block
        int end_label = gen->label_counter++;
        
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Debug if block");
        
        // Generate condition check (similar to normal if)
        if (stmt->debug_if.condition->type == EXPR_COMPARISON) {
            codegen_generate_comparison(gen, stmt->debug_if.condition, end_label);
        } else {
            // For non-comparison expressions, check if result is non-zero
            codegen_generate_expression_value(gen, stmt->debug_if.condition);
            codegen_emit(gen, "    cmp rax, 0");
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "    je .L%d", end_label);
            codegen_emit(gen, buffer);
        }
        
        // Generate debug if body
        for (int i = 0; i < stmt->debug_if.body_count; i++) {
            codegen_generate_statement(gen, stmt->debug_if.body[i]);
        }
        
        // End label
        char buffer[256];
        snprintf(buffer, sizeof(buffer), ".L%d:  ; End debug if", end_label);
        codegen_emit(gen, buffer);
    } else if (stmt->type == STMT_DEBUG_PAUSE) {
        // Debug pause: Generate breakpoint (int3 instruction)
        codegen_emit(gen, "    int3                ; debug pause (breakpoint)");
    } else if (stmt->type == STMT_IMPORT) {
        // Import statement: Currently a no-op in codegen
        // In future, this could generate extern declarations
        // For now, we assume all modules are compiled together and linked
        char comment[256];
        if (stmt->import_stmt.alias) {
            snprintf(comment, sizeof(comment), "; Import: %s as %s", 
                     stmt->import_stmt.module_name, stmt->import_stmt.alias);
        } else {
            snprintf(comment, sizeof(comment), "; Import: %s", stmt->import_stmt.module_name);
        }
        codegen_emit(gen, comment);
    }
}

