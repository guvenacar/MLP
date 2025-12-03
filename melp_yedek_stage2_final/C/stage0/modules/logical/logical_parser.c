#include "logical_parser.h"
#include "../comparison/comparison_parser.h"
#include "../arithmetic/arithmetic_parser.h"
#include <stdlib.h>
#include <stdio.h>

// Logical expression parsing
// Stage 1: Parse logical operations (AND, OR, NOT)

void logical_free(LogicalExpr* logical) {
    if (!logical) return;
    if (logical->left) expression_free(logical->left);
    if (logical->right) expression_free(logical->right);
    free(logical);
}

// Helper: Check if current token is a logical operator
static int is_logical_op(TokenType type) {
    return type == TOKEN_AND || type == TOKEN_OR || type == TOKEN_NOT;
}

// Helper: Convert token type to LogicalOp
static LogicalOp token_to_logop(TokenType type) {
    switch (type) {
        case TOKEN_AND: return LOG_AND;
        case TOKEN_OR:  return LOG_OR;
        case TOKEN_NOT: return LOG_NOT;
        default:
            fprintf(stderr, "Error: Unknown logical operator: %d\n", type);
            return LOG_AND;  // fallback
    }
}

// Helper: Advance to next token
static void advance(Parser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    parser->current_token = lexer_next_token(parser->lexer);
}

// Forward declaration for recursive parsing
static Expression* parse_logical_term(Parser* parser);

// Helper: Parse NOT expression (unary)
static LogicalExpr* parse_not_expr(Parser* parser) {
    advance(parser);  // consume '!'
    
    // Parse operand (can be another logical expression)
    Expression* operand = parse_logical_term(parser);
    if (!operand) {
        return NULL;
    }
    
    LogicalExpr* not_expr = malloc(sizeof(LogicalExpr));
    not_expr->op = LOG_NOT;
    not_expr->left = NULL;
    not_expr->right = operand;
    
    return not_expr;
}

// Helper: Parse a logical term (comparison or parenthesized logical expression)
static Expression* parse_logical_term(Parser* parser) {
    // Check for unary NOT
    if (parser->current_token && parser->current_token->type == TOKEN_NOT) {
        // Parse NOT as a special logical expression
        // For now, wrap it in an Expression (implementation detail)
        return (Expression*)parse_not_expr(parser);
    }
    
    // Check for parenthesized expression
    if (parser->current_token && parser->current_token->type == TOKEN_LPAREN) {
        advance(parser);  // consume '('
        Expression* expr = (Expression*)logical_parse(parser);
        if (parser->current_token && parser->current_token->type == TOKEN_RPAREN) {
            advance(parser);  // consume ')'
        }
        return expr;
    }
    
    // Try parsing as comparison (which internally parses arithmetic)
    ComparisonExpr* cmp = comparison_parse(parser);
    if (cmp) {
        // Wrap comparison in Expression for now
        return (Expression*)cmp;
    }
    
    // Fallback: parse as arithmetic expression
    return expression_parse(parser);
}

// Main logical parser
// Parses: logical_term ( (AND|OR) logical_term )*
// Precedence: NOT > AND > OR
LogicalExpr* logical_parse(Parser* parser) {
    // Parse left term
    Expression* left = parse_logical_term(parser);
    if (!left) return NULL;
    
    // Check if logical operator follows
    if (!parser->current_token || 
        (parser->current_token->type != TOKEN_AND && 
         parser->current_token->type != TOKEN_OR)) {
        // No binary logical operator - return single term
        // Note: This might be a comparison or arithmetic expression
        expression_free(left);
        return NULL;
    }
    
    // Parse AND/OR chains with left-to-right associativity
    LogicalOp op = token_to_logop(parser->current_token->type);
    advance(parser);  // consume operator
    
    // Parse right term
    Expression* right = parse_logical_term(parser);
    if (!right) {
        expression_free(left);
        return NULL;
    }
    
    // Build logical node
    LogicalExpr* logical = malloc(sizeof(LogicalExpr));
    logical->op = op;
    logical->left = left;
    logical->right = right;
    
    // Handle chained operators (left-to-right)
    while (parser->current_token && 
           (parser->current_token->type == TOKEN_AND || 
            parser->current_token->type == TOKEN_OR)) {
        LogicalOp next_op = token_to_logop(parser->current_token->type);
        advance(parser);
        
        Expression* next_right = parse_logical_term(parser);
        if (!next_right) {
            logical_free(logical);
            return NULL;
        }
        
        // Build new node with previous result as left
        LogicalExpr* new_logical = malloc(sizeof(LogicalExpr));
        new_logical->op = next_op;
        new_logical->left = (Expression*)logical;
        new_logical->right = next_right;
        logical = new_logical;
    }
    
    return logical;
}
