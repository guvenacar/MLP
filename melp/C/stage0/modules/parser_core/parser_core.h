#ifndef PARSER_CORE_H
#define PARSER_CORE_H

#include "../../lexer.h"

// Parser core structure - minimal, modular design
typedef struct {
    Lexer* lexer;
    Token* current_token;
} Parser;

// Forward declarations for expression/statement types
typedef struct Expression Expression;
typedef struct Statement Statement;

// Create and manage parser
Parser* parser_create(Lexer* lexer);
void parser_free(Parser* parser);

// Token management helpers
void parser_advance(Parser* parser);
Token* parser_peek(Parser* parser);
int parser_match(Parser* parser, TokenType type);
int parser_expect(Parser* parser, TokenType type, const char* error_msg);

#endif
