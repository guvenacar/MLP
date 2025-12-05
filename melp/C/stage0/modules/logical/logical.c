#include "logical.h"
#include <stdlib.h>

// Free logical expression
void logical_expr_free(LogicalExpr* expr) {
    if (!expr) return;
    
    if (expr->left) {
        logical_expr_free(expr->left);
    }
    
    if (expr->right) {
        logical_expr_free(expr->right);
    }
    
    if (expr->value) {
        free(expr->value);
    }
    
    free(expr);
}
