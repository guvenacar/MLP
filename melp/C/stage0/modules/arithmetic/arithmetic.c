#include "arithmetic.h"
#include <stdlib.h>

// Free arithmetic expression
void arithmetic_expr_free(ArithmeticExpr* expr) {
    if (!expr) return;
    
    if (expr->left) arithmetic_expr_free(expr->left);
    if (expr->right) arithmetic_expr_free(expr->right);
    if (expr->value) free(expr->value);
    
    free(expr);
}

// Arithmetic operations module
// This module only contains operation type enums
// No implementation needed for Stage 0
