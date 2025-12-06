/**
 * MODÜL #58: CONST_GENERICS - Core Implementation
 * 
 * Const generic engine, type instantiation, compile-time evaluation
 */

#define _GNU_SOURCE
#include "const_generics.h"

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

ConstGenericContext* const_generic_context_new(void) {
    ConstGenericContext* ctx = malloc(sizeof(ConstGenericContext));
    ctx->types = NULL;
    ctx->instances = NULL;
    ctx->type_count = 0;
    ctx->instance_count = 0;
    ctx->const_array_count = 0;
    ctx->const_function_count = 0;
    return ctx;
}

void const_generic_context_free(ConstGenericContext* ctx) {
    // Free types
    ConstGenericType* type = ctx->types;
    while (type) {
        ConstGenericType* next = type->next;
        const_generic_type_free(type);
        type = next;
    }
    
    // Free instances
    ConstGenericInstance* inst = ctx->instances;
    while (inst) {
        ConstGenericInstance* next = inst->next;
        const_generic_instance_free(inst);
        inst = next;
    }
    
    free(ctx);
}

// ============================================================================
// CONST VALUES
// ============================================================================

ConstValue* const_value_new_int(int64_t val) {
    ConstValue* cv = malloc(sizeof(ConstValue));
    cv->kind = CONST_INT;
    cv->value.int_val = val;
    cv->expr_text = NULL;
    cv->is_evaluated = true;
    return cv;
}

ConstValue* const_value_new_usize(uint64_t val) {
    ConstValue* cv = malloc(sizeof(ConstValue));
    cv->kind = CONST_USIZE;
    cv->value.usize_val = val;
    cv->expr_text = NULL;
    cv->is_evaluated = true;
    return cv;
}

ConstValue* const_value_new_bool(bool val) {
    ConstValue* cv = malloc(sizeof(ConstValue));
    cv->kind = CONST_BOOL;
    cv->value.bool_val = val;
    cv->expr_text = NULL;
    cv->is_evaluated = true;
    return cv;
}

ConstValue* const_value_new_char(char val) {
    ConstValue* cv = malloc(sizeof(ConstValue));
    cv->kind = CONST_CHAR;
    cv->value.char_val = val;
    cv->expr_text = NULL;
    cv->is_evaluated = true;
    return cv;
}

ConstValue* const_value_new_expr(const char* expr_text) {
    ConstValue* cv = malloc(sizeof(ConstValue));
    cv->kind = CONST_EXPR;
    cv->expr_text = strdup(expr_text);
    cv->is_evaluated = false;
    return cv;
}

void const_value_free(ConstValue* val) {
    if (val->expr_text) {
        free(val->expr_text);
    }
    free(val);
}

bool const_value_equals(ConstValue* a, ConstValue* b) {
    if (a->kind != b->kind) return false;
    
    switch (a->kind) {
        case CONST_INT:
            return a->value.int_val == b->value.int_val;
        case CONST_USIZE:
            return a->value.usize_val == b->value.usize_val;
        case CONST_BOOL:
            return a->value.bool_val == b->value.bool_val;
        case CONST_CHAR:
            return a->value.char_val == b->value.char_val;
        case CONST_EXPR:
            return strcmp(a->expr_text, b->expr_text) == 0;
        default:
            return false;
    }
}

char* const_value_to_string(ConstValue* val) {
    char* str = malloc(128);
    
    switch (val->kind) {
        case CONST_INT:
            snprintf(str, 128, "%ld", val->value.int_val);
            break;
        case CONST_USIZE:
            snprintf(str, 128, "%lu", val->value.usize_val);
            break;
        case CONST_BOOL:
            snprintf(str, 128, "%s", val->value.bool_val ? "true" : "false");
            break;
        case CONST_CHAR:
            snprintf(str, 128, "'%c'", val->value.char_val);
            break;
        case CONST_EXPR:
            snprintf(str, 128, "%s", val->expr_text);
            break;
        default:
            snprintf(str, 128, "<unknown>");
            break;
    }
    
    return str;
}

// ============================================================================
// CONST GENERIC PARAMETERS
// ============================================================================

ConstGenericParam* const_generic_param_new(const char* name, ConstKind kind) {
    ConstGenericParam* param = malloc(sizeof(ConstGenericParam));
    param->name = strdup(name);
    param->kind = kind;
    param->default_value = NULL;
    param->has_default = false;
    param->line_number = 0;
    return param;
}

void const_generic_param_set_default(ConstGenericParam* param, ConstValue* val) {
    param->default_value = val;
    param->has_default = true;
}

void const_generic_param_free(ConstGenericParam* param) {
    free(param->name);
    if (param->default_value) {
        const_value_free(param->default_value);
    }
    free(param);
}

// ============================================================================
// CONST GENERIC TYPES
// ============================================================================

