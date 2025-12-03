#ifndef VARIABLE_PARSER_H
#define VARIABLE_PARSER_H

#include "../../lexer.h"
#include "variable.h"

// Variable parser structure
typedef struct {
    Lexer* lexer;
    Token* current_token;
} VariableParser;

// Create/free parser
VariableParser* variable_parser_create(Lexer* lexer);
void variable_parser_free(VariableParser* parser);

// Parse variable declaration: numeric x = 10
VariableDeclaration* variable_parse_declaration(VariableParser* parser);

// Parse variable assignment: x = 20
VariableAssignment* variable_parse_assignment(VariableParser* parser);

// Free structures
void variable_declaration_free(VariableDeclaration* decl);
void variable_assignment_free(VariableAssignment* assign);

#endif
