#ifndef TYPE_ALIAS_H
#define TYPE_ALIAS_H

// TIER 1: Type Aliases
// Syntax: type Distance = numeric
//         type UserID = text
//         type Point = struct { x: numeric, y: numeric }

typedef struct TypeAlias {
    char* alias_name;        // New type name (e.g., "Distance")
    char* target_type;       // Target type (e.g., "numeric", "text")
    int is_struct;           // Whether target is a struct definition
    void* struct_def;        // Struct definition if is_struct
    struct TypeAlias* next;  // For linked list
} TypeAlias;

// Create and manage type aliases
TypeAlias* type_alias_create(const char* alias_name, const char* target_type);
void type_alias_set_struct(TypeAlias* alias, void* struct_def);
void type_alias_free(TypeAlias* alias);

// Type alias registry (global)
void type_alias_register(TypeAlias* alias);
TypeAlias* type_alias_lookup(const char* name);
void type_alias_clear_all();

#endif // TYPE_ALIAS_H
