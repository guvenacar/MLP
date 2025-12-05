#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include "../../lexer.h"

// Arithmetic operation types
typedef enum {
    ARITH_ADD,       // +
    ARITH_SUB,       // -
    ARITH_MUL,       // *
    ARITH_DIV,       // /
    ARITH_MOD,       // %
    ARITH_POW,       // **
    ARITH_AND,       // & (bitwise AND)
    ARITH_OR,        // | (bitwise OR)
    ARITH_XOR        // ^ (bitwise XOR)
} ArithmeticOp;

// Arithmetic expression structure
typedef struct ArithmeticExpr {
    ArithmeticOp op;              // Operation type
    struct ArithmeticExpr* left;  // Left operand (can be nested expression)
    struct ArithmeticExpr* right; // Right operand (can be nested expression)
    
    // Leaf node data (for literals/variables)
    int is_literal;               // 1 if literal value, 0 if variable
    char* value;                  // Literal value or variable name
    int is_float;                 // 1 if floating point number
} ArithmeticExpr;

// Free arithmetic expression
void arithmetic_expr_free(ArithmeticExpr* expr);

#endif
