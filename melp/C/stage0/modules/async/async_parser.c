#include "async_parser.h"
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
typedef struct Expression Expression;
typedef struct Statement Statement;
extern Expression* expression_parse(Parser* parser);
extern Statement* parse_statement(Parser* parser);

// Parse async function body
static void* parse_async_body(Parser* parser) {
    int capacity = 8;
    Statement** body = (Statement**)malloc(sizeof(Statement*) * capacity);
    int body_count = 0;
    
    while (parser->current_token) {
        // Check for "end"
        if (parser->current_token->type == TOKEN_IDENTIFIER &&
            strcmp(parser->current_token->value, "end") == 0) {
            break;
        }
        
        Statement* stmt = parse_statement(parser);
        if (!stmt) {
            break;
        }
        
        if (body_count >= capacity) {
            capacity *= 2;
            body = (Statement**)realloc(body, sizeof(Statement*) * capacity);
        }
        
        body[body_count++] = stmt;
    }
    
    return body;
}

// Parse async function: async func name(...) returns type ... end func
AsyncFunction* parse_async_function_definition(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    // Check for "async" keyword
    if (strcmp(parser->current_token->value, "async") != 0) {
        return NULL;
    }
    advance(parser);
    
    // Expect "func" or "function"
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        (strcmp(parser->current_token->value, "func") != 0 &&
         strcmp(parser->current_token->value, "function") != 0)) {
        fprintf(stderr, "Error: Expected 'func' after 'async'\n");
        return NULL;
    }
    advance(parser);
    
    // Parse function name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected function name\n");
        return NULL;
    }
    
    AsyncFunction* func = async_function_create(parser->current_token->value, ASYNC_FUNCTION);
    advance(parser);
    
    // Parse parameters
    if (!expect(parser, TOKEN_LPAREN, "Expected '(' after function name")) {
        async_function_free(func);
        return NULL;
    }
    
    // Parse parameter list (simplified - count parameters)
    func->param_count = 0;
    int capacity = 4;
    func->param_names = (char**)malloc(sizeof(char*) * capacity);
    
    while (parser->current_token && parser->current_token->type != TOKEN_RPAREN) {
        if (parser->current_token->type == TOKEN_IDENTIFIER) {
            if (func->param_count >= (size_t)capacity) {
                capacity *= 2;
                func->param_names = (char**)realloc(func->param_names, sizeof(char*) * capacity);
            }
            func->param_names[func->param_count++] = strdup(parser->current_token->value);
            advance(parser);
            
            // Skip type annotation if present
            if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
                advance(parser);
            }
        }
        
        if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
            advance(parser);
        }
    }
    
    if (!expect(parser, TOKEN_RPAREN, "Expected ')' after parameters")) {
        async_function_free(func);
        return NULL;
    }
    
    // Check for "returns" keyword
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, "returns") == 0) {
        advance(parser);
        // Skip return type
        if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
            advance(parser);
        }
    }
    
    // Parse async function body
    func->body = parse_async_body(parser);
    
    // Expect "end func"
    if (!expect(parser, TOKEN_IDENTIFIER, "Expected 'end'")) {
        async_function_free(func);
        return NULL;
    }
    
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        (strcmp(parser->current_token->value, "func") == 0 ||
         strcmp(parser->current_token->value, "function") == 0)) {
        advance(parser);
    }
    
    // Create return future
    func->return_future = future_create();
    
    return func;
}

// Parse await expression: await expression
AwaitExpr* parse_await_expression(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "await") != 0) {
        return NULL;
    }
    advance(parser);  // consume "await"
    
    AwaitExpr* expr = await_expr_create(AWAIT_FUTURE);
    
    // Check for special await forms
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
        if (strcmp(parser->current_token->value, "await_all") == 0) {
            expr->type = AWAIT_ALL;
            advance(parser);
        } else if (strcmp(parser->current_token->value, "await_any") == 0) {
            expr->type = AWAIT_ANY;
            advance(parser);
        } else if (strcmp(parser->current_token->value, "await_race") == 0) {
            expr->type = AWAIT_RACE;
            advance(parser);
        }
    }
    
    // Parse await target(s)
    if (expr->type == AWAIT_ALL || expr->type == AWAIT_ANY || expr->type == AWAIT_RACE) {
        // Parse array of targets: [f1, f2, f3]
        if (!expect(parser, TOKEN_LBRACKET, "Expected '[' after await_all/any/race")) {
            await_expr_free(expr);
            return NULL;
        }
        
        expr->target_count = 0;
        int capacity = 4;
        expr->targets = (Future**)malloc(sizeof(Future*) * capacity);
        
        while (parser->current_token && parser->current_token->type != TOKEN_RBRACKET) {
            // Parse expression (simplified - create placeholder future)
            Expression* target_expr = expression_parse(parser);
            if (!target_expr) {
                await_expr_free(expr);
                return NULL;
            }
            
            if (expr->target_count >= (size_t)capacity) {
                capacity *= 2;
                expr->targets = (Future**)realloc(expr->targets, sizeof(Future*) * capacity);
            }
            
            // Create future for this target (simplified)
            expr->targets[expr->target_count++] = future_create();
            
            if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
                advance(parser);
            }
        }
        
        if (!expect(parser, TOKEN_RBRACKET, "Expected ']' after targets")) {
            await_expr_free(expr);
            return NULL;
        }
    } else {
        // Single target
        Expression* target_expr = expression_parse(parser);
        if (!target_expr) {
            await_expr_free(expr);
            return NULL;
        }
        expr->target = target_expr;
    }
    
    return expr;
}

