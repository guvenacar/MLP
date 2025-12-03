#include "codegen_context.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ============================================================================
// CONTEXT CREATION AND DESTRUCTION
// ============================================================================

CodegenContext* codegen_context_create(FILE* output_file) {
    CodegenContext* ctx = malloc(sizeof(CodegenContext));
    
    // Initialize counters
    ctx->label_counter = 0;
    ctx->print_string_counter = 0;
    
    // Initialize loop stack
    ctx->loop_depth = 0;
    
    // Initialize variable tracking
    ctx->global_var_count = 0;
    
    // Initialize function context
    ctx->current_function_ctx = NULL;
    
    // Set output file
    ctx->output_file = output_file;
    
    return ctx;
}

void codegen_context_destroy(CodegenContext* ctx) {
    if (ctx) {
        free(ctx);
    }
}

// ============================================================================
// LABEL GENERATION
// ============================================================================

int codegen_context_next_label(CodegenContext* ctx) {
    return ctx->label_counter++;
}

// ============================================================================
// STRING COUNTER
// ============================================================================

int codegen_context_next_string_id(CodegenContext* ctx) {
    return ctx->print_string_counter++;
}

// ============================================================================
// LOOP STACK MANAGEMENT
// ============================================================================

void codegen_context_push_loop(CodegenContext* ctx, int exit_label, int continue_label) {
    if (ctx->loop_depth >= MAX_LOOP_DEPTH) {
        fprintf(stderr, "Error: Maximum loop nesting depth exceeded\n");
        return;
    }
    
    ctx->loop_stack[ctx->loop_depth].exit_label = exit_label;
    ctx->loop_stack[ctx->loop_depth].continue_label = continue_label;
    ctx->loop_depth++;
}

void codegen_context_pop_loop(CodegenContext* ctx) {
    if (ctx->loop_depth > 0) {
        ctx->loop_depth--;
    }
}

LoopContext* codegen_context_current_loop(CodegenContext* ctx) {
    if (ctx->loop_depth > 0) {
        return &ctx->loop_stack[ctx->loop_depth - 1];
    }
    return NULL;
}

// ============================================================================
// VARIABLE TYPE TRACKING
// ============================================================================

void codegen_context_add_var(CodegenContext* ctx, const char* name, int type) {
    if (ctx->global_var_count >= MAX_GLOBAL_VARS) {
        fprintf(stderr, "Error: Maximum global variables exceeded\n");
        return;
    }
    
    strncpy(ctx->global_var_types[ctx->global_var_count].name, name, 255);
    ctx->global_var_types[ctx->global_var_count].name[255] = '\0';
    ctx->global_var_types[ctx->global_var_count].type = type;
    ctx->global_var_count++;
}

int codegen_context_get_var_type(CodegenContext* ctx, const char* name) {
    // Search in global variables
    for (int i = 0; i < ctx->global_var_count; i++) {
        if (strcmp(ctx->global_var_types[i].name, name) == 0) {
            return ctx->global_var_types[i].type;
        }
    }
    
    // Check function parameters
    if (ctx->current_function_ctx) {
        for (int i = 0; i < ctx->current_function_ctx->param_count; i++) {
            if (strcmp(ctx->current_function_ctx->params[i]->name, name) == 0) {
                return ctx->current_function_ctx->params[i]->type;
            }
        }
    }
    
    return 0; // VAR_NUMERIC default
}

// ============================================================================
// FUNCTION CONTEXT
// ============================================================================

void codegen_context_set_function(CodegenContext* ctx, FunctionContext* func_ctx) {
    ctx->current_function_ctx = func_ctx;
}

FunctionContext* codegen_context_get_function(CodegenContext* ctx) {
    return ctx->current_function_ctx;
}
