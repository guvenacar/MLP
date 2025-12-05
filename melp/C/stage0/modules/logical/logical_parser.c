#include "logical_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Helper: Advance to next token
static void advance(LogicalParser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    parser->current_token = lexer_next_token(parser->lexer);
}

// Create parser
LogicalParser* logical_parser_create(Lexer* lexer) {
    LogicalParser* parser = malloc(sizeof(LogicalParser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

// Free parser
void logical_parser_free(LogicalParser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    free(parser);
}

// Parse primary (boolean literal or variable)
LogicalExpr* logical_parse_primary(LogicalParser* parser) {
    if (!parser || !parser->current_token) {
        return NULL;
    }
    
    LogicalExpr* expr = malloc(sizeof(LogicalExpr));
    expr->left = NULL;
    expr->right = NULL;
    expr->value = NULL;
    
    // Boolean literal
    if (parser->current_token->type == TOKEN_TRUE || 
        parser->current_token->type == TOKEN_FALSE) {
        expr->is_literal = 1;
        expr->value = strdup(parser->current_token->value);
        advance(parser);
        return expr;
    }
    
    // Variable
    if (parser->current_token->type == TOKEN_IDENTIFIER) {
        expr->is_literal = 0;
        expr->value = strdup(parser->current_token->value);
        advance(parser);
        return expr;
    }
    
    // Parentheses
    if (parser->current_token->type == TOKEN_LPAREN) {
        advance(parser);
        free(expr);
        expr = logical_parse_expression(parser);
        
        if (parser->current_token->type != TOKEN_RPAREN) {
            fprintf(stderr, "Error: Expected ')'\n");
            logical_expr_free(expr);
            return NULL;
        }
        advance(parser);
        return expr;
    }
    
    free(expr);
    return NULL;
}

// Parse NOT (highest precedence)
LogicalExpr* logical_parse_not(LogicalParser* parser) {
    if (!parser || !parser->current_token) {
        return NULL;
    }
    
    // Check for NOT operator
    if (parser->current_token->type == TOKEN_NOT) {
        advance(parser);
        
        LogicalExpr* expr = malloc(sizeof(LogicalExpr));
        expr->op = LOG_NOT;
        expr->left = logical_parse_not(parser);  // Right-associative
        expr->right = NULL;
        expr->is_literal = 0;
        expr->value = NULL;
        
        if (!expr->left) {
            free(expr);
            return NULL;
        }
        
        return expr;
    }
    
    return logical_parse_primary(parser);
}

// Parse AND
LogicalExpr* logical_parse_and(LogicalParser* parser) {
    LogicalExpr* left = logical_parse_not(parser);
    if (!left) return NULL;
    
    while (parser->current_token && parser->current_token->type == TOKEN_AND) {
        advance(parser);
        
        LogicalExpr* right = logical_parse_not(parser);
        if (!right) {
            logical_expr_free(left);
            return NULL;
        }
        
        LogicalExpr* binary = malloc(sizeof(LogicalExpr));
        binary->op = LOG_AND;
        binary->left = left;
        binary->right = right;
        binary->is_literal = 0;
        binary->value = NULL;
        
        left = binary;
    }
    
    return left;
}

// Parse OR (lowest precedence)
LogicalExpr* logical_parse_or(LogicalParser* parser) {
    LogicalExpr* left = logical_parse_and(parser);
    if (!left) return NULL;
    
    while (parser->current_token && parser->current_token->type == TOKEN_OR) {
        advance(parser);
        
        LogicalExpr* right = logical_parse_and(parser);
        if (!right) {
            logical_expr_free(left);
            return NULL;
        }
        
        LogicalExpr* binary = malloc(sizeof(LogicalExpr));
        binary->op = LOG_OR;
        binary->left = left;
        binary->right = right;
        binary->is_literal = 0;
        binary->value = NULL;
        
        left = binary;
    }
    
    return left;
}

// Parse expression (entry point)
LogicalExpr* logical_parse_expression(LogicalParser* parser) {
    return logical_parse_or(parser);
}
