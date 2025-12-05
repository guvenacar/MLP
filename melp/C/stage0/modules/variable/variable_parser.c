#include "variable_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Helper: Advance to next token
static void advance(VariableParser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    parser->current_token = lexer_next_token(parser->lexer);
}

// Create parser
VariableParser* variable_parser_create(Lexer* lexer) {
    VariableParser* parser = malloc(sizeof(VariableParser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

// Free parser
void variable_parser_free(VariableParser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    free(parser);
}

// Parse variable declaration: numeric x = 10
VariableDeclaration* variable_parse_declaration(VariableParser* parser) {
    if (!parser || !parser->current_token) {
        return NULL;
    }
    
    // Check for type keyword
    VarType base_type;
    if (parser->current_token->type == TOKEN_NUMERIC) {
        base_type = VAR_NUMERIC;
    } else if (parser->current_token->type == TOKEN_TEXT) {
        base_type = VAR_STRING;
    } else if (parser->current_token->type == TOKEN_BOOLEAN) {
        base_type = VAR_BOOLEAN;
    } else {
        return NULL;  // Not a variable declaration
    }
    
    advance(parser);  // consume type
    
    // Check for pointer (*) or array ([])
    int is_pointer = 0;
    int is_array = 0;
    int array_size = 0;
    VarType actual_type = base_type;
    
    if (parser->current_token->type == TOKEN_MULTIPLY) {
        // Pointer type: numeric*
        is_pointer = 1;
        actual_type = VAR_POINTER;
        advance(parser);  // consume '*'
    } else if (parser->current_token->type == TOKEN_LBRACKET) {
        // Array type: numeric[10] or numeric[]
        is_array = 1;
        actual_type = VAR_ARRAY;
        advance(parser);  // consume '['
        
        // Check for array size
        if (parser->current_token->type == TOKEN_NUMBER) {
            array_size = atoi(parser->current_token->value);
            advance(parser);  // consume size
        }
        
        if (parser->current_token->type != TOKEN_RBRACKET) {
            fprintf(stderr, "Error: Expected ']' in array declaration\n");
            return NULL;
        }
        advance(parser);  // consume ']'
    }
    
    // Expect identifier
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected identifier after type\n");
        return NULL;
    }
    
    VariableDeclaration* decl = malloc(sizeof(VariableDeclaration));
    decl->name = strdup(parser->current_token->value);
    decl->type = actual_type;
    decl->base_type = base_type;
    decl->is_pointer = is_pointer;
    decl->is_array = is_array;
    decl->array_size = array_size;
    decl->value = NULL;
    decl->init_expr = NULL;
    decl->internal_num_type = INTERNAL_INT64;
    decl->internal_str_type = INTERNAL_RODATA;
    decl->storage = STORAGE_BSS;
    decl->has_decimal_point = 0;
    
    advance(parser);  // consume identifier
    
    // Expect '='
    if (parser->current_token->type != TOKEN_ASSIGN) {
        fprintf(stderr, "Error: Expected '=' after variable name\n");
        free(decl->name);
        free(decl);
        return NULL;
    }
    
    advance(parser);  // consume '='
    
    // Parse value
    if (parser->current_token->type == TOKEN_NUMBER) {
        decl->value = strdup(parser->current_token->value);
        
        // Detect decimal point for TTO
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
        
        advance(parser);
    } else if (parser->current_token->type == TOKEN_STRING) {
        decl->value = strdup(parser->current_token->value);
        
        // TTO: Detect string storage type
        int len = strlen(decl->value);
        if (len <= 23) {
            decl->internal_str_type = INTERNAL_SSO;
        } else {
            decl->internal_str_type = INTERNAL_HEAP;
        }
        
        advance(parser);
    } else if (parser->current_token->type == TOKEN_TRUE) {
        decl->value = strdup("true");
        advance(parser);
    } else if (parser->current_token->type == TOKEN_FALSE) {
        decl->value = strdup("false");
        advance(parser);
    } else if (parser->current_token->type == TOKEN_LBRACKET) {
        // Array literal: [1, 2, 3]
        // For now, store the literal string representation
        // Full parsing happens in array module
        char array_literal[1024] = "[";
        int pos = 1;
        
        advance(parser);  // consume '['
        
        while (parser->current_token && 
               parser->current_token->type != TOKEN_RBRACKET &&
               parser->current_token->type != TOKEN_EOF) {
            
            // Append current token value
            if (parser->current_token->type == TOKEN_NUMBER ||
                parser->current_token->type == TOKEN_STRING ||
                parser->current_token->type == TOKEN_IDENTIFIER) {
                int len = strlen(parser->current_token->value);
                if (pos + len + 5 < 1024) {
                    if (parser->current_token->type == TOKEN_STRING) {
                        array_literal[pos++] = '"';
                    }
                    strcpy(array_literal + pos, parser->current_token->value);
                    pos += len;
                    if (parser->current_token->type == TOKEN_STRING) {
                        array_literal[pos++] = '"';
                    }
                }
            }
            
            advance(parser);
            
            // Check for comma
            if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
                if (pos + 2 < 1024) {
                    array_literal[pos++] = ',';
                    array_literal[pos++] = ' ';
                }
                advance(parser);  // consume ','
            }
        }
        
        if (parser->current_token && parser->current_token->type == TOKEN_RBRACKET) {
            if (pos + 2 < 1024) {
                array_literal[pos++] = ']';
                array_literal[pos] = '\0';
            }
            advance(parser);  // consume ']'
            
            decl->value = strdup(array_literal);
        } else {
            fprintf(stderr, "Error: Expected ']' in array literal\n");
            free(decl->name);
            free(decl);
            return NULL;
        }
    } else {
        fprintf(stderr, "Error: Expected value after '='\n");
        free(decl->name);
        free(decl);
        return NULL;
    }
    
    return decl;
}

// Parse variable assignment: x = 20
VariableAssignment* variable_parse_assignment(VariableParser* parser) {
    if (!parser || !parser->current_token) {
        return NULL;
    }
    
    // Check for identifier
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    VariableAssignment* assign = malloc(sizeof(VariableAssignment));
    assign->name = strdup(parser->current_token->value);
    assign->value_expr = NULL;
    
    advance(parser);  // consume identifier
    
    // Expect '='
    if (parser->current_token->type != TOKEN_ASSIGN) {
        fprintf(stderr, "Error: Expected '=' in assignment\n");
        free(assign->name);
        free(assign);
        return NULL;
    }
    
    advance(parser);  // consume '='
    
    // For now, we just store the token value
    // In full implementation, we'd parse expression here
    if (parser->current_token->type == TOKEN_NUMBER || 
        parser->current_token->type == TOKEN_STRING ||
        parser->current_token->type == TOKEN_IDENTIFIER) {
        // Store as simple value for now
        advance(parser);
    }
    
    return assign;
}

// Free declaration
void variable_declaration_free(VariableDeclaration* decl) {
    if (!decl) return;
    
    if (decl->name) free(decl->name);
    if (decl->value) free(decl->value);
    // init_expr would be freed here if used
    
    free(decl);
}

// Free assignment
void variable_assignment_free(VariableAssignment* assign) {
    if (!assign) return;
    
    if (assign->name) free(assign->name);
    // value_expr would be freed here if used
    
    free(assign);
}
