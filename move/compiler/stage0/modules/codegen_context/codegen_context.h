#ifndef CODEGEN_CONTEXT_H
#define CODEGEN_CONTEXT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "tto_types.h"  // TTO type definitions (must come before parser.h)
#include "../../codegen.h"
#include "../../parser.h"

// ============================================================================
// Forward Declarations & Constants
// ============================================================================

#define MAX_LOOP_DEPTH 32
#define MAX_GLOBAL_VARS 1024

// Loop context for break/continue
typedef struct {
    int exit_label;
    int continue_label;
} LoopContext;

// Variable info (legacy)
typedef struct {
    char* name;
    int type;  // 0=numeric, 1=text, 2=boolean
} VarInfo;

// Function context (forward declaration)
typedef struct FunctionContext {
    char* name;
    int stack_size;
    int param_count;
    void** params;  // Array of parameter structs (opaque pointer)
} FunctionContext;

// ============================================================================
// TTO (Transparent Type Optimization) Definitions
// ============================================================================
// Note: TTOTypeInfo and InternalType are defined in tto_types.h

// Variable info with TTO
typedef struct {
    char* name;
    InternalType internal_type;  // TTO-inferred type
    TTOTypeInfo tto_info;        // Full TTO analysis
    int offset;                  // Stack offset or heap pointer
} TTOVarInfo;

// ============================================================================
// Original Codegen Context
// ============================================================================

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
    
    // TTO: Enhanced variable tracking
    TTOVarInfo tto_var_types[MAX_GLOBAL_VARS];
    int tto_var_count;
    
    // Current function context
    FunctionContext* current_function_ctx;
    
    // Output file handle
    FILE* output_file;
    
    // TTO: Optimization flags
    bool tto_enabled;            // Enable TTO optimization
    bool tto_aggressive;         // Aggressive int64 usage
    bool tto_track_overflow;     // Runtime overflow detection
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

// ============================================================================
// TTO Functions
// ============================================================================

// Type inference
TTOTypeInfo tto_infer_numeric_type(const char* literal);
TTOTypeInfo tto_infer_string_type(const char* literal, bool is_constant);

// Variable tracking
void tto_add_var(CodegenContext* ctx, const char* name, TTOTypeInfo info);
TTOVarInfo* tto_get_var(CodegenContext* ctx, const char* name);

// Optimization decisions
bool tto_should_use_int64(int64_t value);
bool tto_should_use_double(const char* literal);
bool tto_should_use_sso(size_t string_length);

// Promotion helpers
void tto_generate_int64_to_bigdec_promotion(CodegenContext* ctx, const char* var_name);
void tto_generate_overflow_check(CodegenContext* ctx, const char* operation);

#endif
