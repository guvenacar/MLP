// MELP Modular Codegen - Enum Management
// Enum tablosu yönetimi

#include "codegen.h"

// ============================================================================
// Enum Value Add
// ============================================================================

void codegen_add_enum_value(Codegen* gen, const char* enum_name, const char* member_name, long value) {
    EnumValue* ev = malloc(sizeof(EnumValue));
    ev->enum_name = malloc(strlen(enum_name) + 1);
    strcpy(ev->enum_name, enum_name);
    ev->member_name = malloc(strlen(member_name) + 1);
    strcpy(ev->member_name, member_name);
    ev->value = value;
    ev->next = gen->enums;
    gen->enums = ev;
}

// ============================================================================
// Enum Value Lookup
// ============================================================================

long codegen_find_enum_value(Codegen* gen, const char* enum_name, const char* member_name) {
    EnumValue* current = gen->enums;
    while (current) {
        if (strcmp(current->enum_name, enum_name) == 0 &&
            strcmp(current->member_name, member_name) == 0) {
            return current->value;
        }
        current = current->next;
    }
    fprintf(stderr, "Codegen error: Undefined enum value '%s.%s'\n", enum_name, member_name);
    exit(1);
}
