#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type_alias_parser.h"
#include "type_alias.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.mlp>\n", argv[0]);
        return 1;
    }
    
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", argv[1]);
        return 1;
    }
    
    // Read file content
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* source = malloc(size + 1);
    fread(source, 1, size, file);
    source[size] = '\0';
    fclose(file);
    
    // Create lexer
    Lexer* lexer = lexer_create(source);
    
    // Parse type aliases
    Token* tok = lexer_next_token(lexer);
    while (tok && tok->type != TOKEN_EOF) {
        if (tok->type == TOKEN_IDENTIFIER && strcmp(tok->value, "type") == 0) {
            token_free(tok);
            TypeAlias* alias = parse_type_alias(lexer);
            if (alias) {
                printf("Type alias: %s -> %s\n", alias->alias_name, alias->target_type);
            }
            tok = lexer_next_token(lexer);
        } else {
            token_free(tok);
            tok = lexer_next_token(lexer);
        }
    }
    
    if (tok) token_free(tok);
    
    // Clean up
    type_alias_clear_all();
    lexer_free(lexer);
    free(source);
    
    printf("✅ Type alias parsing completed!\n");
    return 0;
}
