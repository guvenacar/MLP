#include "pattern_matching_parser.h"
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

static int match_token(Parser* parser, TokenType type) {
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
Pattern* parse_pattern_internal(Parser* parser);
MatchArm* parse_match_arm_internal(Parser* parser);

// Parse match expression: match expr { arms }
Match* parse_match_statement(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "match") != 0) {
        return NULL;
    }
    advance(parser);  // consume "match"
    
    // Parse expression to match (simplified - skip for now)
    void* expr = NULL;
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
        advance(parser);
    }
    
    Match* match = pattern_match_create(expr);
    
    // Expect '{'
    if (!expect(parser, TOKEN_LBRACKET, "Expected '{' after match expression")) {
        pattern_match_free(match);
        return NULL;
    }
    
    // Parse match arms
    while (parser->current_token && parser->current_token->type != TOKEN_RBRACKET) {
        MatchArm* arm = parse_match_arm_internal(parser);
        if (arm) {
            match_add_arm(match, arm);
        }
        
        // Check for comma
        if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
            advance(parser);
        }
    }
    
    if (!expect(parser, TOKEN_RBRACKET, "Expected '}' at end of match")) {
        pattern_match_free(match);
        return NULL;
    }
    
    return match;
}

// Parse match arm: pattern | pattern when guard => body
MatchArm* parse_match_arm_internal(Parser* parser) {
    MatchArm* arm = match_arm_create();
    
    // Parse patterns separated by |
    while (1) {
        Pattern* pat = parse_pattern_internal(parser);
        if (!pat) {
            break;
        }
        
        match_arm_add_pattern(arm, pat);
        
        // Check for | (or pattern)
        if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
            strcmp(parser->current_token->value, "|") == 0) {
            advance(parser);
        } else {
            break;
        }
    }
    
    // Check for guard: when condition
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, "when") == 0) {
        advance(parser);
        
        // Parse guard expression (simplified - just skip)
        while (parser->current_token && parser->current_token->type != TOKEN_IDENTIFIER) {
            advance(parser);
        }
    }
    
    // Expect '=>'
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, "=>") == 0) {
        advance(parser);
    }
    
    // Parse arm body
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
    
    return arm;
}

// Parse pattern: literal, variable, wildcard, struct, array, range
Pattern* parse_pattern_internal(Parser* parser) {
    if (!parser->current_token) {
        return NULL;
    }
    
    Pattern* pat = NULL;
    
    // Check pattern type
    if (parser->current_token->type == TOKEN_IDENTIFIER) {
        const char* val = parser->current_token->value;
        
        // Wildcard pattern: _
        if (strcmp(val, "_") == 0) {
            pat = pattern_create(PAT_WILDCARD);
            advance(parser);
            
        } else {
            // Variable binding or struct pattern
            char* name = strdup(val);
            advance(parser);
            
            // Check for struct pattern: Name { fields }
            if (parser->current_token && parser->current_token->type == TOKEN_LBRACKET) {
                pat = pattern_create(PAT_STRUCT);
                advance(parser);
                
                // Parse struct fields
                while (parser->current_token && parser->current_token->type != TOKEN_RBRACKET) {
                    if (parser->current_token->type == TOKEN_IDENTIFIER) {
                        Pattern* field_pat = pattern_create(PAT_VARIABLE);
                        pattern_set_var_name(field_pat, parser->current_token->value);
                        pattern_add_sub(pat, field_pat);
                        advance(parser);
                        
                        // Check for comma
                        if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
                            advance(parser);
                        }
                    } else {
                        advance(parser);
                    }
                }
                
                if (parser->current_token && parser->current_token->type == TOKEN_RBRACKET) {
                    advance(parser);
                }
                
            } else if (parser->current_token && parser->current_token->type == TOKEN_LPAREN) {
                // Tuple pattern: Name(x, y)
                pat = pattern_create(PAT_STRUCT);
                advance(parser);
                
                // Parse tuple elements
                while (parser->current_token && parser->current_token->type != TOKEN_RPAREN) {
                    Pattern* elem = parse_pattern_internal(parser);
                    if (elem) {
                        pattern_add_sub(pat, elem);
                    }
                    
                    if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
                        advance(parser);
                    }
                }
                
                if (parser->current_token && parser->current_token->type == TOKEN_RPAREN) {
                    advance(parser);
                }
                
            } else {
                // Simple variable binding
                pat = pattern_create(PAT_VARIABLE);
                pattern_set_var_name(pat, name);
            }
            
            free(name);
        }
        
    } else if (parser->current_token->type == TOKEN_LBRACKET) {
        // Array pattern: [x, y, z] or [head, ...tail]
        pat = pattern_create(PAT_ARRAY);
        advance(parser);
        
        while (parser->current_token && parser->current_token->type != TOKEN_RBRACKET) {
            // Check for rest pattern: ...name
            if (parser->current_token->type == TOKEN_IDENTIFIER &&
                strcmp(parser->current_token->value, "...") == 0) {
                advance(parser);
                
                Pattern* rest = pattern_create(PAT_VARIABLE);
                rest->is_rest = 1;
                
                if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
                    pattern_set_var_name(rest, parser->current_token->value);
                    advance(parser);
                }
                
                pattern_add_sub(pat, rest);
            } else {
                Pattern* elem = parse_pattern_internal(parser);
                if (elem) {
                    pattern_add_sub(pat, elem);
                }
            }
            
            if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
                advance(parser);
            }
        }
        
        if (parser->current_token && parser->current_token->type == TOKEN_RBRACKET) {
            advance(parser);
        }
        
    } else if (parser->current_token->type == TOKEN_LPAREN) {
        // Tuple pattern or grouped pattern
        advance(parser);
        
        Pattern* inner = parse_pattern_internal(parser);
        pat = inner;
        
        if (parser->current_token && parser->current_token->type == TOKEN_RPAREN) {
            advance(parser);
        }
        
    } else {
        // Literal pattern (number, string, etc.)
        pat = pattern_create(PAT_LITERAL);
        advance(parser);
    }
    
    // Check for range pattern: x..y
    if (pat && parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, "..") == 0) {
        advance(parser);
        
        Pattern* range_pat = pattern_create(PAT_RANGE);
        pattern_add_sub(range_pat, pat);
        
        Pattern* end = parse_pattern_internal(parser);
        if (end) {
            pattern_add_sub(range_pat, end);
        }
        
        pat = range_pat;
    }
    
    return pat;
}

// Parse destructure: let pattern = expr
Destructure* parse_destructure_statement(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "let") != 0) {
        return NULL;
    }
    advance(parser);  // consume "let"
    
    // Parse pattern
    Pattern* pat = parse_pattern_internal(parser);
    if (!pat) {
        fprintf(stderr, "Error: Expected pattern after 'let'\n");
        return NULL;
    }
    
    // Expect '='
    if (!expect(parser, TOKEN_ASSIGN, "Expected '=' after pattern")) {
        pattern_free(pat);
        return NULL;
    }
    
    // Parse value expression (simplified - just skip identifier)
    void* value = NULL;
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
        advance(parser);
    }
    
    Destructure* dest = destructure_create(pat, value);
    return dest;
}

// Legacy compatibility wrappers
Match* parse_match(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_match_statement(&parser);
}

MatchArm* parse_match_arm(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_match_arm_internal(&parser);
}

Pattern* parse_pattern(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_pattern_internal(&parser);
}

Destructure* parse_destructure(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_destructure_statement(&parser);
}
