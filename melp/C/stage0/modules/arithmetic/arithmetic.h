#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <stdbool.h>
#include "../../lexer.h"
#include "../codegen_context/tto_types.h"

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
    
    // ========== Phase 2.3: TTO Type Propagation ==========
    TTOTypeInfo* tto_info;        // TTO analysis result (heap allocated)
    bool tto_analyzed;            // Has TTO analysis been performed?
    bool needs_overflow_check;    // Runtime overflow detection needed?
} ArithmeticExpr;

// Free arithmetic expression
void arithmetic_expr_free(ArithmeticExpr* expr);

// ========== Phase 2.3: TTO Type Propagation Functions ==========
// Infer TTO type for arithmetic expression
void arithmetic_infer_tto_type(ArithmeticExpr* expr);

// Propagate types through binary operations
TTOTypeInfo arithmetic_propagate_binary_types(TTOTypeInfo* left, TTOTypeInfo* right, ArithmeticOp op);

// Note: tto_infer_numeric_type() is defined in codegen_context.h

#endif
