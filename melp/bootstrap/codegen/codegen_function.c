// MELP Modular Codegen - Function Management
// Fonksiyon tablosu yönetimi

#include "codegen.h"

// ============================================================================
// Function Table Add
// ============================================================================

void codegen_add_function(Codegen* gen, const char* name, int param_count, VarType* param_types) {
    FuncInfo* func = malloc(sizeof(FuncInfo));
    func->name = malloc(strlen(name) + 1);
    strcpy(func->name, name);
    func->param_count = param_count;
    func->param_types = malloc(sizeof(VarType) * param_count);
    memcpy(func->param_types, param_types, sizeof(VarType) * param_count);
    func->next = gen->functions;
    gen->functions = func;
}

// ============================================================================
// Function Table Lookup
// ============================================================================

FuncInfo* codegen_find_function(Codegen* gen, const char* name) {
    FuncInfo* current = gen->functions;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
