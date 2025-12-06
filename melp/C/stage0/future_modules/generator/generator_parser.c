#include "generator_parser.h"
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

// Parse generator body until "end func" or "end generator"
static void* parse_generator_body(Parser* parser, int* yield_count) {
    *yield_count = 0;
    int capacity = 8;
    Statement** body = (Statement**)malloc(sizeof(Statement*) * capacity);
    int body_count = 0;
    
    while (parser->current_token) {
        // Check for "end"
        if (parser->current_token->type == TOKEN_IDENTIFIER &&
            strcmp(parser->current_token->value, "end") == 0) {
            break;
        }
        
        // Count yield statements
        if (parser->current_token->type == TOKEN_IDENTIFIER &&
            strcmp(parser->current_token->value, "yield") == 0) {
            (*yield_count)++;
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

// Parse generator definition: func name(...) yields type ... end func
Generator* parse_generator_definition(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    // Check for "func" or "generator" keyword
    int is_generator = 0;
    if (strcmp(parser->current_token->value, "func") == 0) {
        advance(parser);
    } else if (strcmp(parser->current_token->value, "generator") == 0) {
        is_generator = 1;
        advance(parser);
    } else {
        return NULL;
    }
    
    // Parse generator name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected generator name\n");
        return NULL;
    }
    
    Generator* gen = generator_create(parser->current_token->value);
    advance(parser);
    
    // Parse parameters (skip for now, similar to function parser)
    if (!expect(parser, TOKEN_LPAREN, "Expected '(' after generator name")) {
        generator_free(gen);
        return NULL;
    }
    
    // Skip parameters (TODO: parse properly in later stages)
    int paren_depth = 1;
    while (parser->current_token && paren_depth > 0) {
        if (parser->current_token->type == TOKEN_LPAREN) {
            paren_depth++;
        } else if (parser->current_token->type == TOKEN_RPAREN) {
            paren_depth--;
        }
        if (paren_depth > 0) {
            advance(parser);
        }
    }
    
    if (!expect(parser, TOKEN_RPAREN, "Expected ')' after parameters")) {
        generator_free(gen);
        return NULL;
    }
    
    // Check for "yields" keyword
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, "yields") == 0) {
        advance(parser);
        // Skip return type (TODO: parse properly)
        if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
            advance(parser);
        }
    }
    
    // Parse generator body
    gen->body = parse_generator_body(parser, &gen->yield_count);
    
    // Expect "end func" or "end generator"
    if (!expect(parser, TOKEN_IDENTIFIER, "Expected 'end'")) {
        generator_free(gen);
        return NULL;
    }
    
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
        if (strcmp(parser->current_token->value, "func") == 0 ||
            strcmp(parser->current_token->value, "generator") == 0) {
            advance(parser);
        }
    }
    
    // Create context
    gen->context = gen_context_create();
    
    return gen;
}

// Parse yield statement: yield value or yield* generator
Yield* parse_yield_statement(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "yield") != 0) {
        return NULL;
    }
    advance(parser);  // consume "yield"
    
    // Static counter for yield IDs
    static int yield_counter = 0;
    int yield_id = yield_counter++;
    
    // Check for yield* (delegate to another generator) - using TOKEN_STAR if available
    // For now, skip this feature as TOKEN_STAR may not be in lexer
    
    // Parse yield value
    Expression* value = expression_parse(parser);
    if (!value) {
        fprintf(stderr, "Error: Expected expression after yield\n");
        return NULL;
    }
    
    Yield* yld = yield_create(value, yield_id);
    
    return yld;
}

// Parse generator call: gen.next() or gen()
GenCall* parse_generator_call(Parser* parser, Generator* gen) {
    if (!parser->current_token) {
        return NULL;
    }
    
    // Check for .next() call
    int is_next_call = 0;
    if (parser->current_token->type == TOKEN_DOT) {
        advance(parser);
        
        if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
            strcmp(parser->current_token->value, "next") == 0) {
            is_next_call = 1;
            advance(parser);
            
            // Expect ()
            if (!expect(parser, TOKEN_LPAREN, "Expected '(' after .next")) {
                return NULL;
            }
            if (!expect(parser, TOKEN_RPAREN, "Expected ')' after .next(")) {
                return NULL;
            }
        }
    } else if (parser->current_token->type == TOKEN_LPAREN) {
        // Direct call: gen()
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
        
        if (!expect(parser, TOKEN_RPAREN, "Expected ')' after arguments")) {
            return NULL;
        }
    }
    
    GenCall* call = gen_call_create(gen, is_next_call);
    return call;
}

// Parse for-in loop with generator: for item in generator() { ... }
Iterator* parse_for_in_generator(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_FOR) {
        return NULL;
    }
    advance(parser);  // consume "for"
    
    // Parse loop variable
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected loop variable after 'for'\n");
        return NULL;
    }
    char* loop_var = strdup(parser->current_token->value);
    advance(parser);
    
    // Expect "in" keyword
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "in") != 0) {
        fprintf(stderr, "Error: Expected 'in' after loop variable\n");
        free(loop_var);
        return NULL;
    }
    advance(parser);
    
    // Parse generator expression (simplified - just get identifier for now)
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected generator expression after 'in'\n");
        free(loop_var);
        return NULL;
    }
    
    // Create a generator reference (simplified)
    Generator* gen = generator_create(parser->current_token->value);
    advance(parser);
    
    // Check for function call ()
    if (parser->current_token && parser->current_token->type == TOKEN_LPAREN) {
        advance(parser);
        // Skip arguments
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
        if (!expect(parser, TOKEN_RPAREN, "Expected ')' after arguments")) {
            free(loop_var);
            generator_free(gen);
            return NULL;
        }
    }
    
    free(loop_var);
    
    // Create iterator
    Iterator* iter = iterator_create(gen);
    
    // Skip loop body (will be parsed by control_flow parser)
    // Just return the iterator for now
    
    return iter;
}

// Legacy compatibility wrappers
Generator* parse_generator(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_generator_definition(&parser);
}

Yield* parse_yield(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_yield_statement(&parser);
}

GenCall* parse_gen_call(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    // Note: This needs a generator reference, returning NULL for now
    return NULL;
}

Iterator* parse_for_generator(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_for_in_generator(&parser);
}

