// MELP Modular Codegen - Struct Management
// Struct tablosu yönetimi

#include "codegen.h"

// ============================================================================
// Struct Add
// ============================================================================

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

// ============================================================================
// Struct Lookup
// ============================================================================

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

// ============================================================================
// Field Offset Lookup
// ============================================================================

int codegen_get_field_offset(Codegen* gen, const char* struct_name, const char* field_name) {
    StructInfo* st = codegen_find_struct(gen, struct_name);
    if (!st) {
        fprintf(stderr, "Codegen error: Unknown struct '%s'\n", struct_name);
        exit(1);
    }
    
    for (int i = 0; i < st->field_count; i++) {
        if (strcmp(st->field_names[i], field_name) == 0) {
            return st->field_offsets[i];
        }
    }
    
    fprintf(stderr, "Codegen error: Unknown field '%s' in struct '%s'\n", field_name, struct_name);
    exit(1);
}

// ============================================================================
// Field Type Lookup
// ============================================================================

VarType codegen_get_field_type(Codegen* gen, const char* struct_name, const char* field_name) {
    StructInfo* st = codegen_find_struct(gen, struct_name);
    if (!st) {
        fprintf(stderr, "Codegen error: Unknown struct '%s'\n", struct_name);
        exit(1);
    }
    
    for (int i = 0; i < st->field_count; i++) {
        if (strcmp(st->field_names[i], field_name) == 0) {
            return st->field_types[i];
        }
    }
    
    fprintf(stderr, "Codegen error: Unknown field '%s' in struct '%s'\n", field_name, struct_name);
    exit(1);
}
