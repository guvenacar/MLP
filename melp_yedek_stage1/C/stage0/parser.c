#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Parser* parser_create(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

void parser_free(Parser* parser) {
    if (parser->current_token) token_free(parser->current_token);
    free(parser);
}

static void advance(Parser* parser) {
    if (parser->current_token) token_free(parser->current_token);
    parser->current_token = lexer_next_token(parser->lexer);
}

static Declaration* parse_declaration(Parser* parser) {
    Declaration* decl = malloc(sizeof(Declaration));
    
    // Type
    if (parser->current_token->type == TOKEN_NUMERIC) {
        decl->type = VAR_NUMERIC;
    } else if (parser->current_token->type == TOKEN_TEXT) {
        decl->type = VAR_TEXT;
    } else if (parser->current_token->type == TOKEN_BOOLEAN) {
        decl->type = VAR_BOOLEAN;
    } else {
        fprintf(stderr, "Error: Expected type keyword\n");
        free(decl);
        return NULL;
    }
    advance(parser);
    
    // Name
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected identifier\n");
        free(decl);
        return NULL;
    }
    decl->name = strdup(parser->current_token->value);
    advance(parser);
    
    // =
    if (parser->current_token->type != TOKEN_ASSIGN) {
        fprintf(stderr, "Error: Expected '='\n");
        free(decl->name);
        free(decl);
        return NULL;
    }
    advance(parser);
    
    // Value
    if (parser->current_token->type == TOKEN_NUMBER) {
        decl->value = strdup(parser->current_token->value);
    } else if (parser->current_token->type == TOKEN_STRING) {
        decl->value = strdup(parser->current_token->value);
    } else if (parser->current_token->type == TOKEN_TRUE) {
        decl->value = strdup("true");
    } else if (parser->current_token->type == TOKEN_FALSE) {
        decl->value = strdup("false");
    } else {
        fprintf(stderr, "Error: Expected value\n");
        free(decl->name);
        free(decl);
        return NULL;
    }
    advance(parser);
    
    return decl;
}

Program* parser_parse(Parser* parser) {
    Program* program = malloc(sizeof(Program));
    program->capacity = 10;
    program->count = 0;
    program->declarations = malloc(sizeof(Declaration*) * program->capacity);
    
    while (parser->current_token->type != TOKEN_EOF) {
        Declaration* decl = parse_declaration(parser);
        if (decl == NULL) {
            program_free(program);
            return NULL;
        }
        
        if (program->count >= program->capacity) {
            program->capacity *= 2;
            program->declarations = realloc(program->declarations, 
                                           sizeof(Declaration*) * program->capacity);
        }
        program->declarations[program->count++] = decl;
    }
    
    return program;
}

void program_free(Program* program) {
    for (int i = 0; i < program->count; i++) {
        free(program->declarations[i]->name);
        free(program->declarations[i]->value);
        free(program->declarations[i]);
    }
    free(program->declarations);
    free(program);
}
