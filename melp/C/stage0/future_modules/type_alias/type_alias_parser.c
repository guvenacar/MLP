#include "type_alias_parser.h"
#include "type_alias.h"
#include "../../lexer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Parse type alias declaration: type Distance = numeric
TypeAlias* parse_type_alias(Lexer* lexer) {
    // Expect 'type' keyword (already consumed)
    
    // Parse alias name
    Token* name_tok = lexer_next_token(lexer);
    if (!name_tok || name_tok->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected type alias name\n");
        if (name_tok) token_free(name_tok);
        return NULL;
    }
    
    char* alias_name = strdup(name_tok->value);
    token_free(name_tok);
    
    // Expect '='
    Token* eq_tok = lexer_next_token(lexer);
    if (!eq_tok || eq_tok->type != TOKEN_ASSIGN) {
        fprintf(stderr, "Error: Expected '=' after type alias name\n");
        if (eq_tok) token_free(eq_tok);
        free(alias_name);
        return NULL;
    }
    token_free(eq_tok);
    
    // Parse target type
    Token* type_tok = lexer_next_token(lexer);
    if (!type_tok || type_tok->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected target type\n");
        if (type_tok) token_free(type_tok);
        free(alias_name);
        return NULL;
    }
    
    char* target_type = strdup(type_tok->value);
    token_free(type_tok);
    
    // Create type alias
    TypeAlias* alias = type_alias_create(alias_name, target_type);
    
    // Register globally
    type_alias_register(alias);
    
    free(alias_name);
    free(target_type);
    
    return alias;
}
