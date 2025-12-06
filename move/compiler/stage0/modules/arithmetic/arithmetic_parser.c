#include "arithmetic_parser.h"
#include "../codegen_context/codegen_context.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Helper: Advance to next token
static void advance(ArithmeticParser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    parser->current_token = lexer_next_token(parser->lexer);
}

// Create parser
ArithmeticParser* arithmetic_parser_create(Lexer* lexer) {
    ArithmeticParser* parser = malloc(sizeof(ArithmeticParser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

// Free parser
void arithmetic_parser_free(ArithmeticParser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    free(parser);
}

// Parse primary expression (numbers, variables, parentheses)
ArithmeticExpr* arithmetic_parse_primary(ArithmeticParser* parser) {
    if (!parser || !parser->current_token) {
        return NULL;
    }
    
    ArithmeticExpr* expr = malloc(sizeof(ArithmeticExpr));
    expr->left = NULL;
    expr->right = NULL;
    expr->value = NULL;
    
    // Phase 2.3: Initialize TTO fields
    expr->tto_info = NULL;
    expr->tto_analyzed = false;
    expr->needs_overflow_check = false;
    
    // Number literal
    if (parser->current_token->type == TOKEN_NUMBER) {
        expr->is_literal = 1;
        expr->value = strdup(parser->current_token->value);
        expr->is_float = (strchr(expr->value, '.') != NULL);
        
        // Phase 2.3: TTO analysis for numeric literal
        TTOTypeInfo* tto = malloc(sizeof(TTOTypeInfo));
        *tto = tto_infer_numeric_type(expr->value);
        expr->tto_info = tto;
        expr->tto_analyzed = true;
        expr->needs_overflow_check = (tto->type == INTERNAL_TYPE_INT64);
        
        advance(parser);
        return expr;
    }
    
    // Variable
    if (parser->current_token->type == TOKEN_IDENTIFIER) {
        expr->is_literal = 0;
        expr->value = strdup(parser->current_token->value);
        expr->is_float = 0;
        
        // Phase 2.3: TTO info for variables (unknown at parse time)
        // We assume INT64 initially, will be refined at runtime
        TTOTypeInfo* tto = malloc(sizeof(TTOTypeInfo));
        tto->type = INTERNAL_TYPE_INT64;
        tto->is_constant = false;  // Variables can change
        tto->needs_promotion = true;  // May need overflow check
        tto->mem_location = MEM_REGISTER;
        expr->tto_info = tto;
        expr->tto_analyzed = true;
        expr->needs_overflow_check = true;
        
        advance(parser);
        return expr;
    }
    
    // Parentheses
    if (parser->current_token->type == TOKEN_LPAREN) {
        advance(parser);  // consume '('
        free(expr);  // We don't need this expr
        expr = arithmetic_parse_expression(parser);
        
        if (parser->current_token->type != TOKEN_RPAREN) {
            fprintf(stderr, "Error: Expected ')'\n");
            arithmetic_expr_free(expr);
            return NULL;
        }
        advance(parser);  // consume ')'
        return expr;
    }
    
    fprintf(stderr, "Error: Unexpected token in arithmetic expression\n");
    free(expr);
    return NULL;
}

// Parse power (highest precedence: **)
ArithmeticExpr* arithmetic_parse_power(ArithmeticParser* parser) {
    ArithmeticExpr* left = arithmetic_parse_primary(parser);
    if (!left) return NULL;
    
    while (parser->current_token && parser->current_token->type == TOKEN_POWER) {
        advance(parser);  // consume **
        
        ArithmeticExpr* right = arithmetic_parse_primary(parser);
        if (!right) {
            arithmetic_expr_free(left);
            return NULL;
        }
        
        // Create binary operation node
        ArithmeticExpr* binary = malloc(sizeof(ArithmeticExpr));
        binary->op = ARITH_POW;
        binary->left = left;
        binary->right = right;
        binary->is_literal = 0;
        binary->value = NULL;
        binary->is_float = 0;
        
        // Phase 2.3: TTO type propagation for binary operation
        binary->tto_info = NULL;
        binary->tto_analyzed = false;
        binary->needs_overflow_check = false;
        
        // If both operands have TTO info, propagate types
        if (left->tto_analyzed && right->tto_analyzed && left->tto_info && right->tto_info) {
            TTOTypeInfo* propagated = malloc(sizeof(TTOTypeInfo));
            *propagated = arithmetic_propagate_binary_types(left->tto_info, right->tto_info, ARITH_POW);
            binary->tto_info = propagated;
            binary->tto_analyzed = true;
            binary->needs_overflow_check = (propagated->type == INTERNAL_TYPE_INT64);
        }
        
        left = binary;
    }
    
    return left;
}

// Parse term (multiplication, division, modulo)
ArithmeticExpr* arithmetic_parse_term(ArithmeticParser* parser) {
    ArithmeticExpr* left = arithmetic_parse_power(parser);  // Changed from primary to power
    if (!left) return NULL;
    
    while (parser->current_token) {
        ArithmeticOp op;
        
        if (parser->current_token->type == TOKEN_MULTIPLY) {
            op = ARITH_MUL;
        } else if (parser->current_token->type == TOKEN_DIVIDE) {
            op = ARITH_DIV;
        } else if (parser->current_token->type == TOKEN_MOD) {
            op = ARITH_MOD;
        } else {
            break;  // Not a term-level operator
        }
        
        advance(parser);  // consume operator
        
        ArithmeticExpr* right = arithmetic_parse_power(parser);  // Changed from primary to power
        if (!right) {
            arithmetic_expr_free(left);
            return NULL;
        }
        
        // Create binary operation node
        ArithmeticExpr* binary = malloc(sizeof(ArithmeticExpr));
        binary->op = op;
        binary->left = left;
        binary->right = right;
        binary->is_literal = 0;
        binary->value = NULL;
        binary->is_float = 0;
        
        // Phase 2.3: TTO type propagation
        binary->tto_info = NULL;
        binary->tto_analyzed = false;
        binary->needs_overflow_check = false;
        
        if (left->tto_analyzed && right->tto_analyzed && left->tto_info && right->tto_info) {
            TTOTypeInfo* propagated = malloc(sizeof(TTOTypeInfo));
            *propagated = arithmetic_propagate_binary_types(left->tto_info, right->tto_info, op);
            binary->tto_info = propagated;
            binary->tto_analyzed = true;
            binary->needs_overflow_check = (propagated->type == INTERNAL_TYPE_INT64);
        }
        
        left = binary;
    }
    
    return left;
}

// Parse factor (addition, subtraction)
ArithmeticExpr* arithmetic_parse_factor(ArithmeticParser* parser) {
    ArithmeticExpr* left = arithmetic_parse_term(parser);
    if (!left) return NULL;
    
    while (parser->current_token) {
        ArithmeticOp op;
        
        if (parser->current_token->type == TOKEN_PLUS) {
            op = ARITH_ADD;
        } else if (parser->current_token->type == TOKEN_MINUS) {
            op = ARITH_SUB;
        } else {
            break;  // Not a factor-level operator
        }
        
        advance(parser);  // consume operator
        
        ArithmeticExpr* right = arithmetic_parse_term(parser);
        if (!right) {
            arithmetic_expr_free(left);
            return NULL;
        }
        
        // Create binary operation node
        ArithmeticExpr* binary = malloc(sizeof(ArithmeticExpr));
        binary->op = op;
        binary->left = left;
        binary->right = right;
        binary->is_literal = 0;
        binary->value = NULL;
        binary->is_float = 0;
        
        // Phase 2.3: TTO type propagation
        binary->tto_info = NULL;
        binary->tto_analyzed = false;
        binary->needs_overflow_check = false;
        
        if (left->tto_analyzed && right->tto_analyzed && left->tto_info && right->tto_info) {
            TTOTypeInfo* propagated = malloc(sizeof(TTOTypeInfo));
            *propagated = arithmetic_propagate_binary_types(left->tto_info, right->tto_info, op);
            binary->tto_info = propagated;
            binary->tto_analyzed = true;
            binary->needs_overflow_check = (propagated->type == INTERNAL_TYPE_INT64);
        }
        
        left = binary;
    }
    
    return left;
}

// Parse bitwise (lowest precedence: &, |, ^)
ArithmeticExpr* arithmetic_parse_bitwise(ArithmeticParser* parser) {
    ArithmeticExpr* left = arithmetic_parse_factor(parser);
    if (!left) return NULL;
    
    while (parser->current_token) {
        ArithmeticOp op;
        
        if (parser->current_token->type == TOKEN_AND) {
            op = ARITH_AND;
        } else if (parser->current_token->type == TOKEN_OR) {
            op = ARITH_OR;
        } else if (parser->current_token->type == TOKEN_XOR) {
            op = ARITH_XOR;
        } else {
            break;  // Not a bitwise operator
        }
        
        advance(parser);  // consume operator
        
        ArithmeticExpr* right = arithmetic_parse_factor(parser);
        if (!right) {
            arithmetic_expr_free(left);
            return NULL;
        }
        
        // Create binary operation node
        ArithmeticExpr* binary = malloc(sizeof(ArithmeticExpr));
        binary->op = op;
        binary->left = left;
        binary->right = right;
        binary->is_literal = 0;
        binary->value = NULL;
        binary->is_float = 0;
        
        // Phase 2.3: TTO type propagation
        binary->tto_info = NULL;
        binary->tto_analyzed = false;
        binary->needs_overflow_check = false;
        
        if (left->tto_analyzed && right->tto_analyzed && left->tto_info && right->tto_info) {
            TTOTypeInfo* propagated = malloc(sizeof(TTOTypeInfo));
            *propagated = arithmetic_propagate_binary_types(left->tto_info, right->tto_info, op);
            binary->tto_info = propagated;
            binary->tto_analyzed = true;
            binary->needs_overflow_check = (propagated->type == INTERNAL_TYPE_INT64);
        }
        
        left = binary;
    }
    
    return left;
}

// Parse full expression (entry point)
ArithmeticExpr* arithmetic_parse_expression(ArithmeticParser* parser) {
    return arithmetic_parse_bitwise(parser);  // Changed from factor to bitwise
}

// Parse assignment: varname = expression
// Returns variable name, fills expr pointer
char* arithmetic_parse_assignment(ArithmeticParser* parser, ArithmeticExpr** expr) {
    if (!parser || !parser->current_token || !expr) {
        return NULL;
    }
    
    // We expect: IDENTIFIER = EXPRESSION
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        return NULL;  // Not an assignment
    }
    
    char* var_name = strdup(parser->current_token->value);
    advance(parser);
    
    // Check for '='
    if (!parser->current_token || parser->current_token->type != TOKEN_EQUAL) {
        free(var_name);
        return NULL;  // Not an assignment
    }
    
    advance(parser);  // consume '='
    
    // Parse expression
    *expr = arithmetic_parse_expression(parser);
    
    if (!*expr) {
        free(var_name);
        return NULL;
    }
    
    return var_name;
}

