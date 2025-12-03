#include "interface_trait_parser.h"
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
TraitMethod* parse_trait_method_internal(Parser* parser);

// Parse trait definition: trait Name { methods }
TraitDef* parse_trait_definition(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "trait") != 0) {
        return NULL;
    }
    advance(parser);  // consume "trait"
    
    // Parse trait name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected trait name after 'trait'\n");
        return NULL;
    }
    
    char* trait_name = strdup(parser->current_token->value);
    advance(parser);
    
    TraitDef* trait = trait_def_create(trait_name);
    free(trait_name);
    
    // Check for parent traits: trait Name : Parent1, Parent2
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, ":") == 0) {
        advance(parser);
        
        // Parse parent traits
        while (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
            char* parent_name = strdup(parser->current_token->value);
            TraitDef* parent = trait_def_create(parent_name);
            trait_def_add_super_trait(trait, parent);
            free(parent_name);
            advance(parser);
            
            // Check for comma
            if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
                advance(parser);
            } else {
                break;
            }
        }
    }
    
    // Expect '{'
    if (!expect(parser, TOKEN_LBRACKET, "Expected '{' after trait name")) {
        trait_def_free(trait);
        return NULL;
    }
    
    // Parse trait methods
    while (parser->current_token && parser->current_token->type != TOKEN_RBRACKET) {
        if (parser->current_token->type == TOKEN_IDENTIFIER) {
            const char* keyword = parser->current_token->value;
            
            if (strcmp(keyword, "func") == 0 || strcmp(keyword, "method") == 0) {
                TraitMethod* method = parse_trait_method_internal(parser);
                if (method) {
                    trait_def_add_method(trait, method);
                }
            } else {
                advance(parser);
            }
        } else {
            advance(parser);
        }
    }
    
    if (!expect(parser, TOKEN_RBRACKET, "Expected '}' at end of trait")) {
        trait_def_free(trait);
        return NULL;
    }
    
    return trait;
}

// Parse trait method: func name(params) returns Type or with default impl
TraitMethod* parse_trait_method_internal(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    const char* keyword = parser->current_token->value;
    if (strcmp(keyword, "func") != 0 && strcmp(keyword, "method") != 0) {
        return NULL;
    }
    advance(parser);  // consume "func"
    
    // Parse method name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected method name after 'func'\n");
        return NULL;
    }
    
    char* method_name = strdup(parser->current_token->value);
    advance(parser);
    
    TraitMethod* method = trait_method_create(method_name);
    free(method_name);
    
    // Expect '('
    if (!expect(parser, TOKEN_LPAREN, "Expected '(' after method name")) {
        trait_method_free(method);
        return NULL;
    }
    
    // Parse parameters (simplified - just skip until ')')
    while (parser->current_token && parser->current_token->type != TOKEN_RPAREN) {
        advance(parser);
    }
    
    if (!expect(parser, TOKEN_RPAREN, "Expected ')' after parameters")) {
        trait_method_free(method);
        return NULL;
    }
    
    // Check for return type: returns Type
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, "returns") == 0) {
        advance(parser);
        
        // Parse return type
        if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
            advance(parser);
        }
    }
    
    // Check for default implementation
    if (parser->current_token && parser->current_token->type == TOKEN_LBRACKET) {
        method->has_default = 1;
        advance(parser);
        
        // Skip default implementation body
        int brace_count = 1;
        while (parser->current_token && brace_count > 0) {
            if (parser->current_token->type == TOKEN_LBRACKET) {
                brace_count++;
            } else if (parser->current_token->type == TOKEN_RBRACKET) {
                brace_count--;
            }
            if (brace_count > 0) {
                advance(parser);
            }
        }
        
        if (brace_count == 0) {
            advance(parser);  // consume final '}'
        }
    }
    
    return method;
}

