/**
 * MODÜL #58: CONST_GENERICS
 * 
 * Rust-style const generics for compile-time type parameters
 * Array<T, N> where N is const, type-level numbers
 * 
 * STANDALONE MODULE - No dependencies on central compiler
 */

#ifndef CONST_GENERICS_H
#define CONST_GENERICS_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

// ============================================================================
// CONST GENERIC TYPES
// ============================================================================

typedef enum {
    CONST_INT,        // const N: i32 = 10
    CONST_USIZE,      // const N: usize = 100
    CONST_BOOL,       // const B: bool = true
    CONST_CHAR,       // const C: char = 'A'
    CONST_EXPR        // const N: usize = M + 1
} ConstKind;

// ============================================================================
// CONST VALUE
// ============================================================================

typedef struct ConstValue {
    ConstKind kind;
    union {
        int64_t int_val;
        uint64_t usize_val;
        bool bool_val;
        char char_val;
    } value;
    char* expr_text;  // For CONST_EXPR
    bool is_evaluated;
} ConstValue;

// ============================================================================
// CONST GENERIC PARAMETER
// ============================================================================

typedef struct ConstGenericParam {
    char* name;              // Parameter name (e.g., "N")
    ConstKind kind;          // Type of const parameter
    ConstValue* default_value; // Optional default value
    bool has_default;
    int line_number;
} ConstGenericParam;

// ============================================================================
// CONST GENERIC TYPE
// ============================================================================

typedef struct ConstGenericType {
    char* base_type;         // Base type name (e.g., "Array")
    char* type_param;        // Type parameter (e.g., "T")
    ConstGenericParam** const_params; // Const parameters
    int const_param_count;
    char* full_name;         // e.g., "Array<T, 10>"
    int line_number;
    struct ConstGenericType* next;
} ConstGenericType;

// ============================================================================
// CONST GENERIC INSTANTIATION
// ============================================================================

typedef struct ConstGenericInstance {
    ConstGenericType* generic_type;
    char* concrete_type;     // e.g., "i32"
    ConstValue** const_args; // Actual const values
    int const_arg_count;
    char* mangled_name;      // e.g., "Array_i32_10"
    bool is_valid;
    struct ConstGenericInstance* next;
} ConstGenericInstance;

// ============================================================================
// CONST GENERIC CONTEXT
// ============================================================================

typedef struct {
    ConstGenericType* types;              // All generic types
    ConstGenericInstance* instances;      // All instantiations
    int type_count;
    int instance_count;
    int const_array_count;                // Arrays with const size
    int const_function_count;             // Functions with const params
} ConstGenericContext;

// ============================================================================
// CONST EXPRESSION EVALUATION
// ============================================================================

typedef enum {
    EXPR_NUMBER,
    EXPR_CONST_REF,  // Reference to another const
    EXPR_ADD,
    EXPR_SUB,
    EXPR_MUL,
    EXPR_DIV,
    EXPR_MOD
} ConstExprType;

typedef struct ConstExpr {
    ConstExprType type;
    union {
        int64_t number;
        char* const_name;
        struct {
            struct ConstExpr* left;
            struct ConstExpr* right;
        } binary;
    } data;
} ConstExpr;

// ============================================================================
// PUBLIC API - Context Management
// ============================================================================

ConstGenericContext* const_generic_context_new(void);
void const_generic_context_free(ConstGenericContext* ctx);

// ============================================================================
// PUBLIC API - Const Values
// ============================================================================

ConstValue* const_value_new_int(int64_t val);
ConstValue* const_value_new_usize(uint64_t val);
ConstValue* const_value_new_bool(bool val);
ConstValue* const_value_new_char(char val);
ConstValue* const_value_new_expr(const char* expr_text);
void const_value_free(ConstValue* val);
bool const_value_equals(ConstValue* a, ConstValue* b);
char* const_value_to_string(ConstValue* val);

// ============================================================================
// PUBLIC API - Const Generic Parameters
// ============================================================================

ConstGenericParam* const_generic_param_new(const char* name, ConstKind kind);
void const_generic_param_set_default(ConstGenericParam* param, ConstValue* val);
void const_generic_param_free(ConstGenericParam* param);

// ============================================================================
// PUBLIC API - Const Generic Types
// ============================================================================

ConstGenericType* const_generic_type_new(const char* base_type, const char* type_param);
void const_generic_type_add_param(ConstGenericType* type, ConstGenericParam* param);
void const_generic_type_register(ConstGenericContext* ctx, ConstGenericType* type);
ConstGenericType* const_generic_type_find(ConstGenericContext* ctx, const char* name);
void const_generic_type_free(ConstGenericType* type);

// ============================================================================
// PUBLIC API - Instantiation
// ============================================================================

ConstGenericInstance* const_generic_instantiate(
    ConstGenericContext* ctx,
    ConstGenericType* generic_type,
    const char* concrete_type,
    ConstValue** const_args,
    int const_arg_count
);

ConstGenericInstance* const_generic_find_instance(
    ConstGenericContext* ctx,
    const char* mangled_name
);

void const_generic_instance_free(ConstGenericInstance* inst);

// ============================================================================
// PUBLIC API - Name Mangling
// ============================================================================

char* const_generic_mangle_name(
    const char* base_type,
    const char* concrete_type,
    ConstValue** const_args,
    int const_arg_count
);

// ============================================================================
// PUBLIC API - Const Expression Evaluation
// ============================================================================

ConstExpr* const_expr_parse(const char* expr_text);
int64_t const_expr_evaluate(ConstExpr* expr, ConstGenericContext* ctx);
void const_expr_free(ConstExpr* expr);

// ============================================================================
// PUBLIC API - Validation
// ============================================================================

bool const_generic_validate_size(uint64_t size);
bool const_generic_validate_instance(ConstGenericInstance* inst);
bool const_generic_can_instantiate(
    ConstGenericType* type,
    ConstValue** const_args,
    int const_arg_count
);

// ============================================================================
// PUBLIC API - Built-in Const Generic Types
// ============================================================================

ConstGenericType* const_generic_builtin_array(void);
ConstGenericType* const_generic_builtin_matrix(void);
ConstGenericType* const_generic_builtin_vector(void);

// ============================================================================
// PUBLIC API - Statistics
// ============================================================================

void const_generic_print_stats(ConstGenericContext* ctx);
void const_generic_dump_types(ConstGenericContext* ctx);
void const_generic_dump_instances(ConstGenericContext* ctx);

// ============================================================================
// DEBUG
// ============================================================================

void const_value_dump(ConstValue* val);
void const_generic_type_dump(ConstGenericType* type);
void const_generic_instance_dump(ConstGenericInstance* inst);

#endif // CONST_GENERICS_H
