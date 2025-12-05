#ifndef LOGICAL_PARSER_H
#define LOGICAL_PARSER_H

#include "logical.h"
#include "../../lexer.h"

// Logical parser structure
typedef struct {
    Lexer* lexer;
    Token* current_token;
} LogicalParser;

// Create/free parser
LogicalParser* logical_parser_create(Lexer* lexer);
void logical_parser_free(LogicalParser* parser);

// Parse logical expression
LogicalExpr* logical_parse_expression(LogicalParser* parser);
LogicalExpr* logical_parse_or(LogicalParser* parser);
LogicalExpr* logical_parse_and(LogicalParser* parser);
LogicalExpr* logical_parse_not(LogicalParser* parser);
LogicalExpr* logical_parse_primary(LogicalParser* parser);

#endif // LOGICAL_PARSER_H