// Parse trait implementation: impl Trait for Type { methods }
TraitImpl* parse_trait_implementation(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "impl") != 0) {
        return NULL;
    }
    advance(parser);  // consume "impl"
    
    // Parse trait name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected trait name after 'impl'\n");
        return NULL;
    }
    
    char* trait_name = strdup(parser->current_token->value);
    TraitDef* trait = trait_def_create(trait_name);
    free(trait_name);
    advance(parser);
    
    // Expect "for"
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "for") != 0) {
        fprintf(stderr, "Error: Expected 'for' after trait name\n");
        trait_def_free(trait);
        return NULL;
    }
    advance(parser);
    
    // Parse type name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected type name after 'for'\n");
        trait_def_free(trait);
        return NULL;
    }
    
    char* type_name = strdup(parser->current_token->value);
    advance(parser);
    
    TraitImpl* impl = trait_impl_create(trait, type_name);
    free(type_name);
    
    // Expect '{'
    if (!expect(parser, TOKEN_LBRACKET, "Expected '{' after type name")) {
        trait_impl_free(impl);
        return NULL;
    }
    
    // Parse method implementations
    while (parser->current_token && parser->current_token->type != TOKEN_RBRACKET) {
        if (parser->current_token->type == TOKEN_IDENTIFIER) {
            const char* keyword = parser->current_token->value;
            
            if (strcmp(keyword, "func") == 0 || strcmp(keyword, "method") == 0) {
                // Parse method implementation (simplified - just skip)
                advance(parser);
                
                // Skip method name
                if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
                    advance(parser);
                }
                
                // Skip parameters
                if (parser->current_token && parser->current_token->type == TOKEN_LPAREN) {
                    advance(parser);
                    while (parser->current_token && parser->current_token->type != TOKEN_RPAREN) {
                        advance(parser);
                    }
                    if (parser->current_token && parser->current_token->type == TOKEN_RPAREN) {
                        advance(parser);
                    }
                }
                
                // Skip method body
                if (parser->current_token && parser->current_token->type == TOKEN_LBRACKET) {
                    int brace_count = 1;
                    advance(parser);
                    while (parser->current_token && brace_count > 0) {
                        if (parser->current_token->type == TOKEN_LBRACKET) {
                            brace_count++;
                        } else if (parser->current_token->type == TOKEN_RBRACKET) {
                            brace_count--;
                        }
                        if (brace_count > 0) {
                            advance(parser);
                        }
                    }
                    if (brace_count == 0) {
                        advance(parser);
                    }
                }
            } else {
                advance(parser);
            }
        } else {
            advance(parser);
        }
    }
    
    if (!expect(parser, TOKEN_RBRACKET, "Expected '}' at end of impl")) {
        trait_impl_free(impl);
        return NULL;
    }
    
    return impl;
}

// Parse trait bound: T: Trait or T: Trait + Trait2
TraitBound* parse_trait_bound_internal(Parser* parser) {
    // Parse type parameter name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    char* type_param = strdup(parser->current_token->value);
    advance(parser);
    
    // Expect ':'
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, ":") != 0) {
        free(type_param);
        return NULL;
    }
    advance(parser);
    
    // Parse trait name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected trait name after ':'\n");
        free(type_param);
        return NULL;
    }
    
    char* trait_name = strdup(parser->current_token->value);
    TraitDef* trait = trait_def_create(trait_name);
    free(trait_name);
    advance(parser);
    
    TraitBound* bound = trait_bound_create(type_param, trait);
    free(type_param);
    
    // Check for additional bounds: + Trait2
    while (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
           strcmp(parser->current_token->value, "+") == 0) {
        advance(parser);
        
        // Parse additional trait (simplified - just skip for now)
        if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
            advance(parser);
        }
    }
    
    return bound;
}

// Legacy compatibility wrappers
TraitDef* parse_trait_def(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_trait_definition(&parser);
}

TraitMethod* parse_trait_method(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_trait_method_internal(&parser);
}

TraitImpl* parse_trait_impl(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_trait_implementation(&parser);
}

TraitBound* parse_trait_bound(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_trait_bound_internal(&parser);
}
