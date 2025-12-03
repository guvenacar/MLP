#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Lexer* lexer_create(const char* source) {
    Lexer* lexer = malloc(sizeof(Lexer));
    lexer->source = source;
    lexer->pos = 0;
    lexer->line = 1;
    return lexer;
}

void lexer_free(Lexer* lexer) {
    free(lexer);
}

void token_free(Token* token) {
    if (token->value) free(token->value);
    free(token);
}

static void skip_whitespace(Lexer* lexer) {
    while (lexer->source[lexer->pos] == ' ' || 
           lexer->source[lexer->pos] == '\t' ||
           lexer->source[lexer->pos] == '\r') {
        lexer->pos++;
    }
    if (lexer->source[lexer->pos] == '\n') {
        lexer->line++;
        lexer->pos++;
        skip_whitespace(lexer);
    }
}

static void skip_comment(Lexer* lexer) {
    // Single line comment: -- comment text
    if (lexer->source[lexer->pos] == '-' && lexer->source[lexer->pos + 1] == '-') {
        // Skip until end of line
        while (lexer->source[lexer->pos] != '\n' && lexer->source[lexer->pos] != '\0') {
            lexer->pos++;
        }
        // Don't skip the newline, let skip_whitespace handle it
    }
}

static Token* make_token(TokenType type, const char* value, int line) {
    Token* token = malloc(sizeof(Token));
    token->type = type;
    token->value = value ? strdup(value) : NULL;
    token->line = line;
    return token;
}

static Token* read_number(Lexer* lexer) {
    int start = lexer->pos;
    while (isdigit(lexer->source[lexer->pos])) {
        lexer->pos++;
    }
    int len = lexer->pos - start;
    char* value = malloc(len + 1);
    strncpy(value, &lexer->source[start], len);
    value[len] = '\0';
    Token* token = make_token(TOKEN_NUMBER, value, lexer->line);
    free(value);
    return token;
}

static Token* read_string(Lexer* lexer) {
    lexer->pos++; // skip opening "
    int start = lexer->pos;
    while (lexer->source[lexer->pos] != '"' && lexer->source[lexer->pos] != '\0') {
        lexer->pos++;
    }
    int len = lexer->pos - start;
    char* value = malloc(len + 1);
    strncpy(value, &lexer->source[start], len);
    value[len] = '\0';
    lexer->pos++; // skip closing "
    Token* token = make_token(TOKEN_STRING, value, lexer->line);
    free(value);
    return token;
}

static Token* read_identifier(Lexer* lexer) {
    int start = lexer->pos;
    while (isalnum(lexer->source[lexer->pos]) || lexer->source[lexer->pos] == '_') {
        lexer->pos++;
    }
    int len = lexer->pos - start;
    char* value = malloc(len + 1);
    strncpy(value, &lexer->source[start], len);
    value[len] = '\0';
    
    // Check keywords
    TokenType type = TOKEN_IDENTIFIER;
    if (strcmp(value, "numeric") == 0) type = TOKEN_NUMERIC;
    else if (strcmp(value, "text") == 0) type = TOKEN_TEXT;
    else if (strcmp(value, "string") == 0) type = TOKEN_TEXT;  // 'string' is alias for 'text'
    else if (strcmp(value, "boolean") == 0) type = TOKEN_BOOLEAN;
    else if (strcmp(value, "true") == 0) type = TOKEN_TRUE;
    else if (strcmp(value, "false") == 0) type = TOKEN_FALSE;
    // Control flow keywords
    else if (strcmp(value, "if") == 0) type = TOKEN_IF;
    else if (strcmp(value, "then") == 0) type = TOKEN_THEN;
    else if (strcmp(value, "else") == 0) type = TOKEN_ELSE;
    else if (strcmp(value, "end") == 0) type = TOKEN_END;
    else if (strcmp(value, "while") == 0) type = TOKEN_WHILE;
    else if (strcmp(value, "do") == 0) type = TOKEN_DO;
    else if (strcmp(value, "for") == 0) type = TOKEN_FOR;
    else if (strcmp(value, "to") == 0) type = TOKEN_TO;
    else if (strcmp(value, "exit") == 0) type = TOKEN_EXIT;
    // Function keywords
    else if (strcmp(value, "function") == 0) type = TOKEN_FUNCTION;
    else if (strcmp(value, "return") == 0) type = TOKEN_RETURN;
    
    Token* token = make_token(type, value, lexer->line);
    free(value);
    return token;
}

