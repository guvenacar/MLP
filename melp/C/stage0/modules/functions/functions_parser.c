#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "functions_parser.h"
#include "functions.h"
#include "../../lexer.h"

// Helper: Convert type keyword to FunctionParamType
static FunctionParamType token_to_param_type(TokenType type) {
    switch (type) {
        case TOKEN_NUMERIC: return FUNC_PARAM_NUMERIC;
        case TOKEN_TEXT: return FUNC_PARAM_TEXT;
        case TOKEN_BOOLEAN: return FUNC_PARAM_BOOLEAN;
        default: return FUNC_PARAM_NUMERIC;
    }
}

// Helper: Convert type keyword to FunctionReturnType
static FunctionReturnType token_to_return_type(TokenType type) {
    switch (type) {
        case TOKEN_NUMERIC: return FUNC_RETURN_NUMERIC;
        case TOKEN_TEXT: return FUNC_RETURN_TEXT;
        case TOKEN_BOOLEAN: return FUNC_RETURN_BOOLEAN;
        default: return FUNC_RETURN_VOID;
    }
}

// Parse function declaration
// Syntax: def func_name(param1: numeric, param2: text) -> numeric { ... }
FunctionDeclaration* parse_function_declaration(Lexer* lexer) {
    Token* tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_FUNCTION) {
        fprintf(stderr, "Error: Expected 'function' keyword\n");
        token_free(tok);
        return NULL;
    }
    token_free(tok);
    
    // Function name
    tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected function name\n");
        token_free(tok);
        return NULL;
    }
    
    char* func_name = strdup(tok->value);
    token_free(tok);
    
    // Left paren
    tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_LPAREN) {
        fprintf(stderr, "Error: Expected '(' after function name\n");
        token_free(tok);
        free(func_name);
        return NULL;
    }
    token_free(tok);
    
    // Create function with default return type (will be updated if return type specified)
    FunctionDeclaration* func = function_create(func_name, FUNC_RETURN_VOID);
    free(func_name);
    
    // Parse parameters
    tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_RPAREN) {
        // First parameter
        if (tok->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected parameter name\n");
            token_free(tok);
            function_free(func);
            return NULL;
        }
        
        char* param_name = strdup(tok->value);
        token_free(tok);
        
        // Optional type annotation: : numeric
        tok = lexer_next_token(lexer);
        FunctionParamType param_type = FUNC_PARAM_NUMERIC;  // Default
        
        if (tok->type == TOKEN_ASSIGN) {  // Using : but lexer might see =
            token_free(tok);
            tok = lexer_next_token(lexer);
            param_type = token_to_param_type(tok->type);
            token_free(tok);
            tok = lexer_next_token(lexer);
        }
        
        function_add_param(func, param_name, param_type);
        free(param_name);
        
        // Additional parameters
        while (tok->type == TOKEN_COMMA) {
            token_free(tok);
            
            tok = lexer_next_token(lexer);
            if (tok->type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Error: Expected parameter name after ','\n");
                token_free(tok);
                function_free(func);
                return NULL;
            }
            
            param_name = strdup(tok->value);
            token_free(tok);
            
            tok = lexer_next_token(lexer);
            param_type = FUNC_PARAM_NUMERIC;  // Default
            
            if (tok->type == TOKEN_ASSIGN) {
                token_free(tok);
                tok = lexer_next_token(lexer);
                param_type = token_to_param_type(tok->type);
                token_free(tok);
                tok = lexer_next_token(lexer);
            }
            
            function_add_param(func, param_name, param_type);
            free(param_name);
        }
    }
    
    // Right paren
    if (tok->type != TOKEN_RPAREN) {
        fprintf(stderr, "Error: Expected ')' after parameters\n");
        token_free(tok);
        function_free(func);
        return NULL;
    }
    token_free(tok);
    
    // Optional return type: -> numeric
    tok = lexer_next_token(lexer);
    if (tok->type == TOKEN_MINUS) {
        token_free(tok);
        tok = lexer_next_token(lexer);
        if (tok->type == TOKEN_GREATER) {  // ->
            token_free(tok);
            tok = lexer_next_token(lexer);
            func->return_type = token_to_return_type(tok->type);
            token_free(tok);
            tok = lexer_next_token(lexer);
        }
    }
    
    // Function body (statements until 'end')
    // For now, we'll just skip to 'end' - actual statement parsing handled by statement parser
    int depth = 0;
    while (tok->type != TOKEN_EOF) {
        if (tok->type == TOKEN_IF || tok->type == TOKEN_WHILE || tok->type == TOKEN_FOR || tok->type == TOKEN_FUNCTION) {
            depth++;
        } else if (tok->type == TOKEN_END) {
            if (depth == 0) {
                token_free(tok);
                break;
            }
            depth--;
        }
        token_free(tok);
        tok = lexer_next_token(lexer);
    }
    
    return func;
}

// Parse function call: func_name(arg1, arg2)
FunctionCall* parse_function_call(Lexer* lexer, const char* func_name) {
    FunctionCall* call = function_call_create(func_name);
    
    Token* tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_LPAREN) {
        fprintf(stderr, "Error: Expected '(' after function name\n");
        token_free(tok);
        function_call_free(call);
        return NULL;
    }
    token_free(tok);
    
    // Parse arguments
    tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_RPAREN) {
        // For now, skip argument parsing - would need expression parser
        // Just count tokens until ')'
        int depth = 1;
        while (tok->type != TOKEN_EOF && depth > 0) {
            if (tok->type == TOKEN_LPAREN) depth++;
            else if (tok->type == TOKEN_RPAREN) depth--;
            token_free(tok);
            if (depth > 0) {
                tok = lexer_next_token(lexer);
            }
        }
    } else {
        token_free(tok);
    }
    
    return call;
}

// Parse return statement: return expr
ReturnStatement* parse_return_statement(Lexer* lexer) {
    Token* tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_RETURN) {
        fprintf(stderr, "Error: Expected 'return' keyword\n");
        token_free(tok);
        return NULL;
    }
    token_free(tok);
    
    // For now, skip expression parsing - would need expression parser
    // Just create empty return
    ReturnStatement* ret = return_statement_create(NULL);
    
    // Skip to semicolon or newline
    tok = lexer_next_token(lexer);
    while (tok->type != TOKEN_SEMICOLON && tok->type != TOKEN_EOF) {
        token_free(tok);
        tok = lexer_next_token(lexer);
    }
    token_free(tok);
    
    return ret;
}
