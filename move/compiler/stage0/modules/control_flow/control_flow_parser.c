#include "control_flow_parser.h"
#include "../comparison/comparison_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void advance(ControlFlowParser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    parser->current_token = lexer_next_token(parser->lexer);
}

ControlFlowParser* control_flow_parser_create(Lexer* lexer) {
    ControlFlowParser* parser = malloc(sizeof(ControlFlowParser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

void control_flow_parser_free(ControlFlowParser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    free(parser);
}

// Parse expression-based if (Tier 1 feature)
IfStatement* control_flow_parse_if_expr(ControlFlowParser* parser) {
    // Parse: result = if condition then value1 else value2
    ComparisonParser* cmp = comparison_parser_create(parser->lexer);
    
    IfStatement* stmt = malloc(sizeof(IfStatement));
    stmt->condition = comparison_parse_expression(cmp);
    stmt->has_else = 1;  // Expression-based if requires else
    stmt->type = CTRL_IF_EXPR;
    
    if (parser->current_token) token_free(parser->current_token);
    parser->current_token = cmp->current_token;
    cmp->current_token = NULL;
    comparison_parser_free(cmp);
    
    return stmt;
}

// Parse if statement
IfStatement* control_flow_parse_if(ControlFlowParser* parser) {
    if (!parser || !parser->current_token || parser->current_token->type != TOKEN_IF) {
        return NULL;
    }
    advance(parser);
    
    IfStatement* stmt = malloc(sizeof(IfStatement));
    stmt->condition = NULL;
    stmt->has_else = 0;
    
    // Parse condition using comparison parser
    ComparisonParser* cmp = comparison_parser_create(parser->lexer);
    stmt->condition = comparison_parse_expression(cmp);
    
    // Sync tokens
    if (parser->current_token) token_free(parser->current_token);
    parser->current_token = cmp->current_token;
    cmp->current_token = NULL;
    comparison_parser_free(cmp);
    
    if (!stmt->condition) {
        if_statement_free(stmt);
        return NULL;
    }
    
    // Expect 'then'
    if (!parser->current_token || parser->current_token->type != TOKEN_THEN) {
        if_statement_free(stmt);
        return NULL;
    }
    advance(parser);
    
    // Skip body until 'else' or 'end if'
    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        if (parser->current_token->type == TOKEN_ELSE) {
            stmt->has_else = 1;
            advance(parser);
            // Skip else body
            while (parser->current_token && parser->current_token->type != TOKEN_END) {
                advance(parser);
            }
            break;
        }
        if (parser->current_token->type == TOKEN_END) {
            advance(parser);
            if (parser->current_token && parser->current_token->type == TOKEN_IF) {
                advance(parser);
            }
            break;
        }
        advance(parser);
    }
    
    return stmt;
}

// Parse while statement
WhileStatement* control_flow_parse_while(ControlFlowParser* parser) {
    if (!parser || !parser->current_token || parser->current_token->type != TOKEN_WHILE) {
        return NULL;
    }
    advance(parser);
    
    WhileStatement* stmt = malloc(sizeof(WhileStatement));
    stmt->condition = NULL;
    
    // Parse condition
    ComparisonParser* cmp = comparison_parser_create(parser->lexer);
    stmt->condition = comparison_parse_expression(cmp);
    
    if (parser->current_token) token_free(parser->current_token);
    parser->current_token = cmp->current_token;
    cmp->current_token = NULL;
    comparison_parser_free(cmp);
    
    // Skip to 'end while'
    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        if (parser->current_token->type == TOKEN_END) {
            advance(parser);
            if (parser->current_token && parser->current_token->type == TOKEN_WHILE) {
                advance(parser);
            }
            break;
        }
        advance(parser);
    }
    
    return stmt;
}

// Parse for statement
ForStatement* control_flow_parse_for(ControlFlowParser* parser) {
    if (!parser || !parser->current_token || parser->current_token->type != TOKEN_FOR) {
        return NULL;
    }
    advance(parser);
    
    ForStatement* stmt = malloc(sizeof(ForStatement));
    stmt->iterator = NULL;
    stmt->start = NULL;
    stmt->end = NULL;
    stmt->label = NULL;  // Initialize label
    
    // Check for label: outer: for i in ...
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
        Token* peek = lexer_next_token(parser->lexer);
        if (peek && peek->type == TOKEN_COLON) {
            // This is a label
            stmt->label = strdup(parser->current_token->value);
            advance(parser);  // Consume label
            advance(parser);  // Consume colon
            
            // Now expect 'for' keyword again
            if (parser->current_token && parser->current_token->type == TOKEN_FOR) {
                advance(parser);
            }
        }
        if (peek) token_free(peek);
    }
    
    // Get iterator
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
        stmt->iterator = strdup(parser->current_token->value);
        advance(parser);
    }
    
    // Skip to 'end for'
    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        if (parser->current_token->type == TOKEN_END) {
            advance(parser);
            if (parser->current_token && parser->current_token->type == TOKEN_FOR) {
                advance(parser);
            }
            break;
        }
        advance(parser);
    }
    
    return stmt;
}
