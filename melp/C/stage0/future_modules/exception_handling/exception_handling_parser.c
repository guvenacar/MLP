#include "exception_handling_parser.h"
#include "../statement/statement_parser.h"
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
typedef struct Statement Statement;
extern Statement* parse_statement(Parser* parser);

// Parse block of statements until "end"
static void* parse_block(Parser* parser) {
    int capacity = 8;
    Statement** block = (Statement**)malloc(sizeof(Statement*) * capacity);
    int count = 0;
    
    while (parser->current_token) {
        // Check for block terminators
        if (parser->current_token->type == TOKEN_IDENTIFIER) {
            const char* val = parser->current_token->value;
            if (strcmp(val, "end") == 0 || 
                strcmp(val, "catch") == 0 || 
                strcmp(val, "finally") == 0) {
                break;
            }
        }
        
        Statement* stmt = parse_statement(parser);
        if (!stmt) {
            break;
        }
        
        if (count >= capacity) {
            capacity *= 2;
            block = (Statement**)realloc(block, sizeof(Statement*) * capacity);
        }
        
        block[count++] = stmt;
    }
    
    return block;
}

// Parse catch block: catch (ExceptionType var_name) ... end catch
CatchBlock* parse_catch_block(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "catch") != 0) {
        return NULL;
    }
    advance(parser);  // consume "catch"
    
    CatchBlock* cb = NULL;
    
    // Check for parenthesized exception type and variable
    if (parser->current_token && parser->current_token->type == TOKEN_LPAREN) {
        advance(parser);
        
        // Parse exception type
        if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected exception type after '('\n");
            return NULL;
        }
        
        char* exc_type = strdup(parser->current_token->value);
        advance(parser);
        
        // Parse variable name
        char* var_name = NULL;
        if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
            var_name = strdup(parser->current_token->value);
            advance(parser);
        }
        
        if (!expect(parser, TOKEN_RPAREN, "Expected ')' after exception declaration")) {
            free(exc_type);
            if (var_name) free(var_name);
            return NULL;
        }
        
        cb = catch_block_create(exc_type, var_name);
        free(exc_type);
        if (var_name) free(var_name);
    } else {
        // Catch-all block (no parentheses)
        cb = catch_block_create("*", NULL);
    }
    
    // Parse catch handler body
    cb->handler = parse_block(parser);
    
    return cb;
}

// Parse try-catch-finally statement
TryCatchFinally* parse_try_catch_finally_statement(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "try") != 0) {
        return NULL;
    }
    advance(parser);  // consume "try"
    
    TryCatchFinally* tcf = try_catch_finally_create();
    
    // Parse try block
    tcf->try_block = parse_block(parser);
    
    // Parse catch blocks (can be multiple)
    while (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
           strcmp(parser->current_token->value, "catch") == 0) {
        
        CatchBlock* cb = parse_catch_block(parser);
        if (!cb) {
            try_catch_finally_free(tcf);
            return NULL;
        }
        
        try_catch_add_catch(tcf, cb);
        
        // Check for "end catch"
        if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
            strcmp(parser->current_token->value, "end") == 0) {
            advance(parser);
            if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
                strcmp(parser->current_token->value, "catch") == 0) {
                advance(parser);
            }
        }
    }
    
    // Parse finally block (optional)
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, "finally") == 0) {
        advance(parser);
        
        tcf->finally_block = parse_block(parser);
        
        // Expect "end finally"
        if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
            strcmp(parser->current_token->value, "end") == 0) {
            advance(parser);
            if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
                strcmp(parser->current_token->value, "finally") == 0) {
                advance(parser);
            }
        }
    }
    
    // Expect "end try"
    if (!expect(parser, TOKEN_IDENTIFIER, "Expected 'end'")) {
        try_catch_finally_free(tcf);
        return NULL;
    }
    
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, "try") == 0) {
        advance(parser);
    }
    
    return tcf;
}

// Parse throw statement: throw ExceptionType("message") or throw variable
Throw* parse_throw_statement(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "throw") != 0) {
        return NULL;
    }
    advance(parser);  // consume "throw"
    
    Exception* exc = NULL;
    
    // Check if it's a new exception or a variable
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
        char* exc_type_or_var = strdup(parser->current_token->value);
        advance(parser);
        
        // Check for constructor call: ExceptionType("message")
        if (parser->current_token && parser->current_token->type == TOKEN_LPAREN) {
            advance(parser);
            
            // Parse exception message (simplified - assume string literal or identifier)
            char* message = NULL;
            if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
                message = strdup(parser->current_token->value);
                advance(parser);
            } else if (parser->current_token) {
                message = strdup(parser->current_token->value);
                advance(parser);
            }
            
            if (!expect(parser, TOKEN_RPAREN, "Expected ')' after exception message")) {
                free(exc_type_or_var);
                if (message) free(message);
                return NULL;
            }
            
            exc = exception_create(exc_type_or_var, message ? message : "");
            free(exc_type_or_var);
            if (message) free(message);
        } else {
            // It's a variable reference - create generic exception
            exc = exception_create("Exception", exc_type_or_var);
            free(exc_type_or_var);
        }
    } else {
        fprintf(stderr, "Error: Expected exception type or variable after 'throw'\n");
        return NULL;
    }
    
    Throw* thr = throw_create(exc);
    return thr;
}

// Parse exception type declaration (for type annotations)
Exception* parse_exception_type_declaration(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    // Parse exception type name
    char* type_name = strdup(parser->current_token->value);
    advance(parser);
    
    // Create exception with just the type (no message yet)
    Exception* exc = exception_create(type_name, "");
    free(type_name);
    
    return exc;
}

// Legacy compatibility wrappers
TryCatchFinally* parse_try_catch_finally(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_try_catch_finally_statement(&parser);
}

CatchBlock* parse_catch(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_catch_block(&parser);
}

Throw* parse_throw(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_throw_statement(&parser);
}

Exception* parse_exception_type(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_exception_type_declaration(&parser);
}

