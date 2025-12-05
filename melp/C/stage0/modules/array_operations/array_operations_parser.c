#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "array_operations_parser.h"
#include "array_operations.h"
#include "../../lexer.h"

// Parse array method calls
ArrayOperation* parse_array_method(Lexer* lexer, const char* method_name) {
    ArrayOperation* op = NULL;
    
    // Determine operation type from method name
    if (strcmp(method_name, "push") == 0) {
        op = array_op_create(ARRAY_OP_PUSH);
    } else if (strcmp(method_name, "pop") == 0) {
        op = array_op_create(ARRAY_OP_POP);
    } else if (strcmp(method_name, "slice") == 0) {
        op = array_op_create(ARRAY_OP_SLICE);
    } else if (strcmp(method_name, "indexOf") == 0) {
        op = array_op_create(ARRAY_OP_INDEXOF);
    } else if (strcmp(method_name, "concat") == 0) {
        op = array_op_create(ARRAY_OP_CONCAT);
    } else if (strcmp(method_name, "reverse") == 0) {
        op = array_op_create(ARRAY_OP_REVERSE);
    } else if (strcmp(method_name, "sort") == 0) {
        op = array_op_create(ARRAY_OP_SORT);
    } else {
        fprintf(stderr, "Error: Unknown array method '%s'\n", method_name);
        return NULL;
    }
    
    // Parse method call: method(arg1, arg2, ...)
    Token* tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_LPAREN) {
        fprintf(stderr, "Error: Expected '(' after method name\n");
        token_free(tok);
        array_op_free(op);
        return NULL;
    }
    token_free(tok);
    
    // Parse arguments (simplified - skip to closing paren)
    int paren_depth = 1;
    while (paren_depth > 0) {
        tok = lexer_next_token(lexer);
        if (tok->type == TOKEN_EOF) {
            fprintf(stderr, "Error: Unexpected EOF in method call\n");
            token_free(tok);
            array_op_free(op);
            return NULL;
        }
        
        if (tok->type == TOKEN_LPAREN) {
            paren_depth++;
        } else if (tok->type == TOKEN_RPAREN) {
            paren_depth--;
        }
        
        token_free(tok);
    }
    
    return op;
}

// Parse array property access
ArrayOperation* parse_array_property(Lexer* lexer, const char* property_name) {
    ArrayOperation* op = NULL;
    
    if (strcmp(property_name, "length") == 0) {
        op = array_op_create(ARRAY_OP_LENGTH);
    } else {
        fprintf(stderr, "Error: Unknown array property '%s'\n", property_name);
        return NULL;
    }
    
    return op;
}
