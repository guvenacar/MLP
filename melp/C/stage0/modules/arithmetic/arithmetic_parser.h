#ifndef ARITHMETIC_PARSER_H
#define ARITHMETIC_PARSER_H

#include "arithmetic.h"
#include "../../lexer.h"

// Arithmetic parser structure
typedef struct {
    Lexer* lexer;
    Token* current_token;
} ArithmeticParser;

// Create/free parser
ArithmeticParser* arithmetic_parser_create(Lexer* lexer);
void arithmetic_parser_free(ArithmeticParser* parser);

// Parse arithmetic expression
ArithmeticExpr* arithmetic_parse_expression(ArithmeticParser* parser);

// Parse binary operation (handles precedence)
ArithmeticExpr* arithmetic_parse_term(ArithmeticParser* parser);        // * / %
ArithmeticExpr* arithmetic_parse_factor(ArithmeticParser* parser);      // + -
ArithmeticExpr* arithmetic_parse_primary(ArithmeticParser* parser);     // numbers, variables

// Parse assignment: varname = expression
// Returns: variable name (caller must free), fills expr pointer
char* arithmetic_parse_assignment(ArithmeticParser* parser, ArithmeticExpr** expr);

#endif
