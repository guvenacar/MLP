#ifndef ARRAY_OPERATIONS_H
#define ARRAY_OPERATIONS_H

#include <stddef.h>

typedef enum {
    ARRAY_OP_PUSH,         // array.push(value)
    ARRAY_OP_POP,          // array.pop()
    ARRAY_OP_SLICE,        // array.slice(start, end)
    ARRAY_OP_LENGTH,       // array.length
    ARRAY_OP_INDEXOF,      // array.indexOf(value)
    ARRAY_OP_CONCAT,       // array1.concat(array2)
    ARRAY_OP_REVERSE,      // array.reverse()
    ARRAY_OP_SORT          // array.sort()
} ArrayOperationType;

typedef struct {
    ArrayOperationType op_type;
    void* array_ref;       // Reference to array (variable or expression)
    void* operand1;        // First operand (value, index, etc.)
    void* operand2;        // Second operand (for slice, etc.)
} ArrayOperation;

// Create and manage array operations
ArrayOperation* array_op_create(ArrayOperationType type);
void array_op_set_array(ArrayOperation* op, void* array_ref);
void array_op_set_operand1(ArrayOperation* op, void* operand);
void array_op_set_operand2(ArrayOperation* op, void* operand);
void array_op_free(ArrayOperation* op);

#endif
