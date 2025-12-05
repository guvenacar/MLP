#ifndef COMMENTS_PARSER_H
#define COMMENTS_PARSER_H

#include "../../lexer.h"

// Comments are handled at lexer level, no parser needed
// This file exists for consistency with modular architecture

// Parser structure (empty for comments)
typedef struct {
    Lexer* lexer;
    Token* current_token;
} CommentsParser;

// Create comments parser (not used but exists for consistency)
CommentsParser* comments_parser_create(Lexer* lexer);
void comments_parser_free(CommentsParser* parser);

#endif // COMMENTS_PARSER_H
