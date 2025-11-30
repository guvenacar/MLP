/**
 * MLP Codegen - Function Definition Code Generation
 * 
 * Handles user-defined function generation including:
 * - Regular functions
 * - Generic functions (template instantiation)
 * - Async functions
 * - Generator functions
 * - Parameter handling
 * - Local variable setup
 */

#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
void codegen_generate_statement(Codegen* gen, Statement* stmt);

void codegen_generate_func_def(Codegen* gen, Statement* stmt) {
    char buffer[256];
    
    // Phase 13: Check if this is a generic function definition
    if (stmt->func_def.type_param_count > 0) {
        // Generic function - register for later instantiation
        codegen_add_generic_func(gen, stmt->func_def.func_name, stmt);
        
        // Don't generate code yet - will be done during instantiation
        snprintf(buffer, sizeof(buffer), "; Generic function '%s' with %d type parameters - deferred",
                 stmt->func_def.func_name, stmt->func_def.type_param_count);
        codegen_emit(gen, buffer);
        return;
    }
    
    // Save current state
    VarInfo* saved_vars = gen->variables;
    int saved_stack_offset = gen->stack_offset;
    int saved_in_function = gen->in_function;
    
    gen->variables = NULL;
    gen->stack_offset = 0;
    gen->in_function = 1;
    
    // Generate function label
    codegen_emit(gen, "");
    
    // Check if module-prefixed function
    if (gen->current_module) {
        snprintf(buffer, sizeof(buffer), "; Module function: %s.%s", 
                 gen->current_module, stmt->func_def.func_name);
        codegen_emit(gen, buffer);
        snprintf(buffer, sizeof(buffer), "%s_%s:", 
                 gen->current_module, stmt->func_def.func_name);
    } else {
        snprintf(buffer, sizeof(buffer), "; Function: %s", stmt->func_def.func_name);
        codegen_emit(gen, buffer);
        snprintf(buffer, sizeof(buffer), "func_%s:", stmt->func_def.func_name);
    }
    codegen_emit(gen, buffer);
    
    // Register function in function table
    codegen_add_function(gen, stmt->func_def.func_name, 
                        stmt->func_def.param_count, stmt->func_def.param_types,
                        stmt->func_def.return_type, stmt->func_def.return_struct_name);
    
    // Function prologue
    codegen_emit(gen, "    push rbp");
    codegen_emit(gen, "    mov rbp, rsp");
    
    // Phase 14: Generator function setup
    if (stmt->func_def.is_generator) {
        codegen_emit(gen, "    ; Generator function setup");
        // Allocate result array
        codegen_emit(gen, "    mov rdi, 64         ; Initial array capacity (8 elements)");
        codegen_emit(gen, "    call mlp_array_alloc");
        gen->stack_offset += 8;
        codegen_add_variable(gen, "__gen_array__", gen->stack_offset, TYPE_NUMERIC, NULL);
        codegen_emit(gen, "    push rax            ; Save result array");
        
        // Initialize index
        gen->stack_offset += 8;
        codegen_add_variable(gen, "__gen_index__", gen->stack_offset, TYPE_NUMERIC, NULL);
        codegen_emit(gen, "    push 0              ; Initialize index to 0");
    }
    
    // Phase 14: Async function setup
    if (stmt->func_def.is_async) {
        codegen_emit(gen, "    ; Async function marker (compiled as sync for now)");
    }
    
    // Parameters are pushed right-to-left, so first param is at [rbp+16]
    // (rbp+0 = old rbp, rbp+8 = return address, rbp+16 = first param)
    for (int i = 0; i < stmt->func_def.param_count; i++) {
        int param_offset = 16 + (i * 8);  // Parameters start at rbp+16
        
        // Allocate local copy of parameter
        gen->stack_offset += 8;
        codegen_add_variable(gen, stmt->func_def.param_names[i], gen->stack_offset,
                            stmt->func_def.param_types[i], 
                            stmt->func_def.param_struct_names ? 
                                stmt->func_def.param_struct_names[i] : NULL);
        
        // Copy parameter from stack to local variable
        snprintf(buffer, sizeof(buffer), "    mov rax, [rbp+%d]   ; Load param %s",
                 param_offset, stmt->func_def.param_names[i]);
        codegen_emit(gen, buffer);
        snprintf(buffer, sizeof(buffer), "    push rax            ; Store as local var");
        codegen_emit(gen, buffer);
    }
    
    // Generate function body
    for (int i = 0; i < stmt->func_def.body_count; i++) {
        codegen_generate_statement(gen, stmt->func_def.body[i]);
    }
    
    // Phase 14: Generator function epilogue
    if (stmt->func_def.is_generator) {
        codegen_emit(gen, "    ; Generator function epilogue");
        int arr_offset = codegen_find_variable(gen, "__gen_array__");
        int idx_offset = codegen_find_variable(gen, "__gen_index__");
        
        // Create result array with actual size
        snprintf(buffer, sizeof(buffer), "    mov rdi, [rbp-%d]   ; Load result array", arr_offset);
        codegen_emit(gen, buffer);
        snprintf(buffer, sizeof(buffer), "    mov rsi, [rbp-%d]   ; Load count", idx_offset);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    call mlp_array_slice  ; Create properly sized array");
        // rax now contains the result array pointer
    }
    
    // Function epilogue (if no return statement reached)
    codegen_emit(gen, "    mov rsp, rbp");
    codegen_emit(gen, "    pop rbp");
    codegen_emit(gen, "    ret");
    codegen_emit(gen, "");
    
    // Restore state
    // Free local variables
    while (gen->variables) {
        VarInfo* next = gen->variables->next;
        free(gen->variables->name);
        free(gen->variables);
        gen->variables = next;
    }
    
    gen->variables = saved_vars;
    gen->stack_offset = saved_stack_offset;
    gen->in_function = saved_in_function;
}
