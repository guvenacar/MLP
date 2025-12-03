// ============================================================================
// MELP Modular Codegen - codegen_core.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// Codegen create, emit, string literal yönetimi

// Forward declarations
void codegen_generate_func_def(Codegen* gen, Statement* stmt);

Codegen* codegen_create(const char* output_file) {
    Codegen* gen = malloc(sizeof(Codegen));
    if (!gen) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    
    gen->out = fopen(output_file, "w");
    if (!gen->out) {
        fprintf(stderr, "Error: Cannot open output file '%s'\n", output_file);
        perror("fopen");
        exit(1);
    }
    
    gen->stack_offset = 0;
    gen->variables = NULL;
    gen->functions = NULL;
    gen->structs = NULL;
    gen->interfaces = NULL;
    gen->enums = NULL;
    gen->type_aliases = NULL;
    gen->strings = NULL;
    gen->modules_seen = NULL;
    gen->generic_funcs = NULL;      // Phase 13: Initialize generic tracking
    gen->generic_instances = NULL;  // Phase 13: Initialize instance tracking
    gen->operator_overloads = NULL; // Phase 16: Initialize operator overloads
    gen->label_counter = 0;
    gen->loop_start_label = -1;
    gen->loop_continue_label = -1;
    gen->loop_end_label = -1;
    gen->in_loop = 0;
    gen->in_function = 0;
    gen->in_operator_overload = 0;
    gen->in_generator = 0;         // Phase 14: Initialize generator tracking
    gen->string_counter = 0;
    gen->current_module = NULL;
    gen->global_var_count = 0;     // Phase 18: Initialize state variables
    // Initialize global_vars array to zero
    memset(gen->global_vars, 0, sizeof(gen->global_vars));
    return gen;
}
