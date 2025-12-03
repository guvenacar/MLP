#include "function_parser.h"
#include "../../parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Helper functions
static void advance(Parser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    parser->current_token = lexer_next_token(parser->lexer);
}

static int match(Parser* parser, TokenType type) {
    if (parser->current_token && parser->current_token->type == type) {
        advance(parser);
        return 1;
    }
    return 0;
}

static int expect(Parser* parser, TokenType type, const char* message) {
    if (parser->current_token && parser->current_token->type == type) {
        advance(parser);
        return 1;
    }
    fprintf(stderr, "Parse error: %s\n", message);
    return 0;
}

// Parse VarType from token
static VarType parse_type(Parser* parser) {
    if (!parser->current_token) {
        return VAR_NUMERIC;
    }
    
    const char* type_str = parser->current_token->value;
    VarType type = VAR_NUMERIC;
    
    if (strcmp(type_str, "numeric") == 0) {
        type = VAR_NUMERIC;
    } else if (strcmp(type_str, "string") == 0) {
        type = VAR_STRING;
    } else if (strcmp(type_str, "boolean") == 0) {
        type = VAR_BOOLEAN;
    } else if (strcmp(type_str, "void") == 0) {
        type = VAR_NUMERIC;  // Use numeric for void temporarily
    }
    
    advance(parser);
    return type;
}

// Parse function parameter: type name
static Parameter* parse_parameter(Parser* parser) {
    // Parse parameter type first
    VarType type = parse_type(parser);
    
    // Then parse parameter name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected parameter name\n");
        return NULL;
    }
    
    Parameter* param = (Parameter*)malloc(sizeof(Parameter));
    param->name = strdup(parser->current_token->value);
    param->type = type;
    advance(parser);
    
    return param;
}

// Parse function parameters: (type1 param1, type2 param2, ...)
static Parameter** parse_parameters(Parser* parser, int* param_count) {
    *param_count = 0;
    int capacity = 4;
    Parameter** params = (Parameter**)malloc(sizeof(Parameter*) * capacity);
    
    if (!expect(parser, TOKEN_LPAREN, "Expected '(' after function name")) {
        free(params);
        return NULL;
    }
    
    // Empty parameter list
    if (parser->current_token && parser->current_token->type == TOKEN_RPAREN) {
        advance(parser);
        return params;
    }
    
    // Parse first parameter
    Parameter* param = parse_parameter(parser);
    if (!param) {
        free(params);
        return NULL;
    }
    params[(*param_count)++] = param;
    
    // Parse remaining parameters
    while (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
        advance(parser);  // consume comma
        
        param = parse_parameter(parser);
        if (!param) {
            // Free already parsed parameters
            for (int i = 0; i < *param_count; i++) {
                free(params[i]->name);
                free(params[i]);
            }
            free(params);
            return NULL;
        }
        
        // Expand array if needed
        if (*param_count >= capacity) {
            capacity *= 2;
            params = (Parameter**)realloc(params, sizeof(Parameter*) * capacity);
        }
        
        params[(*param_count)++] = param;
    }
    
    if (!expect(parser, TOKEN_RPAREN, "Expected ')' after parameters")) {
        for (int i = 0; i < *param_count; i++) {
            free(params[i]->name);
            free(params[i]);
        }
        free(params);
        return NULL;
    }
    
    return params;
}

// Forward declaration for statement parsing
typedef struct Statement Statement;
extern Statement* parse_statement(Parser* parser);

// Parse function body until "end function"
static Statement** parse_function_body(Parser* parser, int* body_count) {
    *body_count = 0;
    int capacity = 8;
    Statement** body = (Statement**)malloc(sizeof(Statement*) * capacity);
    
    while (parser->current_token) {
        // Check for "end"
        if (parser->current_token->type == TOKEN_IDENTIFIER &&
            strcmp(parser->current_token->value, "end") == 0) {
            break;
        }
        
        // Check for "return"
        if (parser->current_token->type == TOKEN_IDENTIFIER &&
            strcmp(parser->current_token->value, "return") == 0) {
            // Parse return statement and break
            Statement* stmt = parse_statement(parser);
            if (stmt) {
                if (*body_count >= capacity) {
                    capacity *= 2;
                    body = (Statement**)realloc(body, sizeof(Statement*) * capacity);
                }
                body[(*body_count)++] = stmt;
            }
            break;
        }
        
        Statement* stmt = parse_statement(parser);
        if (!stmt) {
            break;
        }
        
        if (*body_count >= capacity) {
            capacity *= 2;
            body = (Statement**)realloc(body, sizeof(Statement*) * capacity);
        }
        
        body[(*body_count)++] = stmt;
    }
    
    return body;
}

