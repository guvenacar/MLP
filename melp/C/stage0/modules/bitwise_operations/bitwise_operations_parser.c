#include "bitwise_operations_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parse bitwise operations - simplified version
BitwiseExpr* parse_bitwise_operation(Lexer* lexer) {
    BitwiseExpr* expr = malloc(sizeof(BitwiseExpr));
    memset(expr, 0, sizeof(BitwiseExpr));

    Token* tok = lexer_next_token(lexer);

    // Get left operand (number or identifier)
    if (tok->type == TOKEN_NUMBER) {
        expr->is_literal = 1;
        expr->value = strdup(tok->value);
    } else if (tok->type == TOKEN_IDENTIFIER) {
        expr->is_literal = 0;
        expr->value = strdup(tok->value);
    } else {
        token_free(tok);
        free(expr);
        return NULL;
    }
    token_free(tok);

    // Get operator
    tok = lexer_next_token(lexer);
    if (strcmp(tok->value, "&") == 0) {
        expr->op = BITWISE_AND;
    } else if (strcmp(tok->value, "|") == 0) {
        expr->op = BITWISE_OR;
    } else if (strcmp(tok->value, "^") == 0) {
        expr->op = BITWISE_XOR;
    } else if (strcmp(tok->value, "~") == 0) {
        expr->op = BITWISE_NOT;
    } else if (strcmp(tok->value, "<<") == 0) {
        expr->op = BITWISE_LSHIFT;
    } else if (strcmp(tok->value, ">>") == 0) {
        expr->op = BITWISE_RSHIFT;
    } else {
        // Not a bitwise operator
        token_free(tok);
        free(expr->value);
        free(expr);
        return NULL;
    }
    token_free(tok);

    // Get right operand (for binary operations)
    tok = lexer_next_token(lexer);
    if (tok->type == TOKEN_NUMBER || tok->type == TOKEN_IDENTIFIER) {
        expr->right = malloc(sizeof(BitwiseExpr));
        memset(expr->right, 0, sizeof(BitwiseExpr));
        expr->right->is_literal = (tok->type == TOKEN_NUMBER);
        expr->right->value = strdup(tok->value);
    }
    token_free(tok);

    return expr;
}
