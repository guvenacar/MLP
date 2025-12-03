#include "memory_parser.h"
#include "../arithmetic/arithmetic_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Parse memory allocation
// Syntax: allocate(size) or new Type[count]
MemoryAlloc* parse_memory_alloc(Lexer* lexer) {
    Token* token = lexer_next_token(lexer);
    
    // Check for 'allocate' or 'new' keyword
    if (token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected 'allocate' or 'new' keyword\n");
        token_free(token);
        return NULL;
    }
    
    int is_new = (strcmp(token->value, "new") == 0);
    token_free(token);
    
    if (is_new) {
        // Parse: new Type[count]
        token = lexer_next_token(lexer);
        if (token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected type after 'new'\n");
            token_free(token);
            return NULL;
        }
        
        char* type = strdup(token->value);
        token_free(token);
        
        // Expect '['
        token = lexer_next_token(lexer);
        if (token->type != TOKEN_LBRACKET) {
            fprintf(stderr, "Error: Expected '[' after type\n");
            free(type);
            token_free(token);
            return NULL;
        }
        token_free(token);
        
        // Parse count expression
        Parser temp_parser;
        temp_parser.lexer = lexer;
        temp_parser.current_token = lexer_next_token(lexer);
        Expression* count = expression_parse(&temp_parser);
        
        if (!count) {
            fprintf(stderr, "Error: Failed to parse array count\n");
            free(type);
            return NULL;
        }
        
        // Expect ']'
        token = temp_parser.current_token;
        if (token->type != TOKEN_RBRACKET) {
            fprintf(stderr, "Error: Expected ']'\n");
            free(type);
            expression_free(count);
            return NULL;
        }
        
        MemoryAlloc* alloc = memory_create_alloc(sizeof(void*), ALLOC_HEAP, type);
        free(type);
        return alloc;
        
    } else {
        // Parse: allocate(size)
        // Expect '('
        token = lexer_next_token(lexer);
        if (token->type != TOKEN_LPAREN) {
            fprintf(stderr, "Error: Expected '(' after 'allocate'\n");
            token_free(token);
            return NULL;
        }
        token_free(token);
        
        // Parse size expression
        Parser temp_parser;
        temp_parser.lexer = lexer;
        temp_parser.current_token = lexer_next_token(lexer);
        Expression* size = expression_parse(&temp_parser);
        
        if (!size) {
            fprintf(stderr, "Error: Failed to parse size\n");
            return NULL;
        }
        
        // Expect ')'
        token = temp_parser.current_token;
        if (token->type != TOKEN_RPAREN) {
            fprintf(stderr, "Error: Expected ')'\n");
            expression_free(size);
            return NULL;
        }
        
        MemoryAlloc* alloc = memory_create_alloc(sizeof(void*), ALLOC_HEAP, "byte");
        return alloc;
    }
}

// Parse memory free
// Syntax: free(var) or delete var
MemoryFree* parse_memory_free(Lexer* lexer) {
    Token* token = lexer_next_token(lexer);
    
    // Check for 'free' or 'delete' keyword
    if (token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected 'free' or 'delete' keyword\n");
        token_free(token);
        return NULL;
    }
    
    int is_delete = (strcmp(token->value, "delete") == 0);
    token_free(token);
    
    if (is_delete) {
        // Parse: delete var
        token = lexer_next_token(lexer);
        if (token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected variable after 'delete'\n");
            token_free(token);
            return NULL;
        }
        
        MemoryFree* free_op = memory_create_free(token->value);
        token_free(token);
        return free_op;
        
    } else {
        // Parse: free(var)
        // Expect '('
        token = lexer_next_token(lexer);
        if (token->type != TOKEN_LPAREN) {
            fprintf(stderr, "Error: Expected '(' after 'free'\n");
            token_free(token);
            return NULL;
        }
        token_free(token);
        
        // Parse variable
        token = lexer_next_token(lexer);
        if (token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected variable name\n");
            token_free(token);
            return NULL;
        }
        
        MemoryFree* free_op = memory_create_free(token->value);
        token_free(token);
        
        // Expect ')'
        token = lexer_next_token(lexer);
        if (token->type != TOKEN_RPAREN) {
            fprintf(stderr, "Error: Expected ')'\n");
            memory_free_free(free_op);
            token_free(token);
            return NULL;
        }
        token_free(token);
        
        return free_op;
    }
}

