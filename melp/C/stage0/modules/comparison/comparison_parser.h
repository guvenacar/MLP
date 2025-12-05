#ifndef COMPARISON_PARSER_H
#define COMPARISON_PARSER_H

#include "comparison.h"
#include "../../lexer.h"

// Comparison parser structure
typedef struct {
    Lexer* lexer;
    Token* current_token;
} ComparisonParser;

// Create/free parser
ComparisonParser* comparison_parser_create(Lexer* lexer);
void comparison_parser_free(ComparisonParser* parser);

// Parse comparison expression
ComparisonExpr* comparison_parse_expression(ComparisonParser* parser);

#endif // COMPARISON_PARSER_H
