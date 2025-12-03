// ============================================================================
// MELP Modular Codegen - codegen_interface.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// İçerik: codegen_add_interface, codegen_find_interface
// Interface bilgilerini yönetir

void codegen_add_interface(Codegen* gen, const char* name, MethodSignature** methods, int method_count) {
    InterfaceInfo* iface = malloc(sizeof(InterfaceInfo));
    iface->name = malloc(strlen(name) + 1);
    strcpy(iface->name, name);
    iface->method_count = method_count;
    iface->methods = malloc(sizeof(MethodSignature*) * method_count);
    
    for (int i = 0; i < method_count; i++) {
        iface->methods[i] = methods[i];
    }
    
    iface->next = gen->interfaces;
    gen->interfaces = iface;
}

InterfaceInfo* codegen_find_interface(Codegen* gen, const char* name) {
    InterfaceInfo* current = gen->interfaces;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

