#ifndef CODEGEN_CONTEXT_H
#define CODEGEN_CONTEXT_H

#include <stdio.h>
#include "../../codegen.h"
#include "../../parser.h"

// Global codegen state - centralized context
typedef struct {
    // Label generation
    int label_counter;
    
    // String literal tracking
    int print_string_counter;
    
    // Loop context stack (for exit/continue)
    LoopContext loop_stack[MAX_LOOP_DEPTH];
    int loop_depth;
    
    // Global variable type tracking
    VarInfo global_var_types[MAX_GLOBAL_VARS];
    int global_var_count;
    
    // Current function context
    FunctionContext* current_function_ctx;
    
    // Output file handle
    FILE* output_file;
} CodegenContext;

// Context management
CodegenContext* codegen_context_create(FILE* output_file);
void codegen_context_destroy(CodegenContext* ctx);

// Label generation
int codegen_context_next_label(CodegenContext* ctx);

// String counter
int codegen_context_next_string_id(CodegenContext* ctx);

// Loop stack management
void codegen_context_push_loop(CodegenContext* ctx, int exit_label, int continue_label);
void codegen_context_pop_loop(CodegenContext* ctx);
LoopContext* codegen_context_current_loop(CodegenContext* ctx);

// Variable type tracking
void codegen_context_add_var(CodegenContext* ctx, const char* name, int type);
int codegen_context_get_var_type(CodegenContext* ctx, const char* name);

// Function context
void codegen_context_set_function(CodegenContext* ctx, FunctionContext* func_ctx);
FunctionContext* codegen_context_get_function(CodegenContext* ctx);

#endif
