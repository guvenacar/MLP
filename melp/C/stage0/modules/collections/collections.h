#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#include "../variable/variable.h"
#include "../array/array.h"

// Expression types for collections
typedef enum {
    EXPR_TUPLE_LITERAL,   // <1, 2, 3> - Immutable, stack-allocated
    EXPR_LIST_LITERAL,    // (1, 2, 3) - Mutable, heap-allocated
    EXPR_TUPLE_ACCESS,    // tuple<0> - Access by angle brackets
    EXPR_LIST_ACCESS      // list(0) - Access by parentheses
} CollectionExprType;

// Tuple literal expression
typedef struct {
    int count;
    void** elements;         // Array of Expression pointers
    VarType* element_types;  // Type of each element (inferred)
} TupleLiteral;

// List literal expression
typedef struct {
    int count;
    void** elements;         // Array of Expression pointers
    VarType* element_types;  // Type of each element (inferred)
} ListLiteral;

// Tuple access expression: tuple<index>
typedef struct {
    void* tuple_expr;        // Expression that evaluates to tuple
    int index;               // Static index (compile-time constant)
} TupleAccess;

// List access expression: list(index)
typedef struct {
    void* list_expr;         // Expression that evaluates to list
    void* index_expr;        // Expression for index (can be runtime)
} ListAccess;

// Collection module functions
void collections_init(void);
int collections_is_tuple_literal(void* token);
int collections_is_list_literal(void* token);

#endif