ConstGenericType* const_generic_type_new(const char* base_type, const char* type_param) {
    ConstGenericType* type = malloc(sizeof(ConstGenericType));
    type->base_type = strdup(base_type);
    type->type_param = type_param ? strdup(type_param) : NULL;
    type->const_params = NULL;
    type->const_param_count = 0;
    type->full_name = NULL;
    type->line_number = 0;
    type->next = NULL;
    return type;
}

void const_generic_type_add_param(ConstGenericType* type, ConstGenericParam* param) {
    type->const_param_count++;
    type->const_params = realloc(type->const_params, 
                                 sizeof(ConstGenericParam*) * type->const_param_count);
    type->const_params[type->const_param_count - 1] = param;
}

void const_generic_type_register(ConstGenericContext* ctx, ConstGenericType* type) {
    type->next = ctx->types;
    ctx->types = type;
    ctx->type_count++;
}

ConstGenericType* const_generic_type_find(ConstGenericContext* ctx, const char* name) {
    ConstGenericType* current = ctx->types;
    while (current) {
        if (strcmp(current->base_type, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void const_generic_type_free(ConstGenericType* type) {
    free(type->base_type);
    if (type->type_param) free(type->type_param);
    if (type->full_name) free(type->full_name);
    
    for (int i = 0; i < type->const_param_count; i++) {
        const_generic_param_free(type->const_params[i]);
    }
    free(type->const_params);
    free(type);
}

// ============================================================================
// INSTANTIATION
// ============================================================================

ConstGenericInstance* const_generic_instantiate(
    ConstGenericContext* ctx,
    ConstGenericType* generic_type,
    const char* concrete_type,
    ConstValue** const_args,
    int const_arg_count
) {
    // Validate argument count
    if (const_arg_count != generic_type->const_param_count) {
        return NULL;
    }
    
    ConstGenericInstance* inst = malloc(sizeof(ConstGenericInstance));
    inst->generic_type = generic_type;
    inst->concrete_type = strdup(concrete_type);
    inst->const_args = malloc(sizeof(ConstValue*) * const_arg_count);
    inst->const_arg_count = const_arg_count;
    
    // Copy const arguments
    for (int i = 0; i < const_arg_count; i++) {
        inst->const_args[i] = const_args[i];
    }
    
    // Generate mangled name
    inst->mangled_name = const_generic_mangle_name(
        generic_type->base_type,
        concrete_type,
        const_args,
        const_arg_count
    );
    
    inst->is_valid = const_generic_validate_instance(inst);
    inst->next = NULL;
    
    // Add to context
    inst->next = ctx->instances;
    ctx->instances = inst;
    ctx->instance_count++;
    
    // Update counts
    if (strcmp(generic_type->base_type, "Array") == 0) {
        ctx->const_array_count++;
    }
    
    return inst;
}

ConstGenericInstance* const_generic_find_instance(
    ConstGenericContext* ctx,
    const char* mangled_name
) {
    ConstGenericInstance* current = ctx->instances;
    while (current) {
        if (strcmp(current->mangled_name, mangled_name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void const_generic_instance_free(ConstGenericInstance* inst) {
    free(inst->concrete_type);
    free(inst->mangled_name);
    free(inst->const_args);
    free(inst);
}

// ============================================================================
// NAME MANGLING
// ============================================================================

char* const_generic_mangle_name(
    const char* base_type,
    const char* concrete_type,
    ConstValue** const_args,
    int const_arg_count
) {
    char* mangled = malloc(256);
    int offset = snprintf(mangled, 256, "%s_%s", base_type, concrete_type);
    
    for (int i = 0; i < const_arg_count; i++) {
        char* val_str = const_value_to_string(const_args[i]);
        offset += snprintf(mangled + offset, 256 - offset, "_%s", val_str);
        free(val_str);
    }
    
    return mangled;
}

// ============================================================================
// CONST EXPRESSION EVALUATION
// ============================================================================

ConstExpr* const_expr_parse(const char* expr_text) {
    ConstExpr* expr = malloc(sizeof(ConstExpr));
    
    // Simple parser for numbers and basic operations
    char* endptr;
    long val = strtol(expr_text, &endptr, 10);
    
    if (*endptr == '\0') {
        // Pure number
        expr->type = EXPR_NUMBER;
        expr->data.number = val;
    } else {
        // For now, treat as const reference
        expr->type = EXPR_CONST_REF;
        expr->data.const_name = strdup(expr_text);
    }
    
    return expr;
}

int64_t const_expr_evaluate(ConstExpr* expr, ConstGenericContext* ctx) {
    (void)ctx; // Unused for now
    
    switch (expr->type) {
        case EXPR_NUMBER:
            return expr->data.number;
        case EXPR_CONST_REF:
            // Would look up in context
            return 0;
        case EXPR_ADD:
            return const_expr_evaluate(expr->data.binary.left, ctx) +
                   const_expr_evaluate(expr->data.binary.right, ctx);
        case EXPR_SUB:
            return const_expr_evaluate(expr->data.binary.left, ctx) -
                   const_expr_evaluate(expr->data.binary.right, ctx);
        case EXPR_MUL:
            return const_expr_evaluate(expr->data.binary.left, ctx) *
                   const_expr_evaluate(expr->data.binary.right, ctx);
        case EXPR_DIV:
            return const_expr_evaluate(expr->data.binary.left, ctx) /
                   const_expr_evaluate(expr->data.binary.right, ctx);
        case EXPR_MOD:
            return const_expr_evaluate(expr->data.binary.left, ctx) %
                   const_expr_evaluate(expr->data.binary.right, ctx);
        default:
            return 0;
    }
}

void const_expr_free(ConstExpr* expr) {
    if (expr->type == EXPR_CONST_REF) {
        free(expr->data.const_name);
    } else if (expr->type >= EXPR_ADD && expr->type <= EXPR_MOD) {
        const_expr_free(expr->data.binary.left);
        const_expr_free(expr->data.binary.right);
    }
    free(expr);
}

// ============================================================================
// VALIDATION
// ============================================================================

bool const_generic_validate_size(uint64_t size) {
    // Max array size: 1GB
    const uint64_t MAX_SIZE = 1024 * 1024 * 1024;
    return size > 0 && size <= MAX_SIZE;
}

bool const_generic_validate_instance(ConstGenericInstance* inst) {
    // Validate all const arguments
    for (int i = 0; i < inst->const_arg_count; i++) {
        ConstValue* val = inst->const_args[i];
        if (val->kind == CONST_USIZE) {
            if (!const_generic_validate_size(val->value.usize_val)) {
                return false;
            }
        }
    }
    return true;
}

bool const_generic_can_instantiate(
    ConstGenericType* type,
    ConstValue** const_args,
    int const_arg_count
) {
    return const_arg_count == type->const_param_count;
}

// ============================================================================
// BUILT-IN CONST GENERIC TYPES
// ============================================================================

ConstGenericType* const_generic_builtin_array(void) {
    ConstGenericType* array = const_generic_type_new("Array", "T");
    ConstGenericParam* size = const_generic_param_new("N", CONST_USIZE);
    const_generic_type_add_param(array, size);
    return array;
}

ConstGenericType* const_generic_builtin_matrix(void) {
    ConstGenericType* matrix = const_generic_type_new("Matrix", "T");
    ConstGenericParam* rows = const_generic_param_new("ROWS", CONST_USIZE);
    ConstGenericParam* cols = const_generic_param_new("COLS", CONST_USIZE);
    const_generic_type_add_param(matrix, rows);
    const_generic_type_add_param(matrix, cols);
    return matrix;
}

ConstGenericType* const_generic_builtin_vector(void) {
    ConstGenericType* vector = const_generic_type_new("Vector", "T");
    ConstGenericParam* dim = const_generic_param_new("DIM", CONST_USIZE);
    const_generic_type_add_param(vector, dim);
    return vector;
}

// ============================================================================
// STATISTICS
// ============================================================================

void const_generic_print_stats(ConstGenericContext* ctx) {
    printf("\n=== CONST GENERICS STATISTICS ===\n");
    printf("Total generic types: %d\n", ctx->type_count);
    printf("Total instances: %d\n", ctx->instance_count);
    printf("Const arrays: %d\n", ctx->const_array_count);
    printf("Const functions: %d\n", ctx->const_function_count);
}

void const_generic_dump_types(ConstGenericContext* ctx) {
    printf("\n=== CONST GENERIC TYPES ===\n");
    ConstGenericType* current = ctx->types;
    while (current) {
        const_generic_type_dump(current);
        current = current->next;
    }
}

void const_generic_dump_instances(ConstGenericContext* ctx) {
    printf("\n=== CONST GENERIC INSTANCES ===\n");
    ConstGenericInstance* current = ctx->instances;
    while (current) {
        const_generic_instance_dump(current);
        current = current->next;
    }
}

// ============================================================================
// DEBUG
// ============================================================================

void const_value_dump(ConstValue* val) {
    char* str = const_value_to_string(val);
    printf("  Value: %s", str);
    if (val->kind == CONST_EXPR) {
        printf(" (expression)");
    }
    printf("\n");
    free(str);
}

void const_generic_type_dump(ConstGenericType* type) {
    printf("Generic Type: %s", type->base_type);
    if (type->type_param) {
        printf("<%s", type->type_param);
        for (int i = 0; i < type->const_param_count; i++) {
            printf(", const %s", type->const_params[i]->name);
        }
        printf(">");
    }
    printf("\n");
}

void const_generic_instance_dump(ConstGenericInstance* inst) {
    printf("Instance: %s (%s)\n", inst->mangled_name, inst->is_valid ? "valid" : "invalid");
    printf("  Type: %s<%s", inst->generic_type->base_type, inst->concrete_type);
    for (int i = 0; i < inst->const_arg_count; i++) {
        char* val_str = const_value_to_string(inst->const_args[i]);
        printf(", %s", val_str);
        free(val_str);
    }
    printf(">\n");
}
