#define _POSIX_C_SOURCE 200809L
#include "dependent_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// DEPENDENT TYPE SYSTEM IMPLEMENTATION
// ============================================================================

DepTypeContext* dep_context_create(void) {
    DepTypeContext* ctx = malloc(sizeof(DepTypeContext));
    if (!ctx) return NULL;
    
    ctx->type_capacity = 16;
    ctx->types = malloc(sizeof(DepType*) * ctx->type_capacity);
    if (!ctx->types) {
        free(ctx);
        return NULL;
    }
    
    ctx->type_count = 0;
    ctx->total_checks = 0;
    ctx->violations = 0;
    
    return ctx;
}

void dep_context_destroy(DepTypeContext* ctx) {
    if (!ctx) return;
    
    for (size_t i = 0; i < ctx->type_count; i++) {
        dep_type_free(ctx->types[i]);
    }
    
    free(ctx->types);
    free(ctx);
}

DepType* dep_type_create(const char* base_type) {
    DepType* type = malloc(sizeof(DepType));
    if (!type) return NULL;
    
    type->base_type = strdup(base_type);
    if (!type->base_type) {
        free(type);
        return NULL;
    }
    
    type->param_count = 0;
    type->params = NULL;
    type->is_dependent = false;
    
    return type;
}

void dep_type_add_param(DepType* type, DepValue* value) {
    if (!type || !value) return;
    
    type->params = realloc(type->params, sizeof(DepValue*) * (type->param_count + 1));
    type->params[type->param_count++] = value;
    type->is_dependent = true;
}

void dep_type_free(DepType* type) {
    if (!type) return;
    
    free(type->base_type);
    
    for (size_t i = 0; i < type->param_count; i++) {
        dep_value_free(type->params[i]);
    }
    
    free(type->params);
    free(type);
}

DepValue* dep_value_int(long val) {
    DepValue* value = malloc(sizeof(DepValue));
    if (!value) return NULL;
    
    value->type = DEP_VALUE_INT;
    value->data.int_val = val;
    
    return value;
}

DepValue* dep_value_string(const char* val) {
    DepValue* value = malloc(sizeof(DepValue));
    if (!value) return NULL;
    
    value->type = DEP_VALUE_STRING;
    value->data.str_val = strdup(val);
    if (!value->data.str_val) {
        free(value);
        return NULL;
    }
    
    return value;
}

DepValue* dep_value_bool(bool val) {
    DepValue* value = malloc(sizeof(DepValue));
    if (!value) return NULL;
    
    value->type = DEP_VALUE_BOOL;
    value->data.bool_val = val;
    
    return value;
}

DepValue* dep_value_variable(const char* name) {
    DepValue* value = malloc(sizeof(DepValue));
    if (!value) return NULL;
    
    value->type = DEP_VALUE_VARIABLE;
    value->data.var_name = strdup(name);
    if (!value->data.var_name) {
        free(value);
        return NULL;
    }
    
    return value;
}

void dep_value_free(DepValue* val) {
    if (!val) return;
    
    if (val->type == DEP_VALUE_STRING) {
        free(val->data.str_val);
    } else if (val->type == DEP_VALUE_VARIABLE) {
        free(val->data.var_name);
    }
    
    free(val);
}

void dep_register_type(DepTypeContext* ctx, DepType* type) {
    if (!ctx || !type) return;
    
    if (ctx->type_count >= ctx->type_capacity) {
        ctx->type_capacity *= 2;
        ctx->types = realloc(ctx->types, sizeof(DepType*) * ctx->type_capacity);
    }
    
    ctx->types[ctx->type_count++] = type;
}

bool dep_type_compatible(DepType* type1, DepType* type2) {
    if (!type1 || !type2) return false;
    
    // Base types must match
    if (strcmp(type1->base_type, type2->base_type) != 0) {
        return false;
    }
    
    // Parameter counts must match
    if (type1->param_count != type2->param_count) {
        return false;
    }
    
    // Check each parameter
    for (size_t i = 0; i < type1->param_count; i++) {
        DepValue* v1 = type1->params[i];
        DepValue* v2 = type2->params[i];
        
        // Types must match
        if (v1->type != v2->type) {
            return false;
        }
        
        // Values must match
        switch (v1->type) {
            case DEP_VALUE_INT:
                if (v1->data.int_val != v2->data.int_val) {
                    return false;
                }
                break;
            case DEP_VALUE_STRING:
                if (strcmp(v1->data.str_val, v2->data.str_val) != 0) {
                    return false;
                }
                break;
            case DEP_VALUE_BOOL:
                if (v1->data.bool_val != v2->data.bool_val) {
                    return false;
                }
                break;
            case DEP_VALUE_VARIABLE:
                // Variables: must have same name
                if (strcmp(v1->data.var_name, v2->data.var_name) != 0) {
                    return false;
                }
                break;
        }
    }
    
    return true;
}

bool dep_type_check(DepTypeContext* ctx, DepType* type1, DepType* type2) {
    if (!ctx) return false;
    
    ctx->total_checks++;
    
    bool compatible = dep_type_compatible(type1, type2);
    
    if (!compatible) {
        ctx->violations++;
        fprintf(stderr, "ERROR: Type mismatch - %s vs %s\n",
                type1->base_type, type2->base_type);
    }
    
    return compatible;
}

// ============================================================================
// PARSER
// ============================================================================

DepType* dep_parse_type(const char* type_str) {
    if (!type_str) return NULL;
    
    // Expected format:
    // "Vec<5>"           -> Vec with param 5
    // "Matrix<3, 4>"     -> Matrix with params 3, 4
    // "Array<n>"         -> Array with variable n
    
    char base[128];
    char params[256] = {0};
    
    // Find '<'
    const char* angle = strchr(type_str, '<');
    if (!angle) {
        // Simple type without parameters
        return dep_type_create(type_str);
    }
    
    // Extract base type
    size_t base_len = angle - type_str;
    strncpy(base, type_str, base_len);
    base[base_len] = '\0';
    
    DepType* type = dep_type_create(base);
    if (!type) return NULL;
    
    // Extract parameters
    const char* end = strchr(angle, '>');
    if (!end) {
        dep_type_free(type);
        return NULL;
    }
    
    size_t param_len = end - angle - 1;
    strncpy(params, angle + 1, param_len);
    params[param_len] = '\0';
    
    // Parse each parameter
    char* param = strtok(params, ",");
    while (param) {
        // Skip whitespace
        while (*param == ' ') param++;
        
        // Try to parse as integer
        char* endptr;
        long int_val = strtol(param, &endptr, 10);
        
        if (*endptr == '\0') {
            // It's an integer
            dep_type_add_param(type, dep_value_int(int_val));
        } else {
            // It's a variable
            dep_type_add_param(type, dep_value_variable(param));
        }
        
        param = strtok(NULL, ",");
    }
    
    return type;
}

// ============================================================================
// CODE GENERATION
// ============================================================================

char* codegen_dep_type_check(DepType* type) {
    if (!type) return NULL;
    
    char* code = malloc(512);
    if (!code) return NULL;
    
    int offset = snprintf(code, 512,
        "    ; Dependent type check for '%s'\n"
        "    ; params=%zu, dependent=%d\n",
        type->base_type,
        type->param_count,
        type->is_dependent
    );
    
    for (size_t i = 0; i < type->param_count && offset < 500; i++) {
        DepValue* val = type->params[i];
        offset += snprintf(code + offset, 512 - offset,
            "    ; param[%zu]: type=%d\n", i, val->type);
    }
    
    return code;
}
