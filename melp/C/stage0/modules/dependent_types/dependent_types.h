#ifndef DEPENDENT_TYPES_H
#define DEPENDENT_TYPES_H

#include <stddef.h>
#include <stdbool.h>

// ============================================================================
// MLP DEPENDENT TYPE SYSTEM - MODULE #62
// ============================================================================
// Dependent types: types that depend on values
// Examples: Vec<n> where n is a runtime value
//           Matrix<rows, cols> where dimensions are values
// Based on dependent type theory (Idris, Agda, Coq)

typedef enum {
    DEP_VALUE_INT,
    DEP_VALUE_STRING,
    DEP_VALUE_BOOL,
    DEP_VALUE_VARIABLE
} DepValueType;

typedef struct DepValue {
    DepValueType type;
    union {
        long int_val;
        char* str_val;
        bool bool_val;
        char* var_name;
    } data;
} DepValue;

typedef struct DepType {
    char* base_type;              // "Vec", "Matrix", "Array"
    DepValue** params;            // Value parameters
    size_t param_count;
    bool is_dependent;            // true if has value params
} DepType;

typedef struct DepTypeContext {
    DepType** types;
    size_t type_count;
    size_t type_capacity;
    size_t total_checks;
    size_t violations;
} DepTypeContext;

// Context management
DepTypeContext* dep_context_create(void);
void dep_context_destroy(DepTypeContext* ctx);

// Type creation
DepType* dep_type_create(const char* base_type);
void dep_type_add_param(DepType* type, DepValue* value);
void dep_type_free(DepType* type);

// Value creation
DepValue* dep_value_int(long val);
DepValue* dep_value_string(const char* val);
DepValue* dep_value_bool(bool val);
DepValue* dep_value_variable(const char* name);
void dep_value_free(DepValue* val);

// Type checking
bool dep_type_check(DepTypeContext* ctx, DepType* type1, DepType* type2);
bool dep_type_compatible(DepType* type1, DepType* type2);

// Registration
void dep_register_type(DepTypeContext* ctx, DepType* type);

// Parser
DepType* dep_parse_type(const char* type_str);

// Codegen
char* codegen_dep_type_check(DepType* type);

#endif
