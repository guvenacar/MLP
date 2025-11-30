// MELP Modular Codegen - Variable Management
// Değişken ekleme, bulma, TTO desteği

#include "codegen.h"

// ============================================================================
// Basic Variable Add
// ============================================================================

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
    var->is_global = 0;
    var->is_state = 0;
    var->init_expr = NULL;
    var->internal_numeric_type = INTERNAL_INT64;
    var->internal_string_type = INTERNAL_RODATA;
    var->is_double = 0;
    var->next = gen->variables;
    gen->variables = var;
}

// ============================================================================
// TTO Variable Add - Numeric
// ============================================================================

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
    var->is_global = 0;
    var->is_state = 0;
    var->init_expr = NULL;
    var->internal_numeric_type = internal_type;
    var->internal_string_type = INTERNAL_RODATA;
    var->is_double = (internal_type == INTERNAL_DOUBLE) ? 1 : 0;
    var->next = gen->variables;
    gen->variables = var;
}

// ============================================================================
// TTO Variable Add - String
// ============================================================================

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
    var->is_global = 0;
    var->is_state = 0;
    var->init_expr = NULL;
    var->internal_numeric_type = INTERNAL_INT64;
    var->internal_string_type = str_type;
    var->is_double = 0;
    var->next = gen->variables;
    gen->variables = var;
}

// ============================================================================
// Dynamic Array Add
// ============================================================================

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
    var->array_size = 0;
    var->is_global = 0;
    var->is_state = 0;
    var->init_expr = NULL;
    var->internal_numeric_type = INTERNAL_INT64;
    var->internal_string_type = INTERNAL_RODATA;
    var->is_double = 0;
    var->next = gen->variables;
    gen->variables = var;
}

// ============================================================================
// Pointer Variable Add
// ============================================================================

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
    var->is_global = 0;
    var->is_state = 0;
    var->init_expr = NULL;
    var->internal_numeric_type = INTERNAL_INT64;
    var->internal_string_type = INTERNAL_RODATA;
    var->is_double = 0;
    var->next = gen->variables;
    gen->variables = var;
}

// ============================================================================
// Struct Variable Add
// ============================================================================

void codegen_add_struct_variable(Codegen* gen, const char* name, int offset, const char* struct_name) {
    VarInfo* var = malloc(sizeof(VarInfo));
    var->name = malloc(strlen(name) + 1);
    strcpy(var->name, name);
    var->stack_offset = offset;
    var->type = TYPE_NUMERIC;  // Placeholder
    var->struct_name = malloc(strlen(struct_name) + 1);
    strcpy(var->struct_name, struct_name);
    var->is_dynamic_array = 0;
    var->is_pointer = 0;
    var->is_closure = 0;
    var->is_array = 0;
    var->array_size = 0;
    var->is_global = 0;
    var->is_state = 0;
    var->init_expr = NULL;
    var->internal_numeric_type = INTERNAL_INT64;
    var->internal_string_type = INTERNAL_RODATA;
    var->is_double = 0;
    var->next = gen->variables;
    gen->variables = var;
}

// ============================================================================
// Stack Array Add
// ============================================================================

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
    var->is_global = 0;
    var->is_state = 0;
    var->init_expr = NULL;
    var->internal_numeric_type = INTERNAL_INT64;
    var->internal_string_type = INTERNAL_RODATA;
    var->is_double = 0;
    var->next = gen->variables;
    gen->variables = var;
}

// ============================================================================
// Variable Lookup
// ============================================================================

int codegen_find_variable(Codegen* gen, const char* name) {
    // Check state variables first
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
    // Check state variables first - return a dummy VarInfo for them
    for (int i = 0; i < gen->global_var_count; i++) {
        if (strcmp(gen->global_vars[i].name, name) == 0) {
            static VarInfo state_var;
            state_var.name = gen->global_vars[i].name;
            state_var.type = gen->global_vars[i].type;
            state_var.stack_offset = -1;
            state_var.is_global = 1;
            state_var.is_state = gen->global_vars[i].is_state;
            state_var.is_pointer = 0;
            state_var.is_closure = 0;
            state_var.is_array = 0;
            state_var.is_dynamic_array = 0;
            state_var.internal_numeric_type = INTERNAL_INT64;
            state_var.internal_string_type = INTERNAL_RODATA;
            state_var.is_double = 0;
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
