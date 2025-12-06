#include "standard_library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create standard library context
StdLibContext* stdlib_context_create() {
    StdLibContext* ctx = malloc(sizeof(StdLibContext));
    memset(ctx, 0, sizeof(StdLibContext));
    ctx->auto_link = true;
    return ctx;
}

// Destroy standard library context
void stdlib_context_destroy(StdLibContext* ctx) {
    if (!ctx) return;

    // Free function bindings
    FunctionBinding* func = ctx->functions;
    while (func) {
        FunctionBinding* next = func->next;
        if (func->mlp_name) free(func->mlp_name);
        if (func->c_name) free(func->c_name);
        if (func->return_type) free(func->return_type);
        if (func->documentation) free(func->documentation);
        // TODO: Free param_types
        free(func);
        func = next;
    }

    // Free constant bindings
    ConstantBinding* const_bind = ctx->constants;
    while (const_bind) {
        ConstantBinding* next = const_bind->next;
        if (const_bind->mlp_name) free(const_bind->mlp_name);
        if (const_bind->c_name) free(const_bind->c_name);
        if (const_bind->type) free(const_bind->type);
        if (const_bind->value) free(const_bind->value);
        free(const_bind);
        const_bind = next;
    }

    // TODO: Free include_paths and library_paths
    free(ctx);
}

// Add function binding
void stdlib_add_function(StdLibContext* ctx, const char* mlp_name,
                        const char* c_name, const char* return_type,
                        const char** param_types, int param_count,
                        LibraryType library) {
    FunctionBinding* binding = malloc(sizeof(FunctionBinding));
    memset(binding, 0, sizeof(FunctionBinding));

    binding->mlp_name = strdup(mlp_name);
    binding->c_name = strdup(c_name);
    binding->return_type = strdup(return_type);
    binding->param_count = param_count;
    binding->library = library;

    // TODO: Copy param_types array

    // Add to list
    binding->next = ctx->functions;
    ctx->functions = binding;
}

// Lookup function binding
FunctionBinding* stdlib_lookup_function(StdLibContext* ctx, const char* mlp_name) {
    FunctionBinding* func = ctx->functions;
    while (func) {
        if (strcmp(func->mlp_name, mlp_name) == 0) {
            return func;
        }
        func = func->next;
    }
    return NULL;
}

// Add constant binding
void stdlib_add_constant(StdLibContext* ctx, const char* mlp_name,
                        const char* c_name, const char* type,
                        const char* value, LibraryType library) {
    ConstantBinding* binding = malloc(sizeof(ConstantBinding));
    memset(binding, 0, sizeof(ConstantBinding));

    binding->mlp_name = strdup(mlp_name);
    binding->c_name = strdup(c_name);
    binding->type = strdup(type);
    binding->value = strdup(value);
    binding->library = library;

    // Add to list
    binding->next = ctx->constants;
    ctx->constants = binding;
}

// Lookup constant binding
ConstantBinding* stdlib_lookup_constant(StdLibContext* ctx, const char* mlp_name) {
    ConstantBinding* const_bind = ctx->constants;
    while (const_bind) {
        if (strcmp(const_bind->mlp_name, mlp_name) == 0) {
            return const_bind;
        }
        const_bind = const_bind->next;
    }
    return NULL;
}

// Initialize libc bindings
void stdlib_init_libc(StdLibContext* ctx) {
    printf("Initializing libc bindings...\n");

    // stdio.h
    const char* printf_params[] = {"string", "..."};
    stdlib_add_function(ctx, "printf", "printf", "int",
                       printf_params, 1, LIB_C_STANDARD);

    const char* scanf_params[] = {"string", "..."};
    stdlib_add_function(ctx, "scanf", "scanf", "int",
                       scanf_params, 1, LIB_C_STANDARD);

    const char* fopen_params[] = {"string", "string"};
    stdlib_add_function(ctx, "fopen", "fopen", "FILE*",
                       fopen_params, 2, LIB_C_STANDARD);

    // stdlib.h
    const char* malloc_params[] = {"size_t"};
    stdlib_add_function(ctx, "malloc", "malloc", "void*",
                       malloc_params, 1, LIB_C_STANDARD);

    const char* free_params[] = {"void*"};
    stdlib_add_function(ctx, "free", "free", "void",
                       free_params, 1, LIB_C_STANDARD);

    // string.h
    const char* strlen_params[] = {"string"};
    stdlib_add_function(ctx, "strlen", "strlen", "size_t",
                       strlen_params, 1, LIB_C_STANDARD);

    const char* strcmp_params[] = {"string", "string"};
    stdlib_add_function(ctx, "strcmp", "strcmp", "int",
                       strcmp_params, 2, LIB_C_STANDARD);

    // Constants
    stdlib_add_constant(ctx, "NULL", "NULL", "void*", "0", LIB_C_STANDARD);
    stdlib_add_constant(ctx, "EOF", "EOF", "int", "-1", LIB_C_STANDARD);

    printf("  - Added %d libc function bindings\n", 7);
}

