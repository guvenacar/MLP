#ifndef STANDARD_LIBRARY_H
#define STANDARD_LIBRARY_H

#include <stdio.h>
#include <stdbool.h>

// Library types
typedef enum {
    LIB_C_STANDARD,         // C standard library (libc)
    LIB_MATH,              // Math library (libm)
    LIB_PTHREAD,           // POSIX threads
    LIB_DL,                // Dynamic linking
    LIB_SYSTEM,            // System calls
    LIB_CUSTOM             // Custom MLP standard library
} LibraryType;

// Function binding
typedef struct FunctionBinding {
    char* mlp_name;             // Name in MLP code
    char* c_name;               // Name in C library
    char* return_type;          // Return type
    char** param_types;         // Parameter types
    int param_count;
    LibraryType library;        // Which library is this from
    bool is_variadic;           // Is this a variadic function?
    char* documentation;        // Function documentation
    struct FunctionBinding* next;
} FunctionBinding;

// Constant binding
typedef struct ConstantBinding {
    char* mlp_name;             // Name in MLP code
    char* c_name;               // Name in C
    char* type;                 // Constant type
    char* value;                // Constant value
    LibraryType library;
    struct ConstantBinding* next;
} ConstantBinding;

// Standard library context
typedef struct StdLibContext {
    FunctionBinding* functions;     // Function bindings
    ConstantBinding* constants;     // Constant bindings
    char** include_paths;           // Header include paths
    int include_path_count;
    char** library_paths;           // Library search paths
    int library_path_count;
    bool auto_link;                 // Automatically link required libraries
} StdLibContext;

// Standard library functions
StdLibContext* stdlib_context_create();
void stdlib_context_destroy(StdLibContext* ctx);

// Function bindings
void stdlib_add_function(StdLibContext* ctx, const char* mlp_name,
                        const char* c_name, const char* return_type,
                        const char** param_types, int param_count,
                        LibraryType library);
FunctionBinding* stdlib_lookup_function(StdLibContext* ctx, const char* mlp_name);

// Constant bindings
void stdlib_add_constant(StdLibContext* ctx, const char* mlp_name,
                        const char* c_name, const char* type,
                        const char* value, LibraryType library);
ConstantBinding* stdlib_lookup_constant(StdLibContext* ctx, const char* mlp_name);

// Initialize standard bindings
void stdlib_init_libc(StdLibContext* ctx);      // stdio, stdlib, string, etc.
void stdlib_init_math(StdLibContext* ctx);      // math.h functions
void stdlib_init_posix(StdLibContext* ctx);     // POSIX functions
void stdlib_init_system(StdLibContext* ctx);    // System calls
void stdlib_init_all(StdLibContext* ctx);       // Initialize all bindings

// Code generation helpers
void stdlib_generate_includes(FILE* out, StdLibContext* ctx);
void stdlib_generate_function_call(FILE* out, StdLibContext* ctx,
                                   const char* mlp_name, const char** args, int arg_count);
char** stdlib_get_required_libraries(StdLibContext* ctx, int* count);

// Utility
void stdlib_print_functions(StdLibContext* ctx);
void stdlib_print_constants(StdLibContext* ctx);
bool stdlib_is_stdlib_function(StdLibContext* ctx, const char* name);

#endif
