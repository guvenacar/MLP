#include "array_parser.h"
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
    
    // TODO: Free elements based on collection type
    free(coll);
}

// Placeholder: Parse array literal
Collection* parse_array_literal(ArrayParser* parser) {
    // TODO: Implement in Stage 1
    // Expected syntax: [1, 2, 3, 4]
    // Homogeneous, comma-separated
    fprintf(stderr, "TODO: parse_array_literal not implemented\n");
    return NULL;
}

// Placeholder: Parse list literal
Collection* parse_list_literal(ArrayParser* parser) {
    // TODO: Implement in Stage 1
    // Expected syntax: (10; 20; 30;)
    // Heterogeneous, semicolon-separated, trailing semicolon REQUIRED
    fprintf(stderr, "TODO: parse_list_literal not implemented\n");
    return NULL;
}

// Placeholder: Parse tuple literal
Collection* parse_tuple_literal(ArrayParser* parser) {
    // TODO: Implement in Stage 1
    // Expected syntax: <"name", 25, true>
    // Heterogeneous, immutable, comma-separated
    fprintf(stderr, "TODO: parse_tuple_literal not implemented\n");
    return NULL;
}

// Placeholder: Parse index access
struct Expression* parse_index_access(ArrayParser* parser) {
    // TODO: Implement in Stage 1
    // Array: arr[0] - bracket notation
    // List: list(0) - parenthesis notation (NO SPACE between identifier and paren)
    fprintf(stderr, "TODO: parse_index_access not implemented\n");
    return NULL;
}
