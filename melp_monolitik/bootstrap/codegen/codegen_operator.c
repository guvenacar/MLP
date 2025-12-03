// ============================================================================
// MELP Modular Codegen - codegen_operator.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// Operator overloading registry

// Phase 16: Register an operator overload
void codegen_add_operator_overload(Codegen* gen, const char* op, const char* type_name, const char* func_name) {
    OperatorOverload* oo = malloc(sizeof(OperatorOverload));
    oo->op = malloc(strlen(op) + 1);
    strcpy(oo->op, op);
    oo->type_name = malloc(strlen(type_name) + 1);
    strcpy(oo->type_name, type_name);
    oo->func_name = malloc(strlen(func_name) + 1);
    strcpy(oo->func_name, func_name);
    oo->next = gen->operator_overloads;
    gen->operator_overloads = oo;
}

// Phase 16: Find an operator overload for a type
OperatorOverload* codegen_find_operator_overload(Codegen* gen, const char* op, const char* type_name) {
    OperatorOverload* oo = gen->operator_overloads;
    while (oo) {
        if (strcmp(oo->op, op) == 0 && strcmp(oo->type_name, type_name) == 0) {
            return oo;
        }
        oo = oo->next;
    }
    return NULL;
}
