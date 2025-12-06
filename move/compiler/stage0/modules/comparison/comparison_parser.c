#include "comparison_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Helper: Advance to next token
static void advance(ComparisonParser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    parser->current_token = lexer_next_token(parser->lexer);
}

// Create parser
ComparisonParser* comparison_parser_create(Lexer* lexer) {
    ComparisonParser* parser = malloc(sizeof(ComparisonParser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

// Free parser
void comparison_parser_free(ComparisonParser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    free(parser);
}

// Parse primary value (number or variable)
static void parse_operand(ComparisonParser* parser, char** value, int* is_literal, int* is_float) {
    if (!parser || !parser->current_token) {
        *value = NULL;
        return;
    }
    
    if (parser->current_token->type == TOKEN_NUMBER) {
        *value = strdup(parser->current_token->value);
        *is_literal = 1;
        *is_float = (strchr(*value, '.') != NULL);
        advance(parser);
    } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
        *value = strdup(parser->current_token->value);
        *is_literal = 0;
        *is_float = 0;  // Will be determined at runtime
        advance(parser);
    } else {
        *value = NULL;
    }
}

// Parse comparison expression: value op value
ComparisonExpr* comparison_parse_expression(ComparisonParser* parser) {
    if (!parser || !parser->current_token) {
        return NULL;
    }
    
    ComparisonExpr* expr = malloc(sizeof(ComparisonExpr));
    expr->left = NULL;
    expr->right = NULL;
    expr->left_value = NULL;
    expr->right_value = NULL;
    expr->is_float = 0;
    
    // Parse left operand
    parse_operand(parser, &expr->left_value, &expr->left_is_literal, &expr->is_float);
    
    if (!expr->left_value) {
        free(expr);
        return NULL;
    }
    
    // Parse operator
    if (!parser->current_token) {
        comparison_expr_free(expr);
        return NULL;
    }
    
    switch (parser->current_token->type) {
        case TOKEN_EQUAL:
            expr->op = CMP_EQUAL;
            break;
        case TOKEN_NOT_EQUAL:
            expr->op = CMP_NOT_EQUAL;
            break;
        case TOKEN_LESS:
            expr->op = CMP_LESS;
            break;
        case TOKEN_LESS_EQUAL:
            expr->op = CMP_LESS_EQUAL;
            break;
        case TOKEN_GREATER:
            expr->op = CMP_GREATER;
            break;
        case TOKEN_GREATER_EQUAL:
            expr->op = CMP_GREATER_EQUAL;
            break;
        default:
            fprintf(stderr, "Error: Expected comparison operator\n");
            comparison_expr_free(expr);
            return NULL;
    }
    
    advance(parser);  // consume operator
    
    // Parse right operand
    int right_is_float = 0;
    parse_operand(parser, &expr->right_value, &expr->right_is_literal, &right_is_float);
    
    if (!expr->right_value) {
        comparison_expr_free(expr);
        return NULL;
    }
    
    // If either side is float, treat as float comparison
    if (right_is_float) {
        expr->is_float = 1;
    }
    
    return expr;
}
