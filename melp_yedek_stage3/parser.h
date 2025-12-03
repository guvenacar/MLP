#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "modules/variable/variable.h"

typedef struct {
    VarType type;
    char* name;
    char* value;
    // Smart storage management
    InternalNumericType internal_num_type;
    InternalStringType internal_str_type;
    StorageLocation storage;
    int has_decimal_point;  // For detecting floats
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
