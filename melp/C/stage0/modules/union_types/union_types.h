#ifndef UNION_TYPES_H
#define UNION_TYPES_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

// Union variant types
typedef enum {
    UNION_BASIC,        // Basic union (C-style)
    UNION_TAGGED,       // Tagged union (discriminated)
    UNION_ENUM_VARIANT, // Enum with data variants (Rust-style)
    UNION_UNKNOWN
} UnionKind;

// Union operations
typedef enum {
    UNION_OP_DEFINE,      // Define union type
    UNION_OP_CREATE,      // Create union instance
    UNION_OP_MATCH,       // Pattern match on union
    UNION_OP_GET_TAG,     // Get active variant tag
    UNION_OP_IS_VARIANT,  // Check if specific variant
    UNION_OP_UNWRAP,      // Unwrap variant data
    UNION_OP_AS_REF,      // Get reference to variant
    UNION_OP_UNKNOWN
} UnionOp;

// Union variant definition
typedef struct {
    char* name;
    char* type;          // Data type for this variant
    int tag_value;       // Discriminant value
} UnionVariant;

// Union type definition
typedef struct {
    char* name;
    UnionKind kind;
    UnionVariant** variants;
    int variant_count;
    int variant_capacity;
    int tag_size;        // Size of discriminant (1/2/4 bytes)
} UnionTypeDef;

// Union instance
typedef struct {
    char* var_name;
    char* type_name;
    char* active_variant;
    char* value;
} UnionInstance;

// Pattern match case
typedef struct {
    char* pattern;       // Variant pattern
    char* binding;       // Variable binding (if any)
    char* body;          // Match arm body
} MatchCase;

// Union operation node
typedef struct {
    UnionOp op;
    char* union_name;
    char* variant_name;
    char* args[5];
    int arg_count;
    
    // For match operations
    MatchCase** cases;
    int case_count;
} UnionOpNode;

// Union context
typedef struct {
    UnionTypeDef** type_defs;
    int typedef_count;
    int typedef_capacity;
    
    UnionInstance** instances;
    int instance_count;
    int instance_capacity;
    
    UnionOpNode** operations;
    int op_count;
    int op_capacity;
    
    char* error_message;
} UnionContext;

// Lifecycle functions
UnionContext* union_context_create(void);
void union_context_free(UnionContext* ctx);

// Parser functions
bool union_parse_typedef(UnionContext* ctx, const char* input);
bool union_parse_instance(UnionContext* ctx, const char* input);
bool union_parse_operation(UnionContext* ctx, const char* input);
UnionKind union_get_kind(const char* kind_str);
UnionOp union_get_operation(const char* op_str);

// Codegen functions
void union_codegen_typedef(FILE* out, UnionTypeDef* def);
void union_codegen_instance(FILE* out, UnionInstance* inst);
void union_codegen_operation(FILE* out, UnionOpNode* op);
void union_codegen_full(UnionContext* ctx, const char* output_file);

// Utility functions
void union_add_typedef(UnionContext* ctx, UnionTypeDef* def);
void union_add_instance(UnionContext* ctx, UnionInstance* inst);
void union_add_operation(UnionContext* ctx, UnionOpNode* op);
UnionTypeDef* union_find_typedef(UnionContext* ctx, const char* name);
UnionInstance* union_find_instance(UnionContext* ctx, const char* name);

#endif
