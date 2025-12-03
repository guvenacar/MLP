#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include "modules/variable/variable.h"

// Maximum local variables per function
#define MAX_LOCAL_VARS 32
#define MAX_GLOBAL_VARS 64
#define MAX_STRING_LITERALS 256

// Variable info for type tracking
typedef struct {
    char* name;
    VarType type;
} VarInfo;

// String literal info for expression contexts
typedef struct {
    char* value;
    int id;
} StringLiteralInfo;

// Global variable type table
extern VarInfo global_var_types[MAX_GLOBAL_VARS];
extern int global_var_count;

// Global print string counter (shared between main codegen and control_flow)
extern int print_string_counter;

// String literals registry (for expression contexts)
extern StringLiteralInfo string_literals[MAX_STRING_LITERALS];
extern int string_literal_count;

// Add a string literal to registry and return its ID
int add_string_literal(const char* value);

// Function context for tracking parameters and locals during code generation
typedef struct {
    const char* func_name;
    char** param_names;
    int param_count;
    // Local variables (dynamically added during codegen)
    char* local_names[MAX_LOCAL_VARS];
    VarType local_types[MAX_LOCAL_VARS];
    int local_count;
    int next_local_offset;  // Next available stack offset for locals
} FunctionContext;

// Current function context (NULL when not in a function)
extern FunctionContext* current_function_ctx;

void codegen_generate(Program* program, const char* output_file);

// Helper to check if a variable is a parameter and get its stack offset
// Returns -1 if not a parameter, otherwise returns stack offset (8, 16, 24, ...)
int get_param_stack_offset(const char* var_name);

// Helper to get local variable stack offset, or add new local
// Returns stack offset for the local variable
int get_or_add_local_offset(const char* var_name);

// Check if variable is local (not global)
int is_local_variable(const char* var_name);

// Variable type tracking
void register_global_var(const char* name, VarType type);
VarType get_var_type(const char* name);

#endif
