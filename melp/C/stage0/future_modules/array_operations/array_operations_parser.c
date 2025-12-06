#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "array_operations_parser.h"
#include "array_operations.h"
#include "../../lexer.h"

// Parse range syntax: 1..10
ArrayOperation* parse_array_range(Lexer* lexer) {
    ArrayOperation* op = array_op_create(ARRAY_OP_RANGE);
    
    // Parse start value
    Token* start_tok = lexer_next_token(lexer);
    if (start_tok->type != TOKEN_NUMBER) {
        fprintf(stderr, "Error: Expected number for range start\n");
        token_free(start_tok);
        array_op_free(op);
        return NULL;
    }
    
    // Store start value (simplified - would need proper value storage)
    token_free(start_tok);
    
    // Expect '..'
    Token* dots = lexer_next_token(lexer);
    if (dots->type != TOKEN_DOT) {
        fprintf(stderr, "Error: Expected '..' for range\n");
        token_free(dots);
        array_op_free(op);
        return NULL;
    }
    token_free(dots);
    
    dots = lexer_next_token(lexer);
    if (dots->type != TOKEN_DOT) {
        fprintf(stderr, "Error: Expected '..' for range\n");
        token_free(dots);
        array_op_free(op);
        return NULL;
    }
    token_free(dots);
    
    // Parse end value
    Token* end_tok = lexer_next_token(lexer);
    if (end_tok->type != TOKEN_NUMBER) {
        fprintf(stderr, "Error: Expected number for range end\n");
        token_free(end_tok);
        array_op_free(op);
        return NULL;
    }
    token_free(end_tok);
    
    return op;
}

// Parse slice syntax: arr[start..end]
ArrayOperation* parse_array_slice_syntax(Lexer* lexer) {
    ArrayOperation* op = array_op_create(ARRAY_OP_SLICE_SYNTAX);
    
    // Expect '['
    Token* tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_LBRACKET) {
        fprintf(stderr, "Error: Expected '[' for slice\n");
        token_free(tok);
        array_op_free(op);
        return NULL;
    }
    token_free(tok);
    
    // Parse start index (or '..' for open start)
    tok = lexer_next_token(lexer);
    int has_start = (tok->type == TOKEN_NUMBER);
    if (has_start) {
        token_free(tok);
        tok = lexer_next_token(lexer);
    }
    
    // Expect '..'
    if (tok->type != TOKEN_DOT) {
        fprintf(stderr, "Error: Expected '..' in slice\n");
        token_free(tok);
        array_op_free(op);
        return NULL;
    }
    token_free(tok);
    
    tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_DOT) {
        fprintf(stderr, "Error: Expected '..' in slice\n");
        token_free(tok);
        array_op_free(op);
        return NULL;
    }
    token_free(tok);
    
    // Parse end index (or ']' for open end)
    tok = lexer_next_token(lexer);
    int has_end = (tok->type == TOKEN_NUMBER);
    if (has_end) {
        token_free(tok);
        tok = lexer_next_token(lexer);
    }
    
    // Expect ']'
    if (tok->type != TOKEN_RBRACKET) {
        fprintf(stderr, "Error: Expected ']' after slice\n");
        token_free(tok);
        array_op_free(op);
        return NULL;
    }
    token_free(tok);
    
    return op;
}

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
