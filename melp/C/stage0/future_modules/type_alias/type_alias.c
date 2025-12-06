#include "type_alias.h"
#include <stdlib.h>
#include <string.h>

// Global type alias registry
static TypeAlias* global_aliases = NULL;

// Create new type alias
TypeAlias* type_alias_create(const char* alias_name, const char* target_type) {
    TypeAlias* alias = (TypeAlias*)malloc(sizeof(TypeAlias));
    alias->alias_name = strdup(alias_name);
    alias->target_type = strdup(target_type);
    alias->is_struct = 0;
    alias->struct_def = NULL;
    alias->next = NULL;
    return alias;
}

// Set struct definition for alias
void type_alias_set_struct(TypeAlias* alias, void* struct_def) {
    if (alias) {
        alias->is_struct = 1;
        alias->struct_def = struct_def;
    }
}

// Free type alias
void type_alias_free(TypeAlias* alias) {
    if (!alias) return;
    free(alias->alias_name);
    free(alias->target_type);
    // Note: struct_def is owned by struct module, don't free here
    free(alias);
}

// Register type alias globally
void type_alias_register(TypeAlias* alias) {
    if (!alias) return;
    alias->next = global_aliases;
    global_aliases = alias;
}

// Lookup type alias by name
TypeAlias* type_alias_lookup(const char* name) {
    TypeAlias* current = global_aliases;
    while (current) {
        if (strcmp(current->alias_name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Clear all type aliases
void type_alias_clear_all() {
    TypeAlias* current = global_aliases;
    while (current) {
        TypeAlias* next = current->next;
        type_alias_free(current);
        current = next;
    }
    global_aliases = NULL;
}
