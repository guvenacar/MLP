#ifndef VARIABLE_H
#define VARIABLE_H

typedef enum {
    VAR_NUMERIC,
    VAR_STRING,
    VAR_BOOLEAN
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

#endif
