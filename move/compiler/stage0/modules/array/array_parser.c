#include "array_parser.h"
#include "../arithmetic/arithmetic_parser.h"
#include "../expression/expression.h"
#include "../expression/expression_parser.h"
#include <stdlib.h>
#include <stdio.h>

ArrayParser* array_parser_create(Lexer* lexer) {
    ArrayParser* parser = malloc(sizeof(ArrayParser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

void array_parser_free(ArrayParser* parser) {
    if (parser->current_token) token_free(parser->current_token);
    free(parser);
}

void collection_free(Collection* coll) {
    if (!coll) return;
    
    if (coll->type == COLL_ARRAY) {
        for (int i = 0; i < coll->data.array.length; i++) {
            free(coll->data.array.elements[i]);
        }
        free(coll->data.array.elements);
    } else if (coll->type == COLL_LIST) {
        for (int i = 0; i < coll->data.list.length; i++) {
            free(coll->data.list.elements[i]);
        }
        free(coll->data.list.elements);
        free(coll->data.list.element_types);
    } else if (coll->type == COLL_TUPLE) {
        for (int i = 0; i < coll->data.tuple.length; i++) {
            free(coll->data.tuple.elements[i]);
        }
        free(coll->data.tuple.elements);
        free(coll->data.tuple.element_types);
    }
    
    free(coll);
}

// Helper: Advance to next token
static void advance(ArrayParser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    parser->current_token = lexer_next_token(parser->lexer);
}

// Helper: Check if current token matches expected type
static int match(ArrayParser* parser, TokenType type) {
    return parser->current_token && parser->current_token->type == type;
}

// Helper: Expect a specific token or error
static int expect(ArrayParser* parser, TokenType type, const char* msg) {
    if (!match(parser, type)) {
        fprintf(stderr, "Error: %s\n", msg);
        return 0;
    }
    advance(parser);
    return 1;
}

// Parse array literal: [1, 2, 3, 4]
// Homogeneous, comma-separated
Collection* parse_array_literal(ArrayParser* parser) {
    if (!expect(parser, TOKEN_LBRACKET, "Expected '[' to start array")) {
        return NULL;
    }
    
    // Parse elements
    int capacity = 4;
    int length = 0;
    Expression** elements = malloc(sizeof(Expression*) * capacity);
    
    // Convert to common Parser
    Parser temp_parser;
    temp_parser.lexer = parser->lexer;
    temp_parser.current_token = parser->current_token;
    
    // Parse first element (if not empty array)
    if (!match(parser, TOKEN_RBRACKET)) {
        elements[0] = expression_parse(&temp_parser);
        parser->current_token = temp_parser.current_token;
        
        if (!elements[0]) {
            fprintf(stderr, "Error: Failed to parse array element\n");
            free(elements);
            return NULL;
        }
        length = 1;
        
        // Parse remaining elements
        while (match(parser, TOKEN_COMMA)) {
            advance(parser);  // consume ','
            
            // Check for capacity
            if (length >= capacity) {
                capacity *= 2;
                elements = realloc(elements, sizeof(Expression*) * capacity);
            }
            
            temp_parser.current_token = parser->current_token;
            elements[length] = expression_parse(&temp_parser);
            parser->current_token = temp_parser.current_token;
            
            if (!elements[length]) {
                fprintf(stderr, "Error: Failed to parse array element\n");
                for (int i = 0; i < length; i++) {
                    expression_free(elements[i]);
                }
                free(elements);
                return NULL;
            }
            length++;
        }
    }
    
    // Expect ']'
    if (!expect(parser, TOKEN_RBRACKET, "Expected ']' to close array")) {
        for (int i = 0; i < length; i++) {
            expression_free(elements[i]);
        }
        free(elements);
        return NULL;
    }
    
    // Build array collection
    Collection* coll = malloc(sizeof(Collection));
    coll->type = COLL_ARRAY;
    coll->data.array.element_type = VAR_NUMERIC;  // Default, should be inferred
    coll->data.array.capacity = capacity;
    coll->data.array.length = length;
    coll->data.array.elements = (void**)elements;
    coll->data.array.is_dynamic = 1;  // Heap allocated
    
    return coll;
}

// Parse list literal: (10; 20; 30;)
// Heterogeneous, semicolon-separated, trailing semicolon REQUIRED
Collection* parse_list_literal(ArrayParser* parser) {
    if (!expect(parser, TOKEN_LPAREN, "Expected '(' to start list")) {
        return NULL;
    }
    
    // Parse elements
    int capacity = 4;
    int length = 0;
    Expression** elements = malloc(sizeof(Expression*) * capacity);
    VarType* types = malloc(sizeof(VarType) * capacity);
    
    // Convert to common Parser
    Parser temp_parser;
    temp_parser.lexer = parser->lexer;
    temp_parser.current_token = parser->current_token;
    
    // Parse elements until we hit ')'
    while (!match(parser, TOKEN_RPAREN)) {
        // Check for capacity
        if (length >= capacity) {
            capacity *= 2;
            elements = realloc(elements, sizeof(Expression*) * capacity);
            types = realloc(types, sizeof(VarType) * capacity);
        }
        
        // Parse element
        temp_parser.current_token = parser->current_token;
        elements[length] = expression_parse(&temp_parser);
        parser->current_token = temp_parser.current_token;
        
        if (!elements[length]) {
            fprintf(stderr, "Error: Failed to parse list element\n");
            for (int i = 0; i < length; i++) {
                expression_free(elements[i]);
            }
            free(elements);
            free(types);
            return NULL;
        }
        
        types[length] = VAR_NUMERIC;  // Default, should be inferred
        length++;
        
        // Expect ';' after each element (including last)
        if (!expect(parser, TOKEN_SEMICOLON, "Expected ';' after list element")) {
            for (int i = 0; i < length; i++) {
                expression_free(elements[i]);
            }
            free(elements);
            free(types);
            return NULL;
        }
    }
    
    // Expect ')'
    if (!expect(parser, TOKEN_RPAREN, "Expected ')' to close list")) {
        for (int i = 0; i < length; i++) {
            expression_free(elements[i]);
        }
        free(elements);
        free(types);
        return NULL;
    }
    
    // Build list collection
    Collection* coll = malloc(sizeof(Collection));
    coll->type = COLL_LIST;
    coll->data.list.capacity = capacity;
    coll->data.list.length = length;
    coll->data.list.elements = (void**)elements;
    coll->data.list.element_types = types;
    
    return coll;
}

// Parse tuple literal: <"name", 25, true>
// Heterogeneous, immutable, comma-separated
Collection* parse_tuple_literal(ArrayParser* parser) {
    // Note: '<' and '>' are TOKEN_LESS and TOKEN_GREATER
    if (!expect(parser, TOKEN_LESS, "Expected '<' to start tuple")) {
        return NULL;
    }
    
    // Parse elements
    int capacity = 4;
    int length = 0;
    Expression** elements = malloc(sizeof(Expression*) * capacity);
    VarType* types = malloc(sizeof(VarType) * capacity);
    
    // Convert to common Parser
    Parser temp_parser;
    temp_parser.lexer = parser->lexer;
    temp_parser.current_token = parser->current_token;
    
    // Parse first element
    if (!match(parser, TOKEN_GREATER)) {
        elements[0] = expression_parse(&temp_parser);
        parser->current_token = temp_parser.current_token;
        
        if (!elements[0]) {
            fprintf(stderr, "Error: Failed to parse tuple element\n");
            free(elements);
            free(types);
            return NULL;
        }
        
        types[0] = VAR_NUMERIC;  // Default, should be inferred
        length = 1;
        
        // Parse remaining elements
        while (match(parser, TOKEN_COMMA)) {
            advance(parser);  // consume ','
            
            // Check for capacity
            if (length >= capacity) {
                capacity *= 2;
                elements = realloc(elements, sizeof(Expression*) * capacity);
                types = realloc(types, sizeof(VarType) * capacity);
            }
            
            temp_parser.current_token = parser->current_token;
            elements[length] = expression_parse(&temp_parser);
            parser->current_token = temp_parser.current_token;
            
            if (!elements[length]) {
                fprintf(stderr, "Error: Failed to parse tuple element\n");
                for (int i = 0; i < length; i++) {
                    expression_free(elements[i]);
                }
                free(elements);
                free(types);
                return NULL;
            }
            
            types[length] = VAR_NUMERIC;  // Default, should be inferred
            length++;
        }
    }
    
    // Expect '>'
    if (!expect(parser, TOKEN_GREATER, "Expected '>' to close tuple")) {
        for (int i = 0; i < length; i++) {
            expression_free(elements[i]);
        }
        free(elements);
        free(types);
        return NULL;
    }
    
    // Build tuple collection
    Collection* coll = malloc(sizeof(Collection));
    coll->type = COLL_TUPLE;
    coll->data.tuple.length = length;
    coll->data.tuple.elements = (void**)elements;
    coll->data.tuple.element_types = types;
    
    return coll;
}

// Parse index access
// Array: arr[0] - bracket notation
// List: list(0) - parenthesis notation (NO SPACE between identifier and paren)
Expression* parse_index_access(ArrayParser* parser) {
    // This should be called when we have an identifier followed by '[' or '('
    // For now, return a placeholder expression
    
    // Convert to common Parser
    Parser temp_parser;
    temp_parser.lexer = parser->lexer;
    temp_parser.current_token = parser->current_token;
    
    // Parse base expression (identifier)
    Expression* base = expression_parse(&temp_parser);
    parser->current_token = temp_parser.current_token;
    
    if (!base) {
        fprintf(stderr, "Error: Failed to parse base expression for index access\n");
        return NULL;
    }
    
    // Check for '[' (array access) or '(' (list access)
    if (match(parser, TOKEN_LBRACKET)) {
        advance(parser);  // consume '['
        
        // Parse index expression
        temp_parser.current_token = parser->current_token;
        Expression* index = expression_parse(&temp_parser);
        parser->current_token = temp_parser.current_token;
        
        if (!index) {
            fprintf(stderr, "Error: Failed to parse array index\n");
            expression_free(base);
            return NULL;
        }
        
        // Expect ']'
        if (!expect(parser, TOKEN_RBRACKET, "Expected ']' after array index")) {
            expression_free(base);
            expression_free(index);
            return NULL;
        }
        
        // Build index access expression (simplified for now)
        // In real implementation, this would be a proper IndexAccess AST node
        expression_free(index);  // Cleanup for placeholder
        return base;
        
    } else if (match(parser, TOKEN_LPAREN)) {
        advance(parser);  // consume '('
        
        // Parse index expression
        temp_parser.current_token = parser->current_token;
        Expression* index = expression_parse(&temp_parser);
        parser->current_token = temp_parser.current_token;
        
        if (!index) {
            fprintf(stderr, "Error: Failed to parse list index\n");
            expression_free(base);
            return NULL;
        }
        
        // Expect ')'
        if (!expect(parser, TOKEN_RPAREN, "Expected ')' after list index")) {
            expression_free(base);
            expression_free(index);
            return NULL;
        }
        
        // Build index access expression (simplified for now)
        expression_free(index);  // Cleanup for placeholder
        return base;
    }
    
    // No index access, just return base expression
    return base;
}
