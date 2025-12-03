#include "function_parser.h"
#include <stdlib.h>
#include <stdio.h>

FunctionParser* func_parser_create(Lexer* lexer) {
    FunctionParser* parser = malloc(sizeof(FunctionParser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

void func_parser_free(FunctionParser* parser) {
    if (parser->current_token) token_free(parser->current_token);
    free(parser);
}

void function_free(Function* func) {
    if (!func) return;
    
    // Free name
    if (func->name) free(func->name);
    
    // Free parameters
    for (int i = 0; i < func->param_count; i++) {
        if (func->parameters[i]->name) free(func->parameters[i]->name);
        free(func->parameters[i]);
    }
    if (func->parameters) free(func->parameters);
    
    // TODO: Free body statements
    if (func->body) free(func->body);
    
    free(func);
}

void function_call_free(FunctionCall* call) {
    if (!call) return;
    
    if (call->function_name) free(call->function_name);
    
    // TODO: Free argument expressions
    if (call->arguments) free(call->arguments);
    
    free(call);
}

// Placeholder: Parse function declaration
Function* parse_function_declaration(FunctionParser* parser) {
    // TODO: Implement in Stage 1
    // Expected syntax: function name(type param1, type param2) ... return expr ... end function
    fprintf(stderr, "TODO: parse_function_declaration not implemented\n");
    return NULL;
}

// Placeholder: Parse function call
FunctionCall* parse_function_call(FunctionParser* parser) {
    // TODO: Implement in Stage 1
    // Expected syntax: function_name(arg1, arg2, ...)
    fprintf(stderr, "TODO: parse_function_call not implemented\n");
    return NULL;
}
