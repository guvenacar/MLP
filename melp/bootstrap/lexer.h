// MELP Bootstrap Compiler - Lexer Header

#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TK_EOF,
    TK_KEYWORD,
    TK_IDENTIFIER,
    TK_NUMBER,
    TK_STRING,
    TK_OPERATOR,
    TK_SYMBOL,
    TK_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char value[256];
    int line;
    int col;
} Token;

void lexer_init(const char *source);
Token next_token();
const char* token_type_name(TokenType type);

#endif
