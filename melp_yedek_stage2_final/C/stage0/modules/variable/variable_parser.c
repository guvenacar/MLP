#include "variable_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void advance(Parser* parser) {
    if (parser->current_token) token_free(parser->current_token);
    parser->current_token = lexer_next_token(parser->lexer);
}

static Declaration* parse_declaration(Parser* parser) {
    Declaration* decl = malloc(sizeof(Declaration));
    
    // Initialize smart storage fields
    decl->internal_num_type = INTERNAL_INT64;
    decl->internal_str_type = INTERNAL_RODATA;
    decl->storage = STORAGE_BSS;  // Default: global in .bss
    decl->has_decimal_point = 0;
    
    // Type
    if (parser->current_token->type == TOKEN_NUMERIC) {
        decl->type = VAR_NUMERIC;
    } else if (parser->current_token->type == TOKEN_TEXT) {
        decl->type = VAR_STRING;
    } else if (parser->current_token->type == TOKEN_BOOLEAN) {
        decl->type = VAR_BOOLEAN;
    } else {
        fprintf(stderr, "Error: Expected type keyword\n");
        free(decl);
        return NULL;
    }
    advance(parser);
    
    // Name
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected identifier\n");
        free(decl);
        return NULL;
    }
    decl->name = strdup(parser->current_token->value);
    advance(parser);
    
    // =
    if (parser->current_token->type != TOKEN_ASSIGN) {
        fprintf(stderr, "Error: Expected '='\n");
        free(decl->name);
        free(decl);
        return NULL;
    }
    advance(parser);
    
    // Value with smart detection
    if (parser->current_token->type == TOKEN_NUMBER) {
        decl->value = strdup(parser->current_token->value);
        
        // Detect decimal point for floating point numbers
        if (strchr(decl->value, '.') != NULL) {
            decl->has_decimal_point = 1;
            decl->internal_num_type = INTERNAL_DOUBLE;
        } else {
            // Check if value fits in INT64
            long long val = atoll(decl->value);
            if (val >= -2147483648LL && val <= 2147483647LL) {
                decl->internal_num_type = INTERNAL_INT64;
            } else {
                decl->internal_num_type = INTERNAL_BIGDECIMAL;
            }
        }
    } else if (parser->current_token->type == TOKEN_STRING) {
        decl->value = strdup(parser->current_token->value);
        
        // Smart string storage detection
        int len = strlen(decl->value);
        if (len <= 23) {
            decl->internal_str_type = INTERNAL_SSO;  // Small String Optimization
        } else {
            decl->internal_str_type = INTERNAL_HEAP;  // Heap allocation
        }
    } else if (parser->current_token->type == TOKEN_TRUE) {
        decl->value = strdup("true");
    } else if (parser->current_token->type == TOKEN_FALSE) {
        decl->value = strdup("false");
    } else {
        fprintf(stderr, "Error: Expected value\n");
        free(decl->name);
        free(decl);
        return NULL;
    }
    advance(parser);
    
    return decl;
}
