// ============================================================================
// MELP Modular Codegen - codegen_variable.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// Variable management (add, find, get)

void codegen_add_variable(Codegen* gen, const char* name, int offset, VarType type, const char* struct_name) {
    VarInfo* var = malloc(sizeof(VarInfo));
    var->name = malloc(strlen(name) + 1);
    strcpy(var->name, name);
    var->stack_offset = offset;
    var->type = type;
    if (struct_name) {
        var->struct_name = malloc(strlen(struct_name) + 1);
        strcpy(var->struct_name, struct_name);
    } else {
        var->struct_name = NULL;
    }
    var->is_dynamic_array = 0;
    var->is_pointer = 0;
    var->is_closure = 0;
    var->is_array = 0;
    var->array_size = 0;
    var->internal_numeric_type = INTERNAL_INT64;  // TTO: Default to INT64
    var->internal_string_type = INTERNAL_RODATA;  // TTO: Default to RODATA
    var->next = gen->variables;
    gen->variables = var;
}

// TTO: Add variable with specific internal numeric type
void codegen_add_variable_tto(Codegen* gen, const char* name, int offset, VarType type, InternalNumericType internal_type) {
    VarInfo* var = malloc(sizeof(VarInfo));
    var->name = malloc(strlen(name) + 1);
    strcpy(var->name, name);
    var->stack_offset = offset;
    var->type = type;
    var->struct_name = NULL;
    var->is_dynamic_array = 0;
    var->is_pointer = 0;
    var->is_closure = 0;
    var->is_array = 0;
    var->array_size = 0;
    var->internal_numeric_type = internal_type;  // TTO: Use specified type
    var->internal_string_type = INTERNAL_RODATA;  // TTO: Default to RODATA
    var->next = gen->variables;
    gen->variables = var;
}

// TTO: Add string variable with specific internal string type
void codegen_add_variable_tto_string(Codegen* gen, const char* name, int offset, VarType type, InternalStringType str_type) {
    VarInfo* var = malloc(sizeof(VarInfo));
    var->name = malloc(strlen(name) + 1);
    strcpy(var->name, name);
    var->stack_offset = offset;
    var->type = type;
    var->struct_name = NULL;
    var->is_dynamic_array = 0;
    var->is_pointer = 0;
    var->is_closure = 0;
    var->is_array = 0;
    var->array_size = 0;
    var->internal_numeric_type = INTERNAL_INT64;
    var->internal_string_type = str_type;  // TTO: Use specified string type
    var->next = gen->variables;
    gen->variables = var;
}

void codegen_add_dynamic_array(Codegen* gen, const char* name, int offset, VarType type) {
    VarInfo* var = malloc(sizeof(VarInfo));
    var->name = malloc(strlen(name) + 1);
    strcpy(var->name, name);
    var->stack_offset = offset;
    var->type = type;
    var->struct_name = NULL;
    var->is_dynamic_array = 1;
    var->is_pointer = 0;
    var->is_closure = 0;
    var->is_array = 1;
    var->array_size = 0;  // Dynamic - size unknown at compile time
    var->internal_numeric_type = INTERNAL_INT64;
    var->internal_string_type = INTERNAL_RODATA;
    var->next = gen->variables;
    gen->variables = var;
}

// Phase 10: Add pointer variable
void codegen_add_pointer_variable(Codegen* gen, const char* name, int offset, VarType type) {
    VarInfo* var = malloc(sizeof(VarInfo));
    var->name = malloc(strlen(name) + 1);
    strcpy(var->name, name);
    var->stack_offset = offset;
    var->type = type;
    var->struct_name = NULL;
    var->is_dynamic_array = 0;
    var->is_pointer = 1;
    var->is_closure = 0;
    var->is_array = 0;
    var->array_size = 0;
    var->internal_numeric_type = INTERNAL_INT64;
    var->internal_string_type = INTERNAL_RODATA;
    var->next = gen->variables;
    gen->variables = var;
}

void codegen_add_struct_variable(Codegen* gen, const char* name, int offset, const char* struct_name) {
    VarInfo* var = malloc(sizeof(VarInfo));
    var->name = malloc(strlen(name) + 1);
    strcpy(var->name, name);
    var->is_dynamic_array = 0;
    var->is_pointer = 0;
    var->is_closure = 0;
    var->is_array = 0;
    var->array_size = 0;
    var->stack_offset = offset;
    var->type = TYPE_NUMERIC; // Placeholder
    var->struct_name = malloc(strlen(struct_name) + 1);
    strcpy(var->struct_name, struct_name);
    var->internal_numeric_type = INTERNAL_INT64;
    var->internal_string_type = INTERNAL_RODATA;
    var->next = gen->variables;
    gen->variables = var;
}

// Phase 14: Add stack-allocated array variable
void codegen_add_stack_array(Codegen* gen, const char* name, int offset, VarType type, int array_size) {
    VarInfo* var = malloc(sizeof(VarInfo));
    var->name = malloc(strlen(name) + 1);
    strcpy(var->name, name);
    var->stack_offset = offset;
    var->type = type;
    var->struct_name = NULL;
    var->is_dynamic_array = 0;
    var->is_pointer = 0;
    var->is_closure = 0;
    var->is_array = 1;
    var->array_size = array_size;
    var->internal_numeric_type = INTERNAL_INT64;
    var->internal_string_type = INTERNAL_RODATA;
    var->next = gen->variables;
    gen->variables = var;
}

int codegen_find_variable(Codegen* gen, const char* name) {
    // Phase 18: Check state variables first
    for (int i = 0; i < gen->global_var_count; i++) {
        if (strcmp(gen->global_vars[i].name, name) == 0) {
            return -1;  // Special marker for state variables
        }
    }
    VarInfo* current = gen->variables;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current->stack_offset;
        }
        current = current->next;
    }
    fprintf(stderr, "Codegen error: Undefined variable '%s'\n", name);
    exit(1);
}

VarInfo* codegen_get_variable(Codegen* gen, const char* name) {
    // Phase 18: Check state variables first - return a dummy VarInfo for them
    for (int i = 0; i < gen->global_var_count; i++) {
        if (strcmp(gen->global_vars[i].name, name) == 0) {
            // Return state var info as VarInfo
            static VarInfo state_var;
            state_var.name = gen->global_vars[i].name;
            state_var.type = gen->global_vars[i].type;
            state_var.stack_offset = -1;  // Special marker
            state_var.is_global = 1;
            state_var.is_pointer = 0;
            state_var.is_closure = 0;
            state_var.is_array = 0;
            state_var.internal_numeric_type = INTERNAL_INT64;
            state_var.next = NULL;
            return &state_var;
        }
    }
    VarInfo* current = gen->variables;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    fprintf(stderr, "Codegen error: Undefined variable '%s'\n", name);
    exit(1);
}
