#include <stdlib.h>
#include "array_operations.h"

ArrayOperation* array_op_create(ArrayOperationType type) {
    ArrayOperation* op = malloc(sizeof(ArrayOperation));
    op->op_type = type;
    op->array_ref = NULL;
    op->operand1 = NULL;
    op->operand2 = NULL;
    return op;
}

void array_op_set_array(ArrayOperation* op, void* array_ref) {
    if (op) op->array_ref = array_ref;
}

void array_op_set_operand1(ArrayOperation* op, void* operand) {
    if (op) op->operand1 = operand;
}

void array_op_set_operand2(ArrayOperation* op, void* operand) {
    if (op) op->operand2 = operand;
}

void array_op_free(ArrayOperation* op) {
    if (!op) return;
    // Note: operands freed elsewhere in AST cleanup
    free(op);
}
