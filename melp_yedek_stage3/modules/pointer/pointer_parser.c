#include "pointer_parser.h"
#include "../arithmetic/arithmetic_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Parse pointer declaration
// Syntax: numeric* ptr = &x
Pointer* parse_pointer_declaration(Lexer* lexer) {
    Token* token = lexer_next_token(lexer);
    
    // Parse base type
    if (token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected type for pointer\n");
        token_free(token);
        return NULL;
    }
    
    char* base_type = strdup(token->value);
    token_free(token);
    
    // Expect '*' (would need TOKEN_MULTIPLY or TOKEN_STAR)
    token = lexer_next_token(lexer);
    // Assume '*' is TOKEN_MULTIPLY
    if (token->type != TOKEN_MULTIPLY) {
        fprintf(stderr, "Error: Expected '*' for pointer type\n");
        free(base_type);
        token_free(token);
        return NULL;
    }
    token_free(token);
    
    // Parse pointer variable name
    token = lexer_next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected pointer variable name\n");
        free(base_type);
        token_free(token);
        return NULL;
    }
    
    char* ptr_name = strdup(token->value);
    token_free(token);
    
    // Check for initialization
    token = lexer_next_token(lexer);
    char* init_expr = NULL;
    
    if (token->type == TOKEN_ASSIGN) {
        token_free(token);
        
        // Parse initialization expression (simplified)
        token = lexer_next_token(lexer);
        if (token->value) {
            init_expr = strdup(token->value);
        }
        token_free(token);
    } else {
        token_free(token);
    }
    
    // Create pointer (simplified - use default type)
    Pointer* ptr = pointer_create(ptr_name, PTR_VOID, base_type);
    
    free(base_type);
    free(ptr_name);
    if (init_expr) free(init_expr);
    
    return ptr;
}

// Parse address-of operation
// Syntax: &variable
AddressOf* parse_address_of(Lexer* lexer) {
    Token* token = lexer_next_token(lexer);
    
    // Expect '&' (would need TOKEN_AMPERSAND)
    // For now, assume we're called when & is detected
    
    // Parse variable name
    token = lexer_next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected variable name after &\n");
        token_free(token);
        return NULL;
    }
    
    AddressOf* addr = pointer_create_address_of(token->value);
    token_free(token);
    
    return addr;
}

// Parse dereference operation
// Syntax: *pointer
Dereference* parse_dereference(Lexer* lexer) {
    Token* token = lexer_next_token(lexer);
    
    // Expect '*'
    if (token->type != TOKEN_MULTIPLY) {
        fprintf(stderr, "Error: Expected '*' for dereference\n");
        token_free(token);
        return NULL;
    }
    token_free(token);
    
    // Parse pointer expression
    token = lexer_next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected pointer name\n");
        token_free(token);
        return NULL;
    }
    
    Dereference* deref = pointer_create_deref(token->value);
    token_free(token);
    
    return deref;
}

// Parse pointer arithmetic
// Syntax: ptr + 5, ptr - 3, ptr1 - ptr2
PointerArith* parse_pointer_arith(Lexer* lexer) {
    Token* token = lexer_next_token(lexer);
    
    // Parse left pointer
    if (token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected pointer name\n");
        token_free(token);
        return NULL;
    }
    
    char* left = strdup(token->value);
    token_free(token);
    
    // Parse operator (+, -)
    token = lexer_next_token(lexer);
    PtrArithOp op;
    
    if (token->type == TOKEN_PLUS) {
        op = PTR_ADD;
    } else if (token->type == TOKEN_MINUS) {
        op = PTR_SUB;
    } else {
        fprintf(stderr, "Error: Expected + or - for pointer arithmetic\n");
        free(left);
        token_free(token);
        return NULL;
    }
    token_free(token);
    
    // Parse right operand
    Parser temp_parser;
    temp_parser.lexer = lexer;
    temp_parser.current_token = lexer_next_token(lexer);
    
    Expression* right = expression_parse(&temp_parser);
    
    if (!right) {
        fprintf(stderr, "Error: Failed to parse right operand\n");
        free(left);
        return NULL;
    }
    
    // Convert Expression* to long offset for pointer_create_arith
    // For now, assuming right is a number expression
    long offset = 0;  // TODO: Extract value from Expression in later stages
    
    PointerArith* arith = pointer_create_arith(left, op, offset);
    free(left);
    
    return arith;
}
