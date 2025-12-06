#include "codegen_context.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>

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
    
    // Check function parameters (TODO: implement proper param structure)
    if (ctx->current_function_ctx && ctx->current_function_ctx->params) {
        // For now, skip param check - params structure not fully defined
        // This will be implemented in Phase 2.3
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

// ============================================================================
// TTO (Transparent Type Optimization) IMPLEMENTATION
// ============================================================================

// Infer internal type for numeric literal
TTOTypeInfo tto_infer_numeric_type(const char* literal) {
    TTOTypeInfo info = {0};
    info.is_constant = true;
    info.needs_promotion = false;
    
    // Check if it's a floating point number
    bool has_dot = (strchr(literal, '.') != NULL);
    bool has_e = (strchr(literal, 'e') != NULL || strchr(literal, 'E') != NULL);
    
    if (has_dot || has_e) {
        // Floating point number
        info.type = INTERNAL_TYPE_DOUBLE;
        info.mem_location = MEM_REGISTER; // XMM register
        info.const_value.double_value = atof(literal);
        return info;
    }
    
    // Integer literal - try to parse as int64
    char* endptr;
    errno = 0;  // Clear errno before call
    long long value = strtoll(literal, &endptr, 10);
    
    // Check for overflow or conversion error
    if (errno == ERANGE || *endptr != '\0') {
        // Overflow or invalid format → use BigDecimal
        info.type = INTERNAL_TYPE_BIGDECIMAL;
        info.mem_location = MEM_HEAP;
        return info;
    }
    
    // Successfully converted - check range
    if (value >= INT64_MIN && value <= INT64_MAX) {
        // Fits in int64
        info.type = INTERNAL_TYPE_INT64;
        info.mem_location = MEM_REGISTER; // RAX register
        info.const_value.int_value = (int64_t)value;
        info.needs_promotion = true; // May overflow at runtime
        return info;
    }
    
    // Too big for int64, use BigDecimal
    info.type = INTERNAL_TYPE_BIGDECIMAL;
    info.mem_location = MEM_HEAP;
    return info;
}

// Infer internal type for string literal
TTOTypeInfo tto_infer_string_type(const char* literal, bool is_constant) {
    TTOTypeInfo info = {0};
    info.is_constant = is_constant;
    
    size_t length = strlen(literal);
    
    if (is_constant) {
        // Constant string → .rodata section
        info.type = INTERNAL_TYPE_RODATA_STRING;
        info.mem_location = MEM_RODATA;
        info.const_value.string_value = (char*)literal;
        return info;
    }
    
    if (length <= 23) {
        // Small String Optimization (SSO)
        info.type = INTERNAL_TYPE_SSO_STRING;
        info.mem_location = MEM_STACK;
        return info;
    }
    
    // Heap string
    info.type = INTERNAL_TYPE_HEAP_STRING;
    info.mem_location = MEM_HEAP;
    return info;
}

// Add variable with TTO info
void tto_add_var(CodegenContext* ctx, const char* name, TTOTypeInfo info) {
    if (ctx->tto_var_count >= MAX_GLOBAL_VARS) {
        fprintf(stderr, "Error: Maximum TTO variables exceeded\n");
        return;
    }
    
    ctx->tto_var_types[ctx->tto_var_count].name = strdup(name);
    ctx->tto_var_types[ctx->tto_var_count].internal_type = info.type;
    ctx->tto_var_types[ctx->tto_var_count].tto_info = info;
    ctx->tto_var_count++;
}

// Get TTO variable info
TTOVarInfo* tto_get_var(CodegenContext* ctx, const char* name) {
    for (int i = 0; i < ctx->tto_var_count; i++) {
        if (strcmp(ctx->tto_var_types[i].name, name) == 0) {
            return &ctx->tto_var_types[i];
        }
    }
    return NULL;
}

// Check if value fits in int64
bool tto_should_use_int64(int64_t value) {
    return (value >= INT64_MIN && value <= INT64_MAX);
}

// Check if float literal should use double
bool tto_should_use_double(const char* literal) {
    // Count significant digits
    int digits = 0;
    bool after_dot = false;
    
    for (const char* p = literal; *p; p++) {
        if (*p == '.') {
            after_dot = true;
        } else if (isdigit(*p)) {
            digits++;
        }
    }
    
    // Double has ~15-17 decimal digits precision
    return (digits <= 15);
}

// Check if string should use SSO
bool tto_should_use_sso(size_t string_length) {
    return (string_length <= 23);
}

// Generate int64 to BigDecimal promotion code
void tto_generate_int64_to_bigdec_promotion(CodegenContext* ctx, const char* var_name) {
    FILE* out = ctx->output_file;
    
    fprintf(out, "    ; TTO: Promote int64 to BigDecimal (%s)\n", var_name);
    fprintf(out, "    mov rdi, [rbp-%d]    ; Load int64 value\n", 8); // Example offset
    fprintf(out, "    call bigdec_from_int64\n");
    fprintf(out, "    mov [rbp-%d], rax    ; Store BigDecimal pointer\n", 8);
}

// Generate overflow check code
void tto_generate_overflow_check(CodegenContext* ctx, const char* operation) {
    FILE* out = ctx->output_file;
    int overflow_label = codegen_context_next_label(ctx);
    int continue_label = codegen_context_next_label(ctx);
    
    fprintf(out, "    ; TTO: Overflow check for %s\n", operation);
    fprintf(out, "    jo .L_overflow_%d    ; Jump if overflow\n", overflow_label);
    fprintf(out, "    jmp .L_continue_%d\n", continue_label);
    fprintf(out, ".L_overflow_%d:\n", overflow_label);
    fprintf(out, "    ; Promote to BigDecimal and retry\n");
    fprintf(out, "    call bigdec_promote_and_retry\n");
    fprintf(out, ".L_continue_%d:\n", continue_label);
}