// Parse promise creation: Promise.new() or new Promise()
Promise* parse_promise_creation_internal(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    // Check for "new Promise" or "Promise.new"
    int is_promise = 0;
    if (strcmp(parser->current_token->value, "Promise") == 0) {
        is_promise = 1;
        advance(parser);
        
        // Check for .new()
        if (parser->current_token && parser->current_token->type == TOKEN_DOT) {
            advance(parser);
            if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
                strcmp(parser->current_token->value, "new") == 0) {
                advance(parser);
            }
        }
    } else if (strcmp(parser->current_token->value, "new") == 0) {
        advance(parser);
        if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
            strcmp(parser->current_token->value, "Promise") == 0) {
            is_promise = 1;
            advance(parser);
        }
    }
    
    if (!is_promise) {
        return NULL;
    }
    
    // Parse constructor arguments if present
    if (parser->current_token && parser->current_token->type == TOKEN_LPAREN) {
        advance(parser);
        // Skip arguments for now
        int depth = 1;
        while (parser->current_token && depth > 0) {
            if (parser->current_token->type == TOKEN_LPAREN) {
                depth++;
            } else if (parser->current_token->type == TOKEN_RPAREN) {
                depth--;
            }
            if (depth > 0) {
                advance(parser);
            }
        }
        if (!expect(parser, TOKEN_RPAREN, "Expected ')' after Promise constructor")) {
            return NULL;
        }
    }
    
    Promise* promise = promise_create();
    return promise;
}

// Parse future chain: future.then(...).catch(...).finally(...)
void parse_future_chain_methods(Parser* parser, Future* future) {
    if (!future || !parser->current_token) {
        return;
    }
    
    while (parser->current_token && parser->current_token->type == TOKEN_DOT) {
        advance(parser);  // consume '.'
        
        if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
            break;
        }
        
        const char* method = parser->current_token->value;
        advance(parser);
        
        // Expect (
        if (!expect(parser, TOKEN_LPAREN, "Expected '(' after method name")) {
            break;
        }
        
        // Parse callback (lambda or function reference)
        // Simplified: just skip the argument
        int depth = 1;
        while (parser->current_token && depth > 0) {
            if (parser->current_token->type == TOKEN_LPAREN) {
                depth++;
            } else if (parser->current_token->type == TOKEN_RPAREN) {
                depth--;
            }
            if (depth > 0) {
                advance(parser);
            }
        }
        
        if (!expect(parser, TOKEN_RPAREN, "Expected ')' after callback")) {
            break;
        }
        
        // Set appropriate callback (simplified)
        if (strcmp(method, "then") == 0) {
            // future_then(future, callback);
        } else if (strcmp(method, "catch") == 0) {
            // future_catch(future, callback);
        } else if (strcmp(method, "finally") == 0) {
            // future_finally(future, callback);
        }
    }
}

// Legacy compatibility wrappers
AsyncFunction* parse_async_function(void) {
    // Note: Needs Parser context, returning NULL for now
    return NULL;
}

AwaitExpr* parse_await_expr(void) {
    // Note: Needs Parser context, returning NULL for now
    return NULL;
}

void parse_future_chain(Future* future) {
    // Note: Needs Parser context
    (void)future;
}

Promise* parse_promise_creation(void) {
    // Note: Needs Parser context, returning NULL for now
    return NULL;
}

// Async generator parse - TODO Stage 1

// Async generator parse - TODO Stage 1
AsyncFunction* parse_async_generator(void) {
    // Async generator parse et
    // async function* generate_numbers() {
    //     for i in 0..10 {
    //         await sleep(100);
    //         yield i;
    //     }
    // }
    
    // TODO: Stage 1'de implement edilecek
    return NULL;
}
