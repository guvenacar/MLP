#include "arithmetic_parser.h"
#include <stdlib.h>
#include <string.h>

// Expression parsing for arithmetic operations
// Stage 1: Basic expression parsing with precedence

void expression_free(Expression* expr) {
    if (!expr) return;
    
    if (expr->type == EXPR_VARIABLE) {
        free(expr->data.var_name);
    } else if (expr->type == EXPR_BINARY_OP) {
        expression_free(expr->data.binary.left);
        expression_free(expr->data.binary.right);
    }
    
    free(expr);
}

// TODO: Implement expression parsing in Stage 1
// For now, only placeholder
Expression* expression_parse(Parser* parser) {
    // Placeholder: will be implemented with full expression parsing
    return NULL;
}
