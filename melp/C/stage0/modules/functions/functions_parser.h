#ifndef FUNCTIONS_PARSER_H
#define FUNCTIONS_PARSER_H

#include "../../lexer.h"
#include "functions.h"

// Forward declarations
typedef struct Expression Expression;
typedef struct Statement Statement;

// Parse function declaration: def func_name(param1, param2) { ... }
FunctionDeclaration* parse_function_declaration(Lexer* lexer);

// Parse function call: func_name(arg1, arg2)
FunctionCall* parse_function_call(Lexer* lexer, const char* func_name);

// Parse return statement: return expr
ReturnStatement* parse_return_statement(Lexer* lexer);

#endif