// Initialize math library bindings
void stdlib_init_math(StdLibContext* ctx) {
    printf("Initializing math library bindings...\n");

    const char* single_double[] = {"double"};
    const char* two_doubles[] = {"double", "double"};

    stdlib_add_function(ctx, "sin", "sin", "double", single_double, 1, LIB_MATH);
    stdlib_add_function(ctx, "cos", "cos", "double", single_double, 1, LIB_MATH);
    stdlib_add_function(ctx, "tan", "tan", "double", single_double, 1, LIB_MATH);
    stdlib_add_function(ctx, "sqrt", "sqrt", "double", single_double, 1, LIB_MATH);
    stdlib_add_function(ctx, "pow", "pow", "double", two_doubles, 2, LIB_MATH);
    stdlib_add_function(ctx, "log", "log", "double", single_double, 1, LIB_MATH);
    stdlib_add_function(ctx, "exp", "exp", "double", single_double, 1, LIB_MATH);

    // Constants
    stdlib_add_constant(ctx, "PI", "M_PI", "double", "3.14159265358979323846", LIB_MATH);
    stdlib_add_constant(ctx, "E", "M_E", "double", "2.71828182845904523536", LIB_MATH);

    printf("  - Added %d math function bindings\n", 7);
}

// Initialize POSIX bindings
void stdlib_init_posix(StdLibContext* ctx) {
    printf("Initializing POSIX bindings...\n");
    // TODO: Add pthread, unistd functions
    (void)ctx;
}

// Initialize system call bindings
void stdlib_init_system(StdLibContext* ctx) {
    printf("Initializing system call bindings...\n");
    // TODO: Add syscall wrappers
    (void)ctx;
}

// Initialize all bindings
void stdlib_init_all(StdLibContext* ctx) {
    printf("\nInitializing all standard library bindings...\n");
    printf("=============================================\n");
    stdlib_init_libc(ctx);
    stdlib_init_math(ctx);
    stdlib_init_posix(ctx);
    stdlib_init_system(ctx);
    printf("\nStandard library initialization complete.\n\n");
}

// Generate include directives
void stdlib_generate_includes(FILE* out, StdLibContext* ctx) {
    // TODO: Generate #include directives based on used functions
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include <stdlib.h>\n");
    fprintf(out, "#include <string.h>\n");
    fprintf(out, "#include <math.h>\n");
    (void)ctx;
}

// Generate function call
void stdlib_generate_function_call(FILE* out, StdLibContext* ctx,
                                   const char* mlp_name, const char** args, int arg_count) {
    FunctionBinding* binding = stdlib_lookup_function(ctx, mlp_name);
    if (!binding) {
        fprintf(stderr, "Unknown stdlib function: %s\n", mlp_name);
        return;
    }

    // Generate C function call
    fprintf(out, "    %s(", binding->c_name);
    for (int i = 0; i < arg_count; i++) {
        fprintf(out, "%s", args[i]);
        if (i < arg_count - 1) fprintf(out, ", ");
    }
    fprintf(out, ");\n");
}

// Get required libraries
char** stdlib_get_required_libraries(StdLibContext* ctx, int* count) {
    // TODO: Analyze used functions and return required libraries
    // Example: ["c", "m", "pthread"]
    (void)ctx;
    *count = 0;
    return NULL;
}

// Print all functions
void stdlib_print_functions(StdLibContext* ctx) {
    printf("\nStandard Library Functions:\n");
    printf("===========================\n");

    FunctionBinding* func = ctx->functions;
    int count = 0;
    while (func) {
        printf("  %s -> %s : %s\n", func->mlp_name, func->c_name, func->return_type);
        count++;
        func = func->next;
    }

    printf("\nTotal: %d functions\n", count);
}

// Print all constants
void stdlib_print_constants(StdLibContext* ctx) {
    printf("\nStandard Library Constants:\n");
    printf("===========================\n");

    ConstantBinding* const_bind = ctx->constants;
    int count = 0;
    while (const_bind) {
        printf("  %s -> %s : %s = %s\n",
               const_bind->mlp_name, const_bind->c_name,
               const_bind->type, const_bind->value);
        count++;
        const_bind = const_bind->next;
    }

    printf("\nTotal: %d constants\n", count);
}

// Check if function is stdlib function
bool stdlib_is_stdlib_function(StdLibContext* ctx, const char* name) {
    return stdlib_lookup_function(ctx, name) != NULL;
}
