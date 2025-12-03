#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum {
    VAR_NUMERIC,
    VAR_TEXT,
    VAR_BOOLEAN
} VarType;

typedef struct {
    VarType type;
    char* name;
    char* value;  // string representation
} Declaration;

typedef struct {
    Declaration** declarations;
    int count;
    int capacity;
} Program;

typedef struct {
    Lexer* lexer;
    Token* current_token;
} Parser;

Parser* parser_create(Lexer* lexer);
void parser_free(Parser* parser);
Program* parser_parse(Parser* parser);
void program_free(Program* program);

#endif
