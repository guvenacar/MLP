#include "control_flow_parser.h"
#include "../logical/logical_parser.h"
#include "../comparison/comparison_parser.h"
#include "../arithmetic/arithmetic_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ControlFlowParser* cf_parser_create(Lexer* lexer) {
    ControlFlowParser* parser = malloc(sizeof(ControlFlowParser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

void cf_parser_free(ControlFlowParser* parser) {
    if (parser->current_token) token_free(parser->current_token);
    free(parser);
}

void statement_free(Statement* stmt) {
    if (!stmt) return;
    
    if (stmt->type == STMT_IF) {
        IfStatement* if_stmt = &stmt->data.if_stmt;
        expression_free(if_stmt->condition);
        for (int i = 0; i < if_stmt->then_count; i++) {
            statement_free(if_stmt->then_body[i]);
        }
        free(if_stmt->then_body);
        for (int i = 0; i < if_stmt->else_count; i++) {
            statement_free(if_stmt->else_body[i]);
        }
        free(if_stmt->else_body);
    } else if (stmt->type == STMT_WHILE) {
        WhileStatement* while_stmt = &stmt->data.while_stmt;
        expression_free(while_stmt->condition);
        for (int i = 0; i < while_stmt->body_count; i++) {
            statement_free(while_stmt->body[i]);
        }
        free(while_stmt->body);
    } else if (stmt->type == STMT_FOR) {
        ForStatement* for_stmt = &stmt->data.for_stmt;
        free(for_stmt->variable);
        expression_free(for_stmt->start);
        expression_free(for_stmt->end);
        for (int i = 0; i < for_stmt->body_count; i++) {
            statement_free(for_stmt->body[i]);
        }
        free(for_stmt->body);
    }
    
    free(stmt);
}

// Helper: Advance to next token
static void advance(ControlFlowParser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    parser->current_token = lexer_next_token(parser->lexer);
}

// Helper: Check if current token matches expected type
static int match(ControlFlowParser* parser, TokenType type) {
    return parser->current_token && parser->current_token->type == type;
}

// Helper: Expect a specific token or error
static int expect(ControlFlowParser* parser, TokenType type, const char* msg) {
    if (!match(parser, type)) {
        fprintf(stderr, "Error: %s\n", msg);
        return 0;
    }
    advance(parser);
    return 1;
}

// Helper: Parse a condition expression (logical or comparison)
static Expression* parse_condition(ControlFlowParser* parser) {
    // Convert ControlFlowParser to common Parser
    Parser temp_parser;
    temp_parser.lexer = parser->lexer;
    temp_parser.current_token = parser->current_token;
    
    // Try logical expression first (handles AND/OR/NOT)
    LogicalExpr* logical = logical_parse(&temp_parser);
    if (logical) {
        parser->current_token = temp_parser.current_token;
        return (Expression*)logical;
    }
    
    // Try comparison expression (==, !=, <, >, <=, >=)
    ComparisonExpr* cmp = comparison_parse(&temp_parser);
    if (cmp) {
        parser->current_token = temp_parser.current_token;
        return (Expression*)cmp;
    }
    
    // Fallback: arithmetic expression (for simple boolean values)
    Expression* expr = expression_parse(&temp_parser);
    parser->current_token = temp_parser.current_token;
    return expr;
}

// Helper: Parse statement body (list of statements until 'end' or 'else')
static Statement** parse_body(ControlFlowParser* parser, int* count) {
    int capacity = 4;
    Statement** body = malloc(sizeof(Statement*) * capacity);
    *count = 0;
    
    while (parser->current_token && 
           parser->current_token->type != TOKEN_END &&
           parser->current_token->type != TOKEN_ELSE &&
           parser->current_token->type != TOKEN_EOF) {
        
        // Check for nested control flow
        Statement* stmt = NULL;
        if (match(parser, TOKEN_IF)) {
            stmt = parse_if_statement(parser);
        } else if (match(parser, TOKEN_WHILE)) {
            stmt = parse_while_statement(parser);
        } else if (match(parser, TOKEN_FOR)) {
            stmt = parse_for_statement(parser);
        } else {
            // Skip unknown tokens for now (placeholder for other statements)
            advance(parser);
            continue;
        }
        
        if (stmt) {
            if (*count >= capacity) {
                capacity *= 2;
                body = realloc(body, sizeof(Statement*) * capacity);
            }
            body[*count] = stmt;
            (*count)++;
        }
    }
    
    return body;
}

// Parse if-then-else-end statement
// Syntax: if condition then ... [else ...] end if
Statement* parse_if_statement(ControlFlowParser* parser) {
    if (!expect(parser, TOKEN_IF, "Expected 'if' keyword")) {
        return NULL;
    }
    
    // Parse condition
    Expression* condition = parse_condition(parser);
    if (!condition) {
        fprintf(stderr, "Error: Failed to parse if condition\n");
        return NULL;
    }
    
    // Expect 'then'
    if (!expect(parser, TOKEN_THEN, "Expected 'then' after if condition")) {
        expression_free(condition);
        return NULL;
    }
    
    // Parse then body
    int then_count = 0;
    Statement** then_body = parse_body(parser, &then_count);
    
    // Check for else clause
    int else_count = 0;
    Statement** else_body = NULL;
    if (match(parser, TOKEN_ELSE)) {
        advance(parser);  // consume 'else'
        else_body = parse_body(parser, &else_count);
    }
    
    // Expect 'end'
    if (!expect(parser, TOKEN_END, "Expected 'end' to close if statement")) {
        expression_free(condition);
        for (int i = 0; i < then_count; i++) statement_free(then_body[i]);
        free(then_body);
        for (int i = 0; i < else_count; i++) statement_free(else_body[i]);
        free(else_body);
        return NULL;
    }
    
    // Expect 'if' after 'end'
    if (match(parser, TOKEN_IF)) {
        advance(parser);  // consume 'if'
    }
    
    // Build if statement
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_IF;
    stmt->data.if_stmt.condition = condition;
    stmt->data.if_stmt.then_body = then_body;
    stmt->data.if_stmt.then_count = then_count;
    stmt->data.if_stmt.else_body = else_body;
    stmt->data.if_stmt.else_count = else_count;
    
    return stmt;
}

// Parse while loop
// Syntax: while condition ... end while
Statement* parse_while_statement(ControlFlowParser* parser) {
    if (!expect(parser, TOKEN_WHILE, "Expected 'while' keyword")) {
        return NULL;
    }
    
    // Parse condition
    Expression* condition = parse_condition(parser);
    if (!condition) {
        fprintf(stderr, "Error: Failed to parse while condition\n");
        return NULL;
    }
    
    // Parse body
    int body_count = 0;
    Statement** body = parse_body(parser, &body_count);
    
    // Expect 'end'
    if (!expect(parser, TOKEN_END, "Expected 'end' to close while loop")) {
        expression_free(condition);
        for (int i = 0; i < body_count; i++) statement_free(body[i]);
        free(body);
        return NULL;
    }
    
    // Expect 'while' after 'end'
    if (match(parser, TOKEN_WHILE)) {
        advance(parser);  // consume 'while'
    }
    
    // Build while statement
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_WHILE;
    stmt->data.while_stmt.condition = condition;
    stmt->data.while_stmt.body = body;
    stmt->data.while_stmt.body_count = body_count;
    
    return stmt;
}

// Parse for loop
// Syntax: for variable = start to end ... end for
Statement* parse_for_statement(ControlFlowParser* parser) {
    if (!expect(parser, TOKEN_FOR, "Expected 'for' keyword")) {
        return NULL;
    }
    
    // Parse loop variable
    if (!match(parser, TOKEN_IDENTIFIER)) {
        fprintf(stderr, "Error: Expected variable name in for loop\n");
        return NULL;
    }
    char* variable = strdup(parser->current_token->value);
    advance(parser);
    
    // Expect '='
    if (!expect(parser, TOKEN_ASSIGN, "Expected '=' after for variable")) {
        free(variable);
        return NULL;
    }
    
    // Parse start expression
    Parser temp_parser;
    temp_parser.lexer = parser->lexer;
    temp_parser.current_token = parser->current_token;
    Expression* start = expression_parse(&temp_parser);
    parser->current_token = temp_parser.current_token;
    
    if (!start) {
        fprintf(stderr, "Error: Failed to parse for loop start value\n");
        free(variable);
        return NULL;
    }
    
    // Expect 'to'
    if (!expect(parser, TOKEN_TO, "Expected 'to' in for loop")) {
        free(variable);
        expression_free(start);
        return NULL;
    }
    
    // Parse end expression
    temp_parser.lexer = parser->lexer;
    temp_parser.current_token = parser->current_token;
    Expression* end = expression_parse(&temp_parser);
    parser->current_token = temp_parser.current_token;
    
    if (!end) {
        fprintf(stderr, "Error: Failed to parse for loop end value\n");
        free(variable);
        expression_free(start);
        return NULL;
    }
    
    // Parse body
    int body_count = 0;
    Statement** body = parse_body(parser, &body_count);
    
    // Expect 'end'
    if (!expect(parser, TOKEN_END, "Expected 'end' to close for loop")) {
        free(variable);
        expression_free(start);
        expression_free(end);
        for (int i = 0; i < body_count; i++) statement_free(body[i]);
        free(body);
        return NULL;
    }
    
    // Expect 'for' after 'end'
    if (match(parser, TOKEN_FOR)) {
        advance(parser);  // consume 'for'
    }
    
    // Build for statement
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_FOR;
    stmt->data.for_stmt.variable = variable;
    stmt->data.for_stmt.start = start;
    stmt->data.for_stmt.end = end;
    stmt->data.for_stmt.body = body;
    stmt->data.for_stmt.body_count = body_count;
    
    return stmt;
}
