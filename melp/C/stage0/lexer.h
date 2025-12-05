#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_NUMERIC,
    TOKEN_TEXT,
    TOKEN_BOOLEAN,
    TOKEN_IDENTIFIER,
    TOKEN_ASSIGN,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_TRUE,
    TOKEN_FALSE,
    // Arithmetic operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MOD,             // %
    TOKEN_POWER,           // **
    TOKEN_XOR,             // xor (also ^) - bitwise XOR
    // Comparison operators
    TOKEN_EQUAL,           // ==
    TOKEN_NOT_EQUAL,       // !=
    TOKEN_LESS,            // <
    TOKEN_GREATER,         // >
    TOKEN_LESS_EQUAL,      // <=
    TOKEN_GREATER_EQUAL,   // >=
    // Logical operators
    TOKEN_AND,             // &&
    TOKEN_OR,              // ||
    TOKEN_NOT,             // !
    // Control flow keywords
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    TOKEN_END,
    TOKEN_WHILE,
    TOKEN_DO,
    TOKEN_FOR,
    TOKEN_TO,
    TOKEN_EXIT,
    TOKEN_CONTINUE,
    // Function keywords
    TOKEN_FUNCTION,
    TOKEN_RETURN,
    // Struct keywords
    TOKEN_STRUCT,
    // I/O keywords
    TOKEN_PRINT,
    // Delimiters
    TOKEN_LPAREN,          // (
    TOKEN_RPAREN,          // )
    TOKEN_LBRACKET,        // [
    TOKEN_RBRACKET,        // ]
    TOKEN_COMMA,           // ,
    TOKEN_SEMICOLON,       // ;
    TOKEN_DOT,             // .
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char* value;
    int line;
} Token;

typedef struct {
    const char* source;
    int pos;
    int line;
} Lexer;

Lexer* lexer_create(const char* source);
void lexer_free(Lexer* lexer);
Token* lexer_next_token(Lexer* lexer);
void token_free(Token* token);

#endif
