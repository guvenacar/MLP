#include "lambda_parser.h"
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

// Forward declarations
typedef struct Expression Expression;
typedef struct Statement Statement;
extern Expression* expression_parse(Parser* parser);
extern Statement* parse_statement(Parser* parser);

// Parse capture list: [x, &y, z]
CapturedVar* parse_capture_list_internal(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_LBRACKET) {
        return NULL;
    }
    advance(parser);  // consume '['
    
    CapturedVar* head = NULL;
    CapturedVar* tail = NULL;
    
    while (parser->current_token && parser->current_token->type != TOKEN_RBRACKET) {
        CapturedVar* var = (CapturedVar*)malloc(sizeof(CapturedVar));
        var->next = NULL;
        var->offset = 0;
        
        // Check for reference capture (&)
        if (parser->current_token->type == TOKEN_AND) {
            var->mode = CAPTURE_BY_REF;
            advance(parser);
        } else {
            var->mode = CAPTURE_BY_VALUE;
        }
        
        // Parse variable name
        if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected variable name in capture list\n");
            free(var);
            // Free already parsed captures
            while (head) {
                CapturedVar* temp = head;
                head = head->next;
                free(temp->name);
                free(temp);
            }
            return NULL;
        }
        
        var->name = strdup(parser->current_token->value);
        advance(parser);
        
        // Add to list
        if (!head) {
            head = tail = var;
        } else {
            tail->next = var;
            tail = var;
        }
        
        // Check for comma
        if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
            advance(parser);
        }
    }
    
    if (!expect(parser, TOKEN_RBRACKET, "Expected ']' after capture list")) {
        // Free captures
        while (head) {
            CapturedVar* temp = head;
            head = head->next;
            free(temp->name);
            free(temp);
        }
        return NULL;
    }
    
    return head;
}

// Parse lambda parameters: (x, y)
static char** parse_lambda_parameters(Parser* parser, int* param_count) {
    *param_count = 0;
    int capacity = 4;
    char** params = (char**)malloc(sizeof(char*) * capacity);
    
    if (!parser->current_token || parser->current_token->type != TOKEN_LPAREN) {
        return NULL;
    }
    
    advance(parser);  // consume '('
    
    // Empty parameter list
    if (parser->current_token && parser->current_token->type == TOKEN_RPAREN) {
        advance(parser);
        return params;
    }
    
    // Parse parameters
    while (parser->current_token) {
        if (parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected parameter name\n");
            for (int i = 0; i < *param_count; i++) {
                free(params[i]);
            }
            free(params);
            return NULL;
        }
        
        if (*param_count >= capacity) {
            capacity *= 2;
            params = (char**)realloc(params, sizeof(char*) * capacity);
        }
        
        params[(*param_count)++] = strdup(parser->current_token->value);
        advance(parser);
        
        // Check for comma or closing paren
        if (parser->current_token->type == TOKEN_RPAREN) {
            break;
        } else if (parser->current_token->type == TOKEN_COMMA) {
            advance(parser);
        } else {
            fprintf(stderr, "Error: Expected ',' or ')' in parameters\n");
            for (int i = 0; i < *param_count; i++) {
                free(params[i]);
            }
            free(params);
            return NULL;
        }
    }
    
    if (!expect(parser, TOKEN_RPAREN, "Expected ')' after parameters")) {
        for (int i = 0; i < *param_count; i++) {
            free(params[i]);
        }
        free(params);
        return NULL;
    }
    
    return params;
}

// Parse lambda body: => expression
static void* parse_lambda_body(Parser* parser) {
    // Expect => (as identifier or two tokens: - and >)
    // For now, just look for identifier "=>"
    if (parser->current_token && 
        parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, "=>") == 0) {
        advance(parser);  // consume '=>'
    } else {
        fprintf(stderr, "Error: Expected '=>' after lambda parameters\n");
        return NULL;
    }
    
    // Parse single expression (simplified for Stage 1)
    Expression* body_expr = expression_parse(parser);
    return body_expr;
}

