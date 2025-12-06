#include "parser_core.h"
#include <stdlib.h>
#include <stdio.h>

Parser* parser_create(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

void parser_free(Parser* parser) {
    if (!parser) return;
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    free(parser);
}

void parser_advance(Parser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    parser->current_token = lexer_next_token(parser->lexer);
}

Token* parser_peek(Parser* parser) {
    return parser->current_token;
}

int parser_match(Parser* parser, TokenType type) {
    if (parser->current_token && parser->current_token->type == type) {
        parser_advance(parser);
        return 1;
    }
    return 0;
}

int parser_expect(Parser* parser, TokenType type, const char* error_msg) {
    if (parser->current_token && parser->current_token->type == type) {
        parser_advance(parser);
        return 1;
    }
    fprintf(stderr, "Parser error: %s\n", error_msg);
    return 0;
}
