#include <stdlib.h>
#include "string_operations.h"

StringOperation* string_op_create(StringOperationType type) {
    StringOperation* op = malloc(sizeof(StringOperation));
    op->op_type = type;
    op->operand1 = NULL;
    op->operand2 = NULL;
    op->operand3 = NULL;
    return op;
}

void string_op_set_operand1(StringOperation* op, void* operand) {
    if (op) op->operand1 = operand;
}

void string_op_set_operand2(StringOperation* op, void* operand) {
    if (op) op->operand2 = operand;
}

void string_op_set_operand3(StringOperation* op, void* operand) {
    if (op) op->operand3 = operand;
}

void string_op_free(StringOperation* op) {
    if (!op) return;
    // Note: operands freed elsewhere in AST cleanup
    free(op);
}
