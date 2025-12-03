#include "print_parser.h"
#include "../../lexer.h"
#include <stdio.h>
#include <stdlib.h>

PrintStatement* parse_print_statement(Lexer* lexer) {
    Token* tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_PRINT) {
        fprintf(stderr, "Error: Expected 'print' keyword\n");
        token_free(tok);
        return NULL;
    }
    token_free(tok);
    
    // For now, skip expression parsing - would need expression parser
    // Just create empty print statement
    PrintStatement* stmt = print_statement_create(NULL);
    
    // Skip to newline or semicolon
    tok = lexer_next_token(lexer);
    while (tok->type != TOKEN_SEMICOLON && tok->type != TOKEN_EOF) {
        token_free(tok);
        tok = lexer_next_token(lexer);
    }
    token_free(tok);
    
    return stmt;
}
