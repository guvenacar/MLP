#include "generic_types_parser.h"
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
TypeParameter* parse_type_parameter_internal(Parser* parser);

// Parse generic type: struct Name<T, U> { ... }
GenericType* parse_generic_type_definition(Parser* parser) {
    // Expect type keyword (struct, class, etc.)
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    const char* keyword = parser->current_token->value;
    if (strcmp(keyword, "struct") != 0 && strcmp(keyword, "class") != 0 &&
        strcmp(keyword, "type") != 0) {
        return NULL;
    }
    advance(parser);
    
    // Parse type name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected type name\n");
        return NULL;
    }
    
    char* type_name = strdup(parser->current_token->value);
    advance(parser);
    
    GenericType* gen_type = generic_type_create(type_name);
    free(type_name);
    
    // Check for type parameters: <T, U, V>
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, "<") == 0) {
        advance(parser);
        
        // Parse type parameters
        while (parser->current_token && parser->current_token->type != TOKEN_IDENTIFIER) {
            TypeParameter* param = parse_type_parameter_internal(parser);
            if (param) {
                generic_type_add_param(gen_type, param);
            }
            
            // Check for comma
            if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
                advance(parser);
            } else if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
                       strcmp(parser->current_token->value, ">") == 0) {
                advance(parser);
                break;
            }
        }
    }
    
    // Parse type body (simplified - skip for now)
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
    
    return gen_type;
}

// Parse type parameter: T or T: Trait or T = DefaultType
TypeParameter* parse_type_parameter_internal(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    char* param_name = strdup(parser->current_token->value);
    advance(parser);
    
    TypeParameter* param = type_parameter_create(param_name);
    free(param_name);
    
    // Check for constraint: T: Trait
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, ":") == 0) {
        advance(parser);
        
        // Parse trait name (simplified - just skip identifier)
        if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
            param->has_constraint = 1;
            advance(parser);
        }
    }
    
    // Check for default: T = DefaultType
    if (parser->current_token && parser->current_token->type == TOKEN_ASSIGN) {
        advance(parser);
        
        // Parse default type (simplified - just skip identifier)
        if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
            advance(parser);
        }
    }
    
    return param;
}

// Parse type constraint: where T: Trait
TypeConstraint* parse_type_constraint_clause(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "where") != 0) {
        return NULL;
    }
    advance(parser);  // consume "where"
    
    // Parse parameter name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected type parameter after 'where'\n");
        return NULL;
    }
    
    char* param_name = strdup(parser->current_token->value);
    advance(parser);
    
    // Expect ':'
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, ":") != 0) {
        fprintf(stderr, "Error: Expected ':' after type parameter\n");
        free(param_name);
        return NULL;
    }
    advance(parser);
    
    // Parse trait name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected trait name after ':'\n");
        free(param_name);
        return NULL;
    }
    
    char* trait_name = strdup(parser->current_token->value);
    advance(parser);
    
    TypeConstraint* constraint = type_constraint_create(param_name, trait_name);
    free(param_name);
    free(trait_name);
    
    return constraint;
}

// Parse type instantiation: List<int> or HashMap<string, int>
TypeInstantiation* parse_type_instantiation_expr(Parser* parser) {
    // Parse generic type name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    char* type_name = strdup(parser->current_token->value);
    advance(parser);
    
    // Create generic type placeholder
    GenericType* gen_type = generic_type_create(type_name);
    free(type_name);
    
    TypeInstantiation* inst = type_instantiation_create(gen_type);
    
    // Expect '<'
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "<") != 0) {
        type_instantiation_free(inst);
        return NULL;
    }
    advance(parser);
    
    // Parse type arguments
    void* type_args[16];
    int arg_count = 0;
    
    while (parser->current_token && arg_count < 16) {
        if (parser->current_token->type == TOKEN_IDENTIFIER) {
            const char* arg = parser->current_token->value;
            
            if (strcmp(arg, ">") == 0) {
                advance(parser);
                break;
            }
            
            type_args[arg_count++] = (void*)strdup(arg);
            advance(parser);
            
            // Check for comma
            if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
                advance(parser);
            }
        } else {
            advance(parser);
        }
    }
    
    // Store type arguments
    inst->type_args = malloc(sizeof(void*) * arg_count);
    for (int i = 0; i < arg_count; i++) {
        inst->type_args[i] = type_args[i];
    }
    inst->arg_count = arg_count;
    
    return inst;
}

// Parse generic function: func name<T, U>(params) { ... }
GenericFunction* parse_generic_function_definition(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "func") != 0) {
        return NULL;
    }
    advance(parser);  // consume "func"
    
    // Parse function name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected function name after 'func'\n");
        return NULL;
    }
    
    char* func_name = strdup(parser->current_token->value);
    advance(parser);
    
    GenericFunction* gen_func = generic_function_create(func_name);
    free(func_name);
    
    // Check for type parameters: <T, U>
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, "<") == 0) {
        advance(parser);
        
        // Parse type parameters
        while (parser->current_token) {
            TypeParameter* param = parse_type_parameter_internal(parser);
            if (param) {
                generic_function_add_param(gen_func, param);
            }
            
            // Check for comma or >
            if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
                advance(parser);
            } else if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
                       strcmp(parser->current_token->value, ">") == 0) {
                advance(parser);
                break;
            }
        }
    }
    
    // Parse function parameters
    if (parser->current_token && parser->current_token->type == TOKEN_LPAREN) {
        advance(parser);
        
        // Skip parameters (simplified)
        while (parser->current_token && parser->current_token->type != TOKEN_RPAREN) {
            advance(parser);
        }
        
        if (parser->current_token && parser->current_token->type == TOKEN_RPAREN) {
            advance(parser);
        }
    }
    
    // Parse function body (simplified - skip)
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
    
    return gen_func;
}

// Legacy compatibility wrappers
GenericType* parse_generic_type(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_generic_type_definition(&parser);
}

TypeParameter* parse_type_parameter(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_type_parameter_internal(&parser);
}

TypeConstraint* parse_type_constraint(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_type_constraint_clause(&parser);
}

TypeInstantiation* parse_type_instantiation(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_type_instantiation_expr(&parser);
}

GenericFunction* parse_generic_function(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_generic_function_definition(&parser);
}
