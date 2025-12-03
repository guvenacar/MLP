// ============================================================================
// MELP Modular Codegen - codegen_funcdef.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// İçerik: codegen_generate_func_def
// Fonksiyon tanımlarını işler (generic, method, async dahil)

void codegen_generate_func_def(Codegen* gen, Statement* stmt) {
    char buffer[256];
    
    // Phase 13: If this is a generic function, save it for later instantiation
    if (stmt->func_def.type_param_count > 0) {
        codegen_add_generic_func(gen, stmt->func_def.func_name, stmt);
        // Don't generate code now - will be generated when called with concrete types
        return;
    }
    
    // Generate function name with module prefix if inside module
    char func_name[256];
    if (gen->current_module) {
        snprintf(func_name, sizeof(func_name), "%s_%s", gen->current_module, stmt->func_def.func_name);
    } else {
        snprintf(func_name, sizeof(func_name), "func_%s", stmt->func_def.func_name);
    }
    
    // Register function
    codegen_add_function(gen, stmt->func_def.func_name, 
                        stmt->func_def.param_count, 
                        stmt->func_def.param_types);
    
    codegen_emit(gen, "");
    
    // Export visibility: Add global directive if exported
    if (stmt->func_def.is_exported) {
        snprintf(buffer, sizeof(buffer), "global %s", func_name);
        codegen_emit(gen, buffer);
    }
    
    // Note: Async functions are compiled as regular functions for now
    // Full async runtime with state machines would require complex transformation
    if (stmt->func_def.is_async) {
        snprintf(buffer, sizeof(buffer), "; Async function: %s (compiled as sync for now)", stmt->func_def.func_name);
        codegen_emit(gen, buffer);
    }
    
    snprintf(buffer, sizeof(buffer), "%s:", func_name);
    codegen_emit(gen, buffer);
    codegen_emit(gen, "    push rbp");
    codegen_emit(gen, "    mov rbp, rsp");
    
    // Save previous state
    int prev_stack_offset = gen->stack_offset;
    VarInfo* prev_variables = gen->variables;
    int prev_in_function = gen->in_function;
    int prev_in_operator_overload = gen->in_operator_overload;
    int prev_in_generator = gen->in_generator;
    
    gen->stack_offset = 0;
    gen->variables = NULL;
    gen->in_function = 1;
    gen->in_generator = stmt->func_def.is_generator;
    
    // Check if this is an operator overload function (name contains "_operator_")
    if (strstr(stmt->func_def.func_name, "_operator_") != NULL) {
        gen->in_operator_overload = 1;
    }
    
    // Parameters are passed on stack (above rbp)
    // rbp+16 = first param, rbp+24 = second param, etc.
    for (int i = 0; i < stmt->func_def.param_count; i++) {
        int param_offset = 16 + (i * 8);
        
        // Load parameter from stack above rbp
        snprintf(buffer, sizeof(buffer), "    mov rax, [rbp+%d]   ; Load param %s",
                 param_offset, stmt->func_def.param_names[i]);
        codegen_emit(gen, buffer);
        
        // Allocate local space for parameter
        gen->stack_offset += 8;
        // Pass struct_name if it's a struct type parameter
        const char* param_struct_name = stmt->func_def.param_struct_names[i];
        codegen_add_variable(gen, stmt->func_def.param_names[i], gen->stack_offset, 
                           stmt->func_def.param_types[i], param_struct_name);
        
        snprintf(buffer, sizeof(buffer), "    sub rsp, 8");
        codegen_emit(gen, buffer);
        snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store param %s locally",
                 gen->stack_offset, stmt->func_def.param_names[i]);
        codegen_emit(gen, buffer);
    }
    
    // Phase 14: Generator function setup
    // If this is a generator (yields), allocate result array and index counter
    int gen_array_offset = 0;
    int gen_index_offset = 0;
    if (stmt->func_def.is_generator) {
        codegen_emit(gen, "    ; Generator function setup");
        
        // Allocate space for result array pointer
        gen->stack_offset += 8;
        gen_array_offset = gen->stack_offset;
        codegen_emit(gen, "    sub rsp, 8");
        
        // Allocate space for current index
        gen->stack_offset += 8;
        gen_index_offset = gen->stack_offset;
        codegen_emit(gen, "    sub rsp, 8");
        
        // Initialize with estimated size (will grow if needed) - allocate 16 elements
        codegen_emit(gen, "    mov rdi, 16");
        codegen_emit(gen, "    call mlp_array_alloc");
        snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax      ; Store result array pointer", gen_array_offset);
        codegen_emit(gen, buffer);
        snprintf(buffer, sizeof(buffer), "    mov qword [rbp-%d], 0 ; Initialize index to 0", gen_index_offset);
        codegen_emit(gen, buffer);
        
        // Store offsets for yield statements (use special named variables)
        codegen_add_variable(gen, "__gen_array__", gen_array_offset, TYPE_NUMERIC, NULL);
        codegen_add_variable(gen, "__gen_index__", gen_index_offset, TYPE_NUMERIC, NULL);
    }
    
    // Generate function body
    int has_return = 0;
    for (int i = 0; i < stmt->func_def.body_count; i++) {
        if (stmt->func_def.body[i]->type == STMT_RETURN) {
            has_return = 1;
        }
        codegen_generate_statement(gen, stmt->func_def.body[i]);
    }
    
    // Default return (if no explicit return)
    if (!has_return) {
        // Phase 14: Generator functions return their result array
        if (stmt->func_def.is_generator) {
            int arr_off = codegen_find_variable(gen, "__gen_array__");
            int idx_off = codegen_find_variable(gen, "__gen_index__");
            codegen_emit(gen, "    ; Generator: resize and return result array");
            snprintf(buffer, sizeof(buffer), "    mov rdi, [rbp-%d]     ; Array pointer", arr_off);
            codegen_emit(gen, buffer);
            snprintf(buffer, sizeof(buffer), "    mov rsi, [rbp-%d]    ; Actual size used", idx_off);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    call mlp_array_resize  ; Resize to actual size");
        }
        codegen_emit(gen, "    mov rsp, rbp");
        codegen_emit(gen, "    pop rbp");
        codegen_emit(gen, "    ret");
    }    
    // Restore previous state
    gen->stack_offset = prev_stack_offset;
    gen->variables = prev_variables;
    gen->in_function = prev_in_function;
    gen->in_operator_overload = prev_in_operator_overload;
    gen->in_generator = prev_in_generator;
}

