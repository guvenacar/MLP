#include "enum_parser.h"
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
EnumVariant* parse_enum_variant_internal(Parser* parser, int tag);

// Parse enum definition: enum Name { Variant1, Variant2, ... }
EnumDef* parse_enum_definition(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "enum") != 0) {
        return NULL;
    }
    advance(parser);  // consume "enum"
    
    // Parse enum name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected enum name after 'enum'\n");
        return NULL;
    }
    
    char* enum_name = strdup(parser->current_token->value);
    advance(parser);
    
    EnumDef* enum_def = enum_def_create(enum_name);
    free(enum_name);
    
    // Expect '{'
    if (!expect(parser, TOKEN_LBRACKET, "Expected '{' after enum name")) {
        enum_def_free(enum_def);
        return NULL;
    }
    
    // Parse enum variants
    int tag = 0;
    while (parser->current_token && parser->current_token->type != TOKEN_RBRACKET) {
        if (parser->current_token->type == TOKEN_IDENTIFIER) {
            EnumVariant* variant = parse_enum_variant_internal(parser, tag);
            if (variant) {
                enum_def_add_variant(enum_def, variant);
                tag++;
            }
            
            // Check for comma
            if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
                advance(parser);
            }
        } else {
            advance(parser);
        }
    }
    
    if (!expect(parser, TOKEN_RBRACKET, "Expected '}' at end of enum")) {
        enum_def_free(enum_def);
        return NULL;
    }
    
    return enum_def;
}

// Parse enum variant: VariantName or VariantName(Type) or VariantName { fields }
EnumVariant* parse_enum_variant_internal(Parser* parser, int tag) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    char* variant_name = strdup(parser->current_token->value);
    advance(parser);
    
    EnumVariant* variant = enum_variant_create(variant_name, tag);
    free(variant_name);
    
    // Check for associated data
    if (parser->current_token) {
        if (parser->current_token->type == TOKEN_LPAREN) {
            // Tuple-style: VariantName(Type1, Type2)
            variant->has_data = 1;
            advance(parser);
            
            // Parse types (simplified - just skip until ')')
            while (parser->current_token && parser->current_token->type != TOKEN_RPAREN) {
                advance(parser);
            }
            
            if (parser->current_token && parser->current_token->type == TOKEN_RPAREN) {
                advance(parser);
            }
            
        } else if (parser->current_token->type == TOKEN_LBRACKET) {
            // Struct-style: VariantName { field1: Type1, field2: Type2 }
            variant->has_data = 1;
            advance(parser);
            
            // Parse fields (simplified - just skip until '}')
            while (parser->current_token && parser->current_token->type != TOKEN_RBRACKET) {
                advance(parser);
            }
            
            if (parser->current_token && parser->current_token->type == TOKEN_RBRACKET) {
                advance(parser);
            }
        }
    }
    
    return variant;
}

// Parse enum match: match expr { variant1 => body1, variant2 => body2, _ => default }
EnumMatch* parse_enum_match_statement(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "match") != 0) {
        return NULL;
    }
    advance(parser);  // consume "match"
    
    // Parse expression to match (simplified - just skip identifier)
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected expression after 'match'\n");
        return NULL;
    }
    advance(parser);
    
    // Create placeholder enum value
    EnumValue* value = NULL;
    EnumMatch* match = enum_match_create(value);
    
    // Expect '{'
    if (!expect(parser, TOKEN_LBRACKET, "Expected '{' after match expression")) {
        enum_match_free(match);
        return NULL;
    }
    
    // Parse match arms
    int arm_tag = 0;
    while (parser->current_token && parser->current_token->type != TOKEN_RBRACKET) {
        // Parse pattern (variant name or _)
        if (parser->current_token->type == TOKEN_IDENTIFIER) {
            const char* pattern = parser->current_token->value;
            
            // Check for wildcard _
            int is_wildcard = (strcmp(pattern, "_") == 0);
            advance(parser);
            
            // Expect '=>'
            if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
                strcmp(parser->current_token->value, "=>") == 0) {
                advance(parser);
            }
            
            // Parse arm body (simplified - skip until comma or })
            void* arm_body = NULL;
            if (parser->current_token && parser->current_token->type == TOKEN_LBRACKET) {
                // Block body
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
            } else {
                // Expression body - skip until comma or }
                while (parser->current_token && 
                       parser->current_token->type != TOKEN_COMMA &&
                       parser->current_token->type != TOKEN_RBRACKET) {
                    advance(parser);
                }
            }
            
            if (!is_wildcard) {
                enum_match_add_arm(match, arm_tag++, arm_body);
            }
            
            // Check for comma
            if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
                advance(parser);
            }
        } else {
            advance(parser);
        }
    }
    
    if (!expect(parser, TOKEN_RBRACKET, "Expected '}' at end of match")) {
        enum_match_free(match);
        return NULL;
    }
    
    return match;
}

// Legacy compatibility wrappers
EnumDef* parse_enum_def(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_enum_definition(&parser);
}

EnumVariant* parse_enum_variant(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_enum_variant_internal(&parser, 0);
}

EnumMatch* parse_enum_match(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_enum_match_statement(&parser);
}
