#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "string_operations_parser.h"
#include "string_operations.h"
#include "../../lexer.h"

// Parse string concatenation
StringOperation* parse_string_concat(Lexer* lexer) {
    StringOperation* op = string_op_create(STRING_OP_CONCAT);
    
    // First operand already parsed (string literal or variable)
    // Just mark position for now
    
    Token* tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_PLUS) {
        fprintf(stderr, "Error: Expected '+' for string concatenation\n");
        token_free(tok);
        string_op_free(op);
        return NULL;
    }
    token_free(tok);
    
    // Parse second operand
    tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_STRING && tok->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected string or variable after '+'\n");
        token_free(tok);
        string_op_free(op);
        return NULL;
    }
    token_free(tok);
    
    return op;
}

// Parse string function calls
StringOperation* parse_string_function(Lexer* lexer, const char* func_name) {
    StringOperation* op = NULL;
    
    // Determine operation type
    if (strcmp(func_name, "length") == 0) {
        op = string_op_create(STRING_OP_LENGTH);
    } else if (strcmp(func_name, "substring") == 0) {
        op = string_op_create(STRING_OP_SUBSTRING);
    } else if (strcmp(func_name, "indexOf") == 0) {
        op = string_op_create(STRING_OP_INDEXOF);
    } else if (strcmp(func_name, "charAt") == 0) {
        op = string_op_create(STRING_OP_CHARAT);
    } else if (strcmp(func_name, "toUpper") == 0) {
        op = string_op_create(STRING_OP_UPPER);
    } else if (strcmp(func_name, "toLower") == 0) {
        op = string_op_create(STRING_OP_LOWER);
    } else {
        fprintf(stderr, "Error: Unknown string function '%s'\n", func_name);
        return NULL;
    }
    
    // Parse function call: funcName(arg1, arg2, ...)
    Token* tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_LPAREN) {
        fprintf(stderr, "Error: Expected '(' after function name\n");
        token_free(tok);
        string_op_free(op);
        return NULL;
    }
    token_free(tok);
    
    // Parse arguments (simplified - just skip to closing paren)
    int arg_count = 0;
    int paren_depth = 1;
    
    while (paren_depth > 0) {
        tok = lexer_next_token(lexer);
        if (tok->type == TOKEN_EOF) {
            fprintf(stderr, "Error: Unexpected EOF in function call\n");
            token_free(tok);
            string_op_free(op);
            return NULL;
        }
        
        if (tok->type == TOKEN_LPAREN) {
            paren_depth++;
        } else if (tok->type == TOKEN_RPAREN) {
            paren_depth--;
        } else if (tok->type == TOKEN_COMMA && paren_depth == 1) {
            arg_count++;
        }
        
        token_free(tok);
    }
    
    return op;
}

// TIER 1: Parse string interpolation: "Hello ${name}!"
StringOperation* parse_string_interpolation(Lexer* lexer, const char* template_str) {
    StringOperation* op = string_op_create(STRING_OP_INTERPOLATE);
    
    // Count interpolation expressions
    int expr_count = 0;
    const char* p = template_str;
    while ((p = strstr(p, "${")) != NULL) {
        expr_count++;
        p += 2;
    }
    
    // Store template for later processing
    op->operand1 = (void*)template_str;
    
    return op;
}

// TIER 1: Parse multi-line string: """text"""
StringOperation* parse_multiline_string(Lexer* lexer) {
    StringOperation* op = string_op_create(STRING_OP_MULTILINE);
    
    // Expect opening """
    Token* tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_STRING) {
        fprintf(stderr, "Error: Expected string after \"\"\"\n");
        token_free(tok);
        string_op_free(op);
        return NULL;
    }
    
    // Collect multi-line content
    char* content = strdup(tok->value);
    token_free(tok);
    
    // Expect closing """ (simplified - assumes single token)
    op->operand1 = content;
    
    return op;
}
