// Simple debug test
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int type;
    char* lexeme;
} Token;

int main() {
    Token* tokens = malloc(10 * sizeof(Token));
    memset(tokens, 0, 10 * sizeof(Token));
    
    tokens[0].type = 20;
    tokens[0].lexeme = strdup("fn");
    
    printf("Token: type=%d, lexeme=%s\n", tokens[0].type, tokens[0].lexeme);
    
    free(tokens[0].lexeme);
    free(tokens);
    
    printf("Test passed!\n");
    return 0;
}
