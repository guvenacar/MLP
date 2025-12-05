#include "bitwise_operations.h"
#include <stdlib.h>
#include <string.h>

// Free bitwise expression
void bitwise_expr_free(BitwiseExpr* expr) {
    if (!expr) return;

    if (expr->left) {
        bitwise_expr_free(expr->left);
    }
    if (expr->right) {
        bitwise_expr_free(expr->right);
    }
    if (expr->value) {
        free(expr->value);
    }

    free(expr);
}
