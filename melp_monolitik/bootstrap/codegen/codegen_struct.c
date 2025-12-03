// ============================================================================
// MELP Modular Codegen - codegen_struct.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// İçerik: codegen_add_struct, codegen_find_struct
// Struct bilgilerini yönetir

void codegen_add_struct(Codegen* gen, const char* name, int field_count, 
                       VarType* field_types, char** field_names,
                       char** implements, int implements_count) {
    StructInfo* st = malloc(sizeof(StructInfo));
    st->name = malloc(strlen(name) + 1);
    strcpy(st->name, name);
    st->field_count = field_count;
    
    // Allocate and copy field info
    st->field_types = malloc(sizeof(VarType) * field_count);
    st->field_names = malloc(sizeof(char*) * field_count);
    st->field_offsets = malloc(sizeof(int) * field_count);
    
    int offset = 0;
    for (int i = 0; i < field_count; i++) {
        st->field_types[i] = field_types[i];
        st->field_names[i] = malloc(strlen(field_names[i]) + 1);
        strcpy(st->field_names[i], field_names[i]);
        
        // Calculate offset (all types are 8 bytes for now)
        st->field_offsets[i] = offset;
        offset += 8;
    }
    
    st->total_size = offset;
    
    // Copy implements info
    if (implements && implements_count > 0) {
        st->implements = malloc(sizeof(char*) * implements_count);
        st->implements_count = implements_count;
        for (int i = 0; i < implements_count; i++) {
            st->implements[i] = malloc(strlen(implements[i]) + 1);
            strcpy(st->implements[i], implements[i]);
        }
    } else {
        st->implements = NULL;
        st->implements_count = 0;
    }
    
    st->next = gen->structs;
    gen->structs = st;
}

StructInfo* codegen_find_struct(Codegen* gen, const char* name) {
    StructInfo* current = gen->structs;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

