#include "operator_overloading.h"
#include <stdlib.h>
#include <string.h>

// Operator overload oluştur
OperatorOverload* operator_overload_create(OperatorType op, const char* type_name) {
    OperatorOverload* overload = malloc(sizeof(OperatorOverload));
    overload->op = op;
    overload->type_name = strdup(type_name);
    overload->implementation = NULL;
    overload->is_binary = operator_is_binary(op);
    overload->param_types[0] = NULL;
    overload->param_types[1] = NULL;
    overload->return_type = NULL;
    return overload;
}

// Registry oluştur
OperatorRegistry* operator_registry_create(void) {
    OperatorRegistry* registry = malloc(sizeof(OperatorRegistry));
    registry->capacity = 32;
    registry->overloads = malloc(sizeof(OperatorOverload*) * registry->capacity);
    registry->count = 0;
    return registry;
}

// Dispatch oluştur
OperatorDispatch* operator_dispatch_create(OperatorType op) {
    OperatorDispatch* dispatch = malloc(sizeof(OperatorDispatch));
    dispatch->op = op;
    dispatch->left_operand = NULL;
    dispatch->right_operand = NULL;
    dispatch->left_type = NULL;
    dispatch->right_type = NULL;
    dispatch->result = NULL;
    return dispatch;
}

// Custom operator oluştur
CustomOperator* custom_operator_create(const char* symbol, int precedence) {
    CustomOperator* custom = malloc(sizeof(CustomOperator));
    custom->symbol = strdup(symbol);
    custom->precedence = precedence;
    custom->is_left_assoc = 1;
    custom->implementation = NULL;
    return custom;
}

// Overload parametrelerini ayarla
void operator_overload_set_params(OperatorOverload* overload, 
                                  const char* param1, const char* param2) {
    if (!overload) return;
    
    if (param1) overload->param_types[0] = strdup(param1);
    if (param2 && overload->is_binary) {
        overload->param_types[1] = strdup(param2);
    }
}

// Overload return type ayarla
void operator_overload_set_return(OperatorOverload* overload, 
                                  const char* return_type) {
    if (!overload) return;
    overload->return_type = strdup(return_type);
}

// Overload implementation ayarla
void operator_overload_set_impl(OperatorOverload* overload, void* impl) {
    if (!overload) return;
    overload->implementation = impl;
}

// Registry'ye overload ekle
void registry_add_overload(OperatorRegistry* registry, OperatorOverload* overload) {
    if (!registry || !overload) return;
    
    if (registry->count >= registry->capacity) {
        registry->capacity *= 2;
        registry->overloads = realloc(registry->overloads,
                                     sizeof(OperatorOverload*) * registry->capacity);
    }
    
    registry->overloads[registry->count] = overload;
    registry->count++;
}

// Registry'den overload bul
OperatorOverload* registry_find_overload(OperatorRegistry* registry, 
                                         OperatorType op, 
                                         const char* left_type,
                                         const char* right_type) {
    if (!registry) return NULL;
    
    for (int i = 0; i < registry->count; i++) {
        OperatorOverload* overload = registry->overloads[i];
        
        if (overload->op != op) continue;
        
        // Type matching
        if (left_type && overload->param_types[0]) {
            if (strcmp(left_type, overload->param_types[0]) != 0) continue;
        }
        
        if (overload->is_binary && right_type && overload->param_types[1]) {
            if (strcmp(right_type, overload->param_types[1]) != 0) continue;
        }
        
        return overload;
    }
    
    return NULL;
}

// Operator dispatch
void* dispatch_operator(OperatorDispatch* dispatch, OperatorRegistry* registry) {
    if (!dispatch || !registry) return NULL;
    
    // Overload bul
    OperatorOverload* overload = registry_find_overload(registry,
                                                        dispatch->op,
                                                        dispatch->left_type,
                                                        dispatch->right_type);
    
    if (!overload || !overload->implementation) return NULL;
    
    // Implementation çağır (şimdilik NULL dön)
    // Gerçek implementasyonda function pointer call
    
    return NULL;
}

// Dispatch edilebilir mi?
int can_dispatch(OperatorDispatch* dispatch, OperatorRegistry* registry) {
    if (!dispatch || !registry) return 0;
    
    OperatorOverload* overload = registry_find_overload(registry,
                                                        dispatch->op,
                                                        dispatch->left_type,
                                                        dispatch->right_type);
    
    return overload != NULL;
}

// Custom operator implementation ayarla
void custom_operator_set_impl(CustomOperator* custom, void* impl) {
    if (!custom) return;
    custom->implementation = impl;
}

// Operator type to string
const char* operator_type_to_string(OperatorType op) {
    switch (op) {
        case OP_ADD: return "+";
        case OP_SUB: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
        case OP_MOD: return "%";
        case OP_EQ: return "==";
        case OP_NE: return "!=";
        case OP_LT: return "<";
        case OP_LE: return "<=";
        case OP_GT: return ">";
        case OP_GE: return ">=";
        case OP_INDEX: return "[]";
        case OP_CALL: return "()";
        case OP_ASSIGN: return "=";
        default: return "?";
    }
}

// Binary operator mı?
int operator_is_binary(OperatorType op) {
    switch (op) {
        case OP_UNARY_MINUS:
        case OP_UNARY_PLUS:
        case OP_NOT:
        case OP_BIT_NOT:
        case OP_INCR:
        case OP_DECR:
            return 0;
        default:
            return 1;
    }
}

// Bellek temizleme
void operator_overload_free(OperatorOverload* overload) {
    if (!overload) return;
    
    free(overload->type_name);
    if (overload->param_types[0]) free(overload->param_types[0]);
    if (overload->param_types[1]) free(overload->param_types[1]);
    if (overload->return_type) free(overload->return_type);
    
    free(overload);
}

void operator_registry_free(OperatorRegistry* registry) {
    if (!registry) return;
    
    for (int i = 0; i < registry->count; i++) {
        operator_overload_free(registry->overloads[i]);
    }
    
    free(registry->overloads);
    free(registry);
}

void operator_dispatch_free(OperatorDispatch* dispatch) {
    if (!dispatch) return;
    free(dispatch);
}

void custom_operator_free(CustomOperator* custom) {
    if (!custom) return;
    free(custom->symbol);
    free(custom);
}
