#ifndef ARRAY_PARSER_H
#define ARRAY_PARSER_H

#include "array.h"
#include "../../lexer.h"
#include "../parser_core/parser_core.h"

// Parser for arrays, lists, and tuples
typedef struct {
    Lexer* lexer;
    Token* current_token;
} ArrayParser;

// Parser functions
ArrayParser* array_parser_create(Lexer* lexer);
void array_parser_free(ArrayParser* parser);

// Parse collection literals
// Array: [1, 2, 3, 4]
Collection* parse_array_literal(ArrayParser* parser);

// List: (10; 20; 30;) - semicolon separated, trailing semicolon required
Collection* parse_list_literal(ArrayParser* parser);

// Tuple: <"name", 25, true>
Collection* parse_tuple_literal(ArrayParser* parser);

// Parse array/list index access: arr[0] or list(0)
struct Expression* parse_index_access(ArrayParser* parser);

// Free collection
void collection_free(Collection* coll);

#endif
