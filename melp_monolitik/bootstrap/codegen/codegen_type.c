// ============================================================================
// MELP Modular Codegen - codegen_type.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// Enum values ve Type alias yönetimi

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

void codegen_add_type_alias(Codegen* gen, const char* alias_name, int base_type, const char* struct_name) {
    TypeAlias* alias = malloc(sizeof(TypeAlias));
    alias->alias_name = malloc(strlen(alias_name) + 1);
    strcpy(alias->alias_name, alias_name);
    alias->base_type = base_type;
    
    if (struct_name) {
        alias->struct_name = malloc(strlen(struct_name) + 1);
        strcpy(alias->struct_name, struct_name);
    } else {
        alias->struct_name = NULL;
    }
    
    alias->next = gen->type_aliases;
    gen->type_aliases = alias;
}

TypeAlias* codegen_find_type_alias(Codegen* gen, const char* alias_name) {
    TypeAlias* current = gen->type_aliases;
    while (current) {
        if (strcmp(current->alias_name, alias_name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL; // Not found, might be a regular type
}

