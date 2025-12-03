#ifndef FUNCTION_PARSER_H
#define FUNCTION_PARSER_H

#include "function.h"
#include "../../lexer.h"

// Parser for function declarations and calls
typedef struct {
    Lexer* lexer;
    Token* current_token;
} FunctionParser;

// Parser functions
FunctionParser* func_parser_create(Lexer* lexer);
void func_parser_free(FunctionParser* parser);

// Parse function declaration
// Syntax: function name(type param1, type param2) ... return expr ... end function
Function* parse_function_declaration(FunctionParser* parser);

// Parse function call
// Syntax: function_name(arg1, arg2, ...)
FunctionCall* parse_function_call(FunctionParser* parser);

// Free functions
void function_free(Function* func);
void function_call_free(FunctionCall* call);

#endif
