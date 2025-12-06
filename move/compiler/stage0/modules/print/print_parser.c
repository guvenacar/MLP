#include "print_parser.h"
#include "../../lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PrintStatement* parse_print_statement(Lexer* lexer) {
    // Expect 'print' keyword
    Token* tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_PRINT) {
        fprintf(stderr, "Error: Expected 'print' keyword\n");
        token_free(tok);
        return NULL;
    }
    token_free(tok);
    
    // Expect '('
    tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_LPAREN) {
        fprintf(stderr, "Error: Expected '(' after 'print'\n");
        token_free(tok);
        return NULL;
    }
    token_free(tok);
    
    // Expect string literal
    tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_STRING) {
        fprintf(stderr, "Error: Expected string literal in print()\n");
        token_free(tok);
        return NULL;
    }
    
    // Create statement with string content
    // For now, we'll store the string in the Expression* value field
    // We need to create a simple expression structure
    PrintStatement* stmt = malloc(sizeof(PrintStatement));
    stmt->value = (Expression*)strdup(tok->value);  // Temporary: store string as pointer
    token_free(tok);
    
    // Expect ')'
    tok = lexer_next_token(lexer);
    if (tok->type != TOKEN_RPAREN) {
        fprintf(stderr, "Error: Expected ')' after string\n");
        token_free(tok);
        free(stmt->value);
        free(stmt);
        return NULL;
    }
    token_free(tok);
    
    return stmt;
}
