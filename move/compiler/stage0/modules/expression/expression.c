#include "expression.h"
#include <stdlib.h>
#include <string.h>

Expression* expression_create(ExpressionType type) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = type;
    memset(&expr->data, 0, sizeof(expr->data));
    return expr;
}

void expression_free(Expression* expr) {
    if (!expr) return;
    
    switch (expr->type) {
        case EXPR_STRING:
            free(expr->data.string_value);
            break;
        case EXPR_VARIABLE:
            free(expr->data.var_name);
            break;
        case EXPR_BINARY:
            expression_free(expr->data.binary.left);
            expression_free(expr->data.binary.right);
            free(expr->data.binary.operator);
            break;
        case EXPR_UNARY:
            expression_free(expr->data.unary.operand);
            free(expr->data.unary.operator);
            break;
        case EXPR_CALL:
            free(expr->data.call.func_name);
            for (int i = 0; i < expr->data.call.arg_count; i++) {
                expression_free(expr->data.call.args[i]);
            }
            free(expr->data.call.args);
            break;
        default:
            break;
    }
    
    free(expr);
}
