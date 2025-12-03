// ============================================================================
// MELP Modular Codegen - codegen_generic.c
// Generic function management (add, find, mangle, instance)
// Phase 13: Generic Functions
// ============================================================================

#include "codegen.h"

// Phase 13: Register a generic function definition
void codegen_add_generic_func(Codegen* gen, const char* name, Statement* stmt) {
    GenericFunc* gf = malloc(sizeof(GenericFunc));
    gf->name = malloc(strlen(name) + 1);
    strcpy(gf->name, name);
    gf->func_stmt = stmt;
    gf->type_params = stmt->func_def.type_params;
    gf->type_param_count = stmt->func_def.type_param_count;
    gf->next = gen->generic_funcs;
    gen->generic_funcs = gf;
}

// Phase 13: Find a generic function by name
GenericFunc* codegen_find_generic_func(Codegen* gen, const char* name) {
    GenericFunc* gf = gen->generic_funcs;
    while (gf) {
        if (strcmp(gf->name, name) == 0) {
            return gf;
        }
        gf = gf->next;
    }
    return NULL;
}

// Phase 13: Get type name for mangling
const char* codegen_type_name(VarType type) {
    switch (type) {
        case TYPE_NUMERIC: return "numeric";
        case TYPE_DECIMAL: return "decimal";
        case TYPE_BOOLEAN: return "boolean";
        case TYPE_STRING: return "string";
        default: return "unknown";
    }
}

// Phase 13: Check if a generic instance already exists
GenericInstance* codegen_find_generic_instance(Codegen* gen, const char* name, VarType* type_args, int type_arg_count) {
    GenericInstance* gi = gen->generic_instances;
    while (gi) {
        if (strcmp(gi->original_name, name) == 0 && gi->type_arg_count == type_arg_count) {
            int match = 1;
            for (int i = 0; i < type_arg_count; i++) {
                if (gi->type_args[i] != type_args[i]) {
                    match = 0;
                    break;
                }
            }
            if (match) return gi;
        }
        gi = gi->next;
    }
    return NULL;
}

// Phase 13: Create mangled name for generic instance
char* codegen_mangle_generic_name(const char* name, VarType* type_args, int type_arg_count) {
    char* mangled = malloc(256);
    strcpy(mangled, name);
    for (int i = 0; i < type_arg_count; i++) {
        strcat(mangled, "_");
        strcat(mangled, codegen_type_name(type_args[i]));
    }
    return mangled;
}

// Phase 13: Register a generic instance
void codegen_add_generic_instance(Codegen* gen, const char* original_name, const char* mangled_name, VarType* type_args, int type_arg_count) {
    GenericInstance* gi = malloc(sizeof(GenericInstance));
    gi->original_name = malloc(strlen(original_name) + 1);
    strcpy(gi->original_name, original_name);
    gi->mangled_name = malloc(strlen(mangled_name) + 1);
    strcpy(gi->mangled_name, mangled_name);
    gi->type_args = malloc(sizeof(VarType) * type_arg_count);
    memcpy(gi->type_args, type_args, sizeof(VarType) * type_arg_count);
    gi->type_arg_count = type_arg_count;
    gi->generated = 0;  // Not generated yet
    gi->next = gen->generic_instances;
    gen->generic_instances = gi;
}
