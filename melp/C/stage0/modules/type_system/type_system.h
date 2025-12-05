#ifndef TYPE_SYSTEM_H
#define TYPE_SYSTEM_H

#include "../../lexer.h"
#include <stdbool.h>

// Base type kinds
typedef enum {
    TYPE_INT,           // int
    TYPE_FLOAT,         // float
    TYPE_STRING,        // string
    TYPE_BOOL,          // bool
    TYPE_VOID,          // void
    TYPE_CHAR,          // char
    TYPE_ARRAY,         // array[T]
    TYPE_POINTER,       // *T
    TYPE_FUNCTION,      // func(T1, T2) -> T3
    TYPE_STRUCT,        // struct {...}
    TYPE_ENUM,          // enum {...}
    TYPE_GENERIC,       // T (generic type parameter)
    TYPE_UNKNOWN,       // Not yet inferred
    TYPE_ERROR          // Type error
} TypeKind;

// Type structure
typedef struct Type {
    TypeKind kind;
    char* name;                     // Type name (for named types)

    // Array type info
    struct Type* element_type;      // Array element type or pointer target type
    int array_size;                 // -1 for dynamic arrays

    // Function type info
    struct Type* return_type;       // Function return type
    struct Type** param_types;      // Function parameter types
    int param_count;

    // Struct/Enum member info
    struct Type** member_types;     // Struct member types
    char** member_names;            // Struct member names
    int member_count;

    // Generic constraints
    char** constraints;             // Type constraints (for generics)
    int constraint_count;

    // Metadata
    bool is_mutable;                // Is this type mutable?
    bool is_nullable;               // Can this be null?
} Type;

// Symbol table entry
typedef struct Symbol {
    char* name;
    Type* type;
    bool is_defined;                // Has value been assigned?
    struct Symbol* next;            // For hash table chaining
} Symbol;

// Symbol table (scope)
typedef struct SymbolTable {
    Symbol** symbols;               // Hash table of symbols
    int capacity;
    struct SymbolTable* parent;     // Parent scope
} SymbolTable;

// Type system context
typedef struct TypeContext {
    SymbolTable* global_scope;
    SymbolTable* current_scope;
    Type** type_definitions;        // User-defined types
    int type_count;
} TypeContext;

// Type system functions
TypeContext* type_context_create();
void type_context_destroy(TypeContext* ctx);

Type* type_create(TypeKind kind);
void type_free(Type* type);
bool type_equals(Type* t1, Type* t2);
char* type_to_string(Type* type);

SymbolTable* symbol_table_create(SymbolTable* parent);
void symbol_table_destroy(SymbolTable* table);
void symbol_table_insert(SymbolTable* table, const char* name, Type* type);
Symbol* symbol_table_lookup(SymbolTable* table, const char* name);

// Type checking
bool type_check_assignment(Type* lhs, Type* rhs);
Type* type_infer_expression(TypeContext* ctx, void* expr);
Type* type_check_binary_op(Type* left, Type* right, const char* op);

#endif
