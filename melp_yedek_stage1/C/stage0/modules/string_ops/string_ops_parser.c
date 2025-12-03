#include "string_ops_parser.h"
#include "../arithmetic/arithmetic_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

StringOpsParser* str_parser_create(Lexer* lexer) {
    StringOpsParser* parser = malloc(sizeof(StringOpsParser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

void str_parser_free(StringOpsParser* parser) {
    if (parser->current_token) token_free(parser->current_token);
    free(parser);
}

void string_op_free(StringOpExpr* op) {
    if (!op) return;
    
    expression_free(op->left);
    expression_free(op->right);
    expression_free(op->third);
    free(op);
}

// Helper: Advance to next token
static void advance(StringOpsParser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    parser->current_token = lexer_next_token(parser->lexer);
}

// Helper: Check if current token matches expected type
static int match(StringOpsParser* parser, TokenType type) {
    return parser->current_token && parser->current_token->type == type;
}

// Helper: Expect a specific token or error
static int expect(StringOpsParser* parser, TokenType type, const char* msg) {
    if (!match(parser, type)) {
        fprintf(stderr, "Error: %s\n", msg);
        return 0;
    }
    advance(parser);
    return 1;
}

// Parse string concatenation
// Syntax: string_expr + string_expr
StringOpExpr* parse_string_concat(StringOpsParser* parser) {
    // Convert StringOpsParser to common Parser for expression parsing
    Parser temp_parser;
    temp_parser.lexer = parser->lexer;
    temp_parser.current_token = parser->current_token;
    
    // Parse left string expression
    Expression* left = expression_parse(&temp_parser);
    parser->current_token = temp_parser.current_token;
    
    if (!left) {
        fprintf(stderr, "Error: Failed to parse left string expression\n");
        return NULL;
    }
    
    // Expect '+' operator
    if (!match(parser, TOKEN_PLUS)) {
        fprintf(stderr, "Error: Expected '+' for string concatenation\n");
        expression_free(left);
        return NULL;
    }
    advance(parser);  // consume '+'
    
    // Parse right string expression
    temp_parser.current_token = parser->current_token;
    Expression* right = expression_parse(&temp_parser);
    parser->current_token = temp_parser.current_token;
    
    if (!right) {
        fprintf(stderr, "Error: Failed to parse right string expression\n");
        expression_free(left);
        return NULL;
    }
    
    // Build string concatenation node
    StringOpExpr* op = malloc(sizeof(StringOpExpr));
    op->op = STR_OP_CONCAT;
    op->left = left;
    op->right = right;
    op->third = NULL;
    
    return op;
}

// Parse string function call
// Syntax: func_name(arg1 [, arg2 [, arg3]])
StringOpExpr* parse_string_function(StringOpsParser* parser, const char* func_name) {
    // Determine operation type
    StringOp op;
    int arg_count = 0;  // Expected argument count
    
    if (strcmp(func_name, "length") == 0) {
        op = STR_OP_LENGTH;
        arg_count = 1;
    } else if (strcmp(func_name, "substr") == 0) {
        op = STR_OP_SUBSTR;
        arg_count = 3;  // substr(s, start, len)
    } else if (strcmp(func_name, "split") == 0) {
        op = STR_OP_SPLIT;
        arg_count = 2;  // split(s, delimiter)
    } else if (strcmp(func_name, "replace") == 0) {
        op = STR_OP_REPLACE;
        arg_count = 3;  // replace(s, old, new)
    } else if (strcmp(func_name, "trim") == 0) {
        op = STR_OP_TRIM;
        arg_count = 1;
    } else if (strcmp(func_name, "upper") == 0) {
        op = STR_OP_UPPER;
        arg_count = 1;
    } else if (strcmp(func_name, "lower") == 0) {
        op = STR_OP_LOWER;
        arg_count = 1;
    } else if (strcmp(func_name, "contains") == 0) {
        op = STR_OP_CONTAINS;
        arg_count = 2;  // contains(s, substring)
    } else if (strcmp(func_name, "indexof") == 0) {
        op = STR_OP_INDEXOF;
        arg_count = 2;  // indexof(s, substring)
    } else {
        fprintf(stderr, "Error: Unknown string function: %s\n", func_name);
        return NULL;
    }
    
    // Expect '('
    if (!expect(parser, TOKEN_LPAREN, "Expected '(' after function name")) {
        return NULL;
    }
    
    // Parse arguments
    Parser temp_parser;
    temp_parser.lexer = parser->lexer;
    temp_parser.current_token = parser->current_token;
    
    Expression* arg1 = NULL;
    Expression* arg2 = NULL;
    Expression* arg3 = NULL;
    
    // Parse first argument (always required)
    arg1 = expression_parse(&temp_parser);
    parser->current_token = temp_parser.current_token;
    
    if (!arg1) {
        fprintf(stderr, "Error: Failed to parse first argument\n");
        return NULL;
    }
    
    // Parse second argument if needed
    if (arg_count >= 2) {
        if (!expect(parser, TOKEN_COMMA, "Expected ',' after first argument")) {
            expression_free(arg1);
            return NULL;
        }
        
        temp_parser.current_token = parser->current_token;
        arg2 = expression_parse(&temp_parser);
        parser->current_token = temp_parser.current_token;
        
        if (!arg2) {
            fprintf(stderr, "Error: Failed to parse second argument\n");
            expression_free(arg1);
            return NULL;
        }
    }
    
    // Parse third argument if needed
    if (arg_count >= 3) {
        if (!expect(parser, TOKEN_COMMA, "Expected ',' after second argument")) {
            expression_free(arg1);
            expression_free(arg2);
            return NULL;
        }
        
        temp_parser.current_token = parser->current_token;
        arg3 = expression_parse(&temp_parser);
        parser->current_token = temp_parser.current_token;
        
        if (!arg3) {
            fprintf(stderr, "Error: Failed to parse third argument\n");
            expression_free(arg1);
            expression_free(arg2);
            return NULL;
        }
    }
    
    // Expect ')'
    if (!expect(parser, TOKEN_RPAREN, "Expected ')' after arguments")) {
        expression_free(arg1);
        expression_free(arg2);
        expression_free(arg3);
        return NULL;
    }
    
    // Build string operation node
    StringOpExpr* expr = malloc(sizeof(StringOpExpr));
    expr->op = op;
    expr->left = arg1;
    expr->right = arg2;
    expr->third = arg3;
    
    return expr;
}