// Parse memory copy
// Syntax: copy(dest, src, size)
MemoryCopy* parse_memory_copy(Lexer* lexer) {
    Token* token = lexer_next_token(lexer);
    
    // Expect 'copy' keyword
    // Skip keyword check, expect '('
    token = lexer_next_token(lexer);
    if (token->type != TOKEN_LPAREN) {
        fprintf(stderr, "Error: Expected '(' after 'copy'\n");
        token_free(token);
        return NULL;
    }
    token_free(token);
    
    // Parse dest, src, size
    Parser temp_parser;
    temp_parser.lexer = lexer;
    
    // Parse dest
    temp_parser.current_token = lexer_next_token(lexer);
    Expression* dest = expression_parse(&temp_parser);
    if (!dest) return NULL;
    
    // Expect ','
    if (temp_parser.current_token->type != TOKEN_COMMA) {
        expression_free(dest);
        return NULL;
    }
    token_free(temp_parser.current_token);
    
    // Parse src
    temp_parser.current_token = lexer_next_token(lexer);
    Expression* src = expression_parse(&temp_parser);
    if (!src) {
        expression_free(dest);
        return NULL;
    }
    
    // Expect ','
    if (temp_parser.current_token->type != TOKEN_COMMA) {
        expression_free(dest);
        expression_free(src);
        return NULL;
    }
    token_free(temp_parser.current_token);
    
    // Parse size
    temp_parser.current_token = lexer_next_token(lexer);
    Expression* size = expression_parse(&temp_parser);
    if (!size) {
        expression_free(dest);
        expression_free(src);
        return NULL;
    }
    
    // Expect ')'
    if (temp_parser.current_token->type != TOKEN_RPAREN) {
        expression_free(dest);
        expression_free(src);
        expression_free(size);
        return NULL;
    }
    
    MemoryCopy* copy = memory_create_copy(dest, src, size);
    return copy;
}

// Parse memory zero
// Syntax: zero(var, size)
MemoryZero* parse_memory_zero(Lexer* lexer) {
    Token* token = lexer_next_token(lexer);
    
    // Expect '('
    token = lexer_next_token(lexer);
    if (token->type != TOKEN_LPAREN) {
        fprintf(stderr, "Error: Expected '('\n");
        token_free(token);
        return NULL;
    }
    token_free(token);
    
    // Parse var and size
    Parser temp_parser;
    temp_parser.lexer = lexer;
    
    temp_parser.current_token = lexer_next_token(lexer);
    Expression* var = expression_parse(&temp_parser);
    if (!var) return NULL;
    
    // Expect ','
    if (temp_parser.current_token->type != TOKEN_COMMA) {
        expression_free(var);
        return NULL;
    }
    token_free(temp_parser.current_token);
    
    temp_parser.current_token = lexer_next_token(lexer);
    Expression* size = expression_parse(&temp_parser);
    if (!size) {
        expression_free(var);
        return NULL;
    }
    
    // Expect ')'
    if (temp_parser.current_token->type != TOKEN_RPAREN) {
        expression_free(var);
        expression_free(size);
        return NULL;
    }
    
    MemoryZero* zero = memory_create_zero(var, size);
    return zero;
}

// Parse memory realloc
// Syntax: resize(var, new_size)
MemoryRealloc* parse_memory_realloc(Lexer* lexer) {
    Token* token = lexer_next_token(lexer);
    
    // Expect '('
    token = lexer_next_token(lexer);
    if (token->type != TOKEN_LPAREN) {
        fprintf(stderr, "Error: Expected '('\n");
        token_free(token);
        return NULL;
    }
    token_free(token);
    
    // Parse var and new_size
    Parser temp_parser;
    temp_parser.lexer = lexer;
    
    temp_parser.current_token = lexer_next_token(lexer);
    Expression* var = expression_parse(&temp_parser);
    if (!var) return NULL;
    
    // Expect ','
    if (temp_parser.current_token->type != TOKEN_COMMA) {
        expression_free(var);
        return NULL;
    }
    token_free(temp_parser.current_token);
    
    temp_parser.current_token = lexer_next_token(lexer);
    Expression* new_size = expression_parse(&temp_parser);
    if (!new_size) {
        expression_free(var);
        return NULL;
    }
    
    // Expect ')'
    if (temp_parser.current_token->type != TOKEN_RPAREN) {
        expression_free(var);
        expression_free(new_size);
        return NULL;
    }
    
    MemoryRealloc* realloc_op = memory_create_realloc(var, 0, new_size);
    return realloc_op;
}
