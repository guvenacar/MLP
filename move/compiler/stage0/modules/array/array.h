#ifndef ARRAY_H
#define ARRAY_H

#include "../variable/variable.h"

// Collection types
typedef enum {
    COLL_ARRAY,      // [] - Homogeneous, mutable, stack/heap
    COLL_LIST,       // () - Heterogeneous, mutable, heap
    COLL_TUPLE       // <> - Heterogeneous, immutable, stack
} CollectionType;

// Array: Fixed/dynamic size, homogeneous elements
typedef struct {
    VarType element_type;    // Type of elements (all same type)
    int capacity;            // Allocated capacity
    int length;              // Current number of elements
    void** elements;         // Array of element pointers
    int is_dynamic;          // 0 = fixed size (stack), 1 = dynamic (heap)
} Array;

// List: Dynamic size, heterogeneous elements (always heap)
typedef struct {
    int capacity;            // Allocated capacity
    int length;              // Current number of elements
    void** elements;         // Array of element pointers (mixed types)
    VarType* element_types;  // Type of each element
} List;

// Tuple: Fixed size, heterogeneous, immutable (stack only)
typedef struct {
    int length;              // Number of elements (fixed at creation)
    void** elements;         // Array of element pointers
    VarType* element_types;  // Type of each element
} Tuple;

// Generic collection wrapper
typedef struct {
    CollectionType type;
    union {
        Array array;
        List list;
        Tuple tuple;
    } data;
} Collection;

#endif