// Parse function definition: function name(params) returns type ... end function
Function* parse_function_definition(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "function") != 0) {
        return NULL;
    }
    advance(parser);  // consume "function"
    
    // Parse function name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected function name\n");
        return NULL;
    }
    
    Function* func = (Function*)malloc(sizeof(Function));
    func->name = strdup(parser->current_token->value);
    advance(parser);
    
    // Parse parameters
    func->parameters = parse_parameters(parser, &func->param_count);
    if (!func->parameters && func->param_count > 0) {
        free(func->name);
        free(func);
        return NULL;
    }
    
    // Parse return type (optional, default to numeric)
    func->return_type = VAR_NUMERIC;
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, "returns") == 0) {
        advance(parser);  // consume "returns"
        func->return_type = parse_type(parser);
    }
    
    // Parse function body
    func->body = parse_function_body(parser, &func->body_count);
    func->has_return = 0;
    
    // Check if last statement is return
    if (func->body_count > 0) {
        // Simplified check - in later stages, check Statement type
        func->has_return = 1;
    }
    
    // Expect "end function"
    if (!expect(parser, TOKEN_IDENTIFIER, "Expected 'end'") ||
        !expect(parser, TOKEN_IDENTIFIER, "Expected 'function'")) {
        free(func->name);
        for (int i = 0; i < func->param_count; i++) {
            free(func->parameters[i]->name);
            free(func->parameters[i]);
        }
        free(func->parameters);
        free(func->body);
        free(func);
        return NULL;
    }
    
    return func;
}

// Forward declaration for expression parsing
typedef struct Expression Expression;
extern Expression* expression_parse(Parser* parser);

// Parse function call: function_name(arg1, arg2, ...)
FunctionCall* parse_function_call(FunctionParser* func_parser) {
    Parser parser;
    parser.lexer = func_parser->lexer;
    parser.current_token = func_parser->current_token;
    
    if (!parser.current_token || parser.current_token->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    FunctionCall* call = (FunctionCall*)malloc(sizeof(FunctionCall));
    call->function_name = strdup(parser.current_token->value);
    call->argument_count = 0;
    advance(&parser);
    
    if (!expect(&parser, TOKEN_LPAREN, "Expected '(' after function name")) {
        free(call->function_name);
        free(call);
        return NULL;
    }
    
    // Empty argument list
    if (parser.current_token && parser.current_token->type == TOKEN_RPAREN) {
        advance(&parser);
        call->arguments = NULL;
        func_parser->current_token = parser.current_token;
        return call;
    }
    
    // Parse arguments
    int capacity = 4;
    call->arguments = (Expression**)malloc(sizeof(Expression*) * capacity);
    
    // Parse first argument
    Expression* arg = expression_parse(&parser);
    if (!arg) {
        fprintf(stderr, "Error: Failed to parse function argument\n");
        free(call->function_name);
        free(call->arguments);
        free(call);
        return NULL;
    }
    call->arguments[call->argument_count++] = arg;
    
    // Parse remaining arguments
    while (parser.current_token && parser.current_token->type == TOKEN_COMMA) {
        advance(&parser);  // consume comma
        
        arg = expression_parse(&parser);
        if (!arg) {
            fprintf(stderr, "Error: Failed to parse function argument\n");
            free(call->function_name);
            free(call->arguments);
            free(call);
            return NULL;
        }
        
        if (call->argument_count >= capacity) {
            capacity *= 2;
            call->arguments = (Expression**)realloc(call->arguments, 
                                                    sizeof(Expression*) * capacity);
        }
        
        call->arguments[call->argument_count++] = arg;
    }
    
    if (!expect(&parser, TOKEN_RPAREN, "Expected ')' after arguments")) {
        free(call->function_name);
        free(call->arguments);
        free(call);
        return NULL;
    }
    
    func_parser->current_token = parser.current_token;
    return call;
}

// Parse return statement: return [expression]
ReturnStatement* parse_return_statement(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "return") != 0) {
        return NULL;
    }
    advance(parser);  // consume "return"
    
    ReturnStatement* ret = (ReturnStatement*)malloc(sizeof(ReturnStatement));
    
    // Check for empty return (void function)
    if (!parser->current_token || 
        (parser->current_token->type == TOKEN_IDENTIFIER &&
         strcmp(parser->current_token->value, "end") == 0)) {
        ret->return_value = NULL;
        return ret;
    }
    
    // Parse return expression
    ret->return_value = expression_parse(parser);
    if (!ret->return_value) {
        fprintf(stderr, "Error: Failed to parse return expression\n");
        free(ret);
        return NULL;
    }
    
    return ret;
}

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

// Legacy function for compatibility
Function* parse_function_declaration(FunctionParser* parser) {
    // Convert FunctionParser to Parser for new implementation
    Parser p;
    p.lexer = parser->lexer;
    p.current_token = parser->current_token;
    return parse_function_definition(&p);
}