Token* lexer_next_token(Lexer* lexer) {
    while (1) {
        skip_whitespace(lexer);
        
        // Check for comment
        if (lexer->source[lexer->pos] == '-' && lexer->source[lexer->pos + 1] == '-') {
            skip_comment(lexer);
            continue;  // Re-check for more whitespace/comments
        }
        break;
    }
    
    if (lexer->source[lexer->pos] == '\0') {
        return make_token(TOKEN_EOF, NULL, lexer->line);
    }
    
    char c = lexer->source[lexer->pos];
    
    // Two-character operators (must check before single-char)
    if (c == '=' && lexer->source[lexer->pos + 1] == '=') {
        lexer->pos += 2;
        return make_token(TOKEN_EQUAL, "==", lexer->line);
    }
    
    if (c == '!' && lexer->source[lexer->pos + 1] == '=') {
        lexer->pos += 2;
        return make_token(TOKEN_NOT_EQUAL, "!=", lexer->line);
    }
    
    if (c == '<' && lexer->source[lexer->pos + 1] == '=') {
        lexer->pos += 2;
        return make_token(TOKEN_LESS_EQUAL, "<=", lexer->line);
    }
    
    if (c == '>' && lexer->source[lexer->pos + 1] == '=') {
        lexer->pos += 2;
        return make_token(TOKEN_GREATER_EQUAL, ">=", lexer->line);
    }
    
    // Logical operators (two-character)
    if (c == '&' && lexer->source[lexer->pos + 1] == '&') {
        lexer->pos += 2;
        return make_token(TOKEN_AND, "&&", lexer->line);
    }
    
    if (c == '|' && lexer->source[lexer->pos + 1] == '|') {
        lexer->pos += 2;
        return make_token(TOKEN_OR, "||", lexer->line);
    }
    
    // Single-character operators
    if (c == '=') {
        lexer->pos++;
        return make_token(TOKEN_ASSIGN, "=", lexer->line);
    }
    
    if (c == '<') {
        lexer->pos++;
        return make_token(TOKEN_LESS, "<", lexer->line);
    }
    
    if (c == '>') {
        lexer->pos++;
        return make_token(TOKEN_GREATER, ">", lexer->line);
    }
    
    // Arithmetic operators
    if (c == '+') {
        lexer->pos++;
        return make_token(TOKEN_PLUS, "+", lexer->line);
    }
    
    if (c == '-') {
        lexer->pos++;
        return make_token(TOKEN_MINUS, "-", lexer->line);
    }
    
    if (c == '*') {
        lexer->pos++;
        return make_token(TOKEN_MULTIPLY, "*", lexer->line);
    }
    
    if (c == '/') {
        lexer->pos++;
        return make_token(TOKEN_DIVIDE, "/", lexer->line);
    }
    
    // Logical NOT (single-character, checked after !=)
    if (c == '!') {
        lexer->pos++;
        return make_token(TOKEN_NOT, "!", lexer->line);
    }
    
    // Delimiters
    if (c == '(') {
        lexer->pos++;
        return make_token(TOKEN_LPAREN, "(", lexer->line);
    }
    
    if (c == ')') {
        lexer->pos++;
        return make_token(TOKEN_RPAREN, ")", lexer->line);
    }
    
    if (c == ',') {
        lexer->pos++;
        return make_token(TOKEN_COMMA, ",", lexer->line);
    }
    
    if (c == ';') {
        lexer->pos++;
        return make_token(TOKEN_SEMICOLON, ";", lexer->line);
    }
    
    if (c == '[') {
        lexer->pos++;
        return make_token(TOKEN_LBRACKET, "[", lexer->line);
    }
    
    if (c == ']') {
        lexer->pos++;
        return make_token(TOKEN_RBRACKET, "]", lexer->line);
    }
    
    if (c == '.') {
        lexer->pos++;
        return make_token(TOKEN_DOT, ".", lexer->line);
    }
    
    if (isdigit(c)) {
        return read_number(lexer);
    }
    
    if (c == '"') {
        return read_string(lexer);
    }
    
    if (isalpha(c) || c == '_') {
        return read_identifier(lexer);
    }
    
    return make_token(TOKEN_ERROR, NULL, lexer->line);
}