// Parse lambda: lambda x => x * 2  or  [x, y](a, b) => x + a + b
Lambda* parse_lambda_expression(Parser* parser) {
    if (!parser->current_token) {
        return NULL;
    }
    
    Lambda* lambda = lambda_create(LAMBDA_SIMPLE, 0);
    
    // Check for "lambda" keyword
    if (parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, "lambda") == 0) {
        advance(parser);
    }
    
    // Check for capture list [x, y]
    if (parser->current_token && parser->current_token->type == TOKEN_LBRACKET) {
        lambda->type = LAMBDA_CLOSURE;
        lambda->captures = parse_capture_list_internal(parser);
        if (!lambda->captures) {
            lambda_free(lambda);
            return NULL;
        }
    }
    
    // Parse parameters: (x, y) or just x
    if (parser->current_token && parser->current_token->type == TOKEN_LPAREN) {
        lambda->parameters = parse_lambda_parameters(parser, &lambda->param_count);
        if (!lambda->parameters) {
            lambda_free(lambda);
            return NULL;
        }
    } else if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
        // Single parameter without parentheses: lambda x => x * 2
        lambda->param_count = 1;
        lambda->parameters = (char**)malloc(sizeof(char*));
        lambda->parameters[0] = strdup(parser->current_token->value);
        advance(parser);
    } else {
        fprintf(stderr, "Error: Expected lambda parameters\n");
        lambda_free(lambda);
        return NULL;
    }
    
    // Parse lambda body
    lambda->body = parse_lambda_body(parser);
    if (!lambda->body) {
        lambda_free(lambda);
        return NULL;
    }
    
    return lambda;
}

// Parse lambda call: my_lambda(1, 2, 3) - internal version
static LambdaCall* parse_lambda_call_internal(Parser* parser, Lambda* lambda) {
    if (!parser->current_token || parser->current_token->type != TOKEN_LPAREN) {
        return NULL;
    }
    
    LambdaCall* call = lambda_create_call(lambda, 0);
    advance(parser);  // consume '('
    
    // Empty argument list
    if (parser->current_token && parser->current_token->type == TOKEN_RPAREN) {
        advance(parser);
        return call;
    }
    
    // Parse arguments
    int capacity = 4;
    call->arguments = (void**)malloc(sizeof(void*) * capacity);
    
    while (parser->current_token) {
        Expression* arg = expression_parse(parser);
        if (!arg) {
            fprintf(stderr, "Error: Failed to parse lambda call argument\n");
            lambda_call_free(call);
            return NULL;
        }
        
        if (call->arg_count >= capacity) {
            capacity *= 2;
            call->arguments = (void**)realloc(call->arguments, sizeof(void*) * capacity);
        }
        
        call->arguments[call->arg_count++] = arg;
        
        if (parser->current_token->type == TOKEN_RPAREN) {
            break;
        } else if (parser->current_token->type == TOKEN_COMMA) {
            advance(parser);
        } else {
            fprintf(stderr, "Error: Expected ',' or ')' in lambda call\n");
            lambda_call_free(call);
            return NULL;
        }
    }
    
    if (!expect(parser, TOKEN_RPAREN, "Expected ')' after lambda arguments")) {
        lambda_call_free(call);
        return NULL;
    }
    
    return call;
}

// Parse higher-order function: map(array, lambda), filter(list, predicate)
HigherOrder* parse_higher_order_function(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    // Check for known higher-order functions
    const char* func_name = parser->current_token->value;
    if (strcmp(func_name, "map") != 0 && 
        strcmp(func_name, "filter") != 0 && 
        strcmp(func_name, "reduce") != 0 &&
        strcmp(func_name, "forEach") != 0) {
        return NULL;
    }
    
    advance(parser);  // consume function name
    
    if (!expect(parser, TOKEN_LPAREN, "Expected '(' after higher-order function name")) {
        return NULL;
    }
    
    // Parse collection (first argument)
    Expression* collection = expression_parse(parser);
    if (!collection) {
        fprintf(stderr, "Error: Failed to parse collection argument\n");
        return NULL;
    }
    
    if (!expect(parser, TOKEN_COMMA, "Expected ',' after collection")) {
        return NULL;
    }
    
    // Parse lambda (second argument)
    Lambda* lambda = parse_lambda_expression(parser);
    if (!lambda) {
        fprintf(stderr, "Error: Failed to parse lambda in higher-order function\n");
        return NULL;
    }
    
    if (!expect(parser, TOKEN_RPAREN, "Expected ')' after lambda")) {
        lambda_free(lambda);
        return NULL;
    }
    
    HigherOrder* ho = higher_order_create(func_name, lambda);
    ho->collection = collection;
    
    return ho;
}

// Legacy compatibility wrappers for old API
Lambda* parse_lambda(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_lambda_expression(&parser);
}

LambdaCall* parse_lambda_call(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    // Note: This needs a lambda reference, simplified version
    return NULL;
}

HigherOrder* parse_higher_order(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_higher_order_function(&parser);
}

CapturedVar* parse_capture_list(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_capture_list_internal(&parser);
}
