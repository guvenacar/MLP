#include "comments_parser.h"
#include <stdlib.h>

// Create comments parser (not used but exists for consistency)
CommentsParser* comments_parser_create(Lexer* lexer) {
    CommentsParser* parser = malloc(sizeof(CommentsParser));
    parser->lexer = lexer;
    parser->current_token = NULL;
    return parser;
}

void comments_parser_free(CommentsParser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    free(parser);
}
