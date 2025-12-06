#ifndef VARIABLE_H
#define VARIABLE_H

#include <stdbool.h>
#include "../../lexer.h"
#include "../codegen_context/tto_types.h"

typedef enum {
    VAR_NUMERIC,
    VAR_STRING,
    VAR_BOOLEAN,
    VAR_POINTER,     // Pointer type (numeric*, text*)
    VAR_ARRAY        // Array type (numeric[], text[])
} VarType;

// Internal numeric representation (transparent to user)
typedef enum {
    INTERNAL_INT64,      // Small integers: -2^63 to 2^63-1
    INTERNAL_DOUBLE,     // Floating point: ~15 digits precision
    INTERNAL_BIGDECIMAL  // Arbitrary precision (future)
} InternalNumericType;

// Internal string representation (transparent to user)
typedef enum {
    INTERNAL_SSO,        // Small String Optimization: ≤23 bytes inline
    INTERNAL_HEAP,       // Heap allocated: >23 bytes
    INTERNAL_RODATA      // Read-only data section: string literals
} InternalStringType;

// Storage location for variables
typedef enum {
    STORAGE_REGISTER,    // Temporary values in registers
    STORAGE_STACK,       // Local variables on stack
    STORAGE_DATA,        // Global/state variables in .data
    STORAGE_BSS,         // Uninitialized globals in .bss
    STORAGE_HEAP         // Dynamic allocation via malloc
} StorageLocation;

// Variable declaration structure
typedef struct {
    char* name;                          // Variable name
    VarType type;                        // Variable type (numeric, text, boolean, pointer, array)
    char* value;                         // Initial value (string representation)
    InternalNumericType internal_num_type;  // For TTO optimization (DEPRECATED - use tto_info)
    InternalStringType internal_str_type;   // For TTO optimization (DEPRECATED - use tto_info)
    StorageLocation storage;             // Where variable is stored
    int has_decimal_point;               // Flag for numeric type detection
    void* init_expr;                     // Expression* for complex initialization
    
    // Pointer/Array specific fields
    VarType base_type;                   // Base type for pointer/array (e.g., VAR_NUMERIC for numeric*)
    int is_pointer;                      // 1 if pointer type
    int is_array;                        // 1 if array type
    int array_size;                      // Array size (0 = dynamic)
    
    // ========== Phase 2: TTO Integration ==========
    TTOTypeInfo* tto_info;               // Full TTO analysis result (heap allocated)
    bool tto_analyzed;                   // Has TTO analysis been performed?
    bool needs_overflow_check;           // Runtime overflow detection needed?
} VariableDeclaration;

// Variable assignment structure
typedef struct {
    char* name;                          // Variable name
    void* value_expr;                    // Expression* for value
    
    // ========== Phase 2: TTO Integration ==========
    TTOTypeInfo* tto_info;               // TTO analysis of assigned value (heap allocated)
    bool tto_analyzed;                   // Has TTO analysis been performed?
    bool needs_type_promotion;           // Does this need type promotion (int64 -> bigdec)?
} VariableAssignment;

#endif
