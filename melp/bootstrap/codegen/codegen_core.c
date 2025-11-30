// MELP Modular Codegen - Core Functions
// Codegen oluşturma, emit, string literal yönetimi

#include "codegen.h"
#include "../parser.c"

// ============================================================================
// Codegen Create/Free
// ============================================================================

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
    gen->generic_funcs = NULL;
    gen->generic_instances = NULL;
    gen->operator_overloads = NULL;
    gen->label_counter = 0;
    gen->loop_start_label = -1;
    gen->loop_continue_label = -1;
    gen->loop_end_label = -1;
    gen->in_loop = 0;
    gen->in_function = 0;
    gen->in_operator_overload = 0;
    gen->in_generator = 0;
    gen->string_counter = 0;
    gen->current_module = NULL;
    gen->global_var_count = 0;
    memset(gen->global_vars, 0, sizeof(gen->global_vars));
    return gen;
}

void codegen_free(Codegen* gen) {
    fclose(gen->out);
    free(gen);
}

// ============================================================================
// Emit
// ============================================================================

void codegen_emit(Codegen* gen, const char* line) {
    fprintf(gen->out, "%s\n", line);
}

// ============================================================================
// String Literal Management
// ============================================================================

int codegen_add_string(Codegen* gen, const char* value) {
    int id = gen->string_counter++;
    StringLiteral* str = malloc(sizeof(StringLiteral));
    str->id = id;
    str->value = malloc(strlen(value) + 1);
    strcpy(str->value, value);
    str->next = gen->strings;
    gen->strings = str;
    return id;
}
