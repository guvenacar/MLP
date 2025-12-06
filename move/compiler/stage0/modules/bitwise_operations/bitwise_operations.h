#ifndef BITWISE_OPERATIONS_H
#define BITWISE_OPERATIONS_H

#include "../../lexer.h"

// Bitwise operation types
typedef enum {
    BITWISE_AND,        // &
    BITWISE_OR,         // |
    BITWISE_XOR,        // ^
    BITWISE_NOT,        // ~
    BITWISE_LSHIFT,     // <<
    BITWISE_RSHIFT,     // >>
    BITWISE_ARSHIFT     // >>> (arithmetic right shift - opsiyonel)
} BitwiseOp;

// Bitwise expression structure
typedef struct BitwiseExpr {
    BitwiseOp op;                     // Operation type
    struct BitwiseExpr* left;         // Left operand (NULL for unary NOT)
    struct BitwiseExpr* right;        // Right operand

    // Leaf node data (for literals/variables)
    int is_literal;                   // 1 if literal value, 0 if variable
    char* value;                      // Literal value or variable name
} BitwiseExpr;

// Free bitwise expression
void bitwise_expr_free(BitwiseExpr* expr);

#endif
