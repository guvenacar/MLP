#include "lexer.c"
#include <stdio.h>

int main() {
    const char* test_code = "try\n    print(42)\ncatch\n    print(99)\nend try\n";
    
    Lexer* lexer = lexer_create(test_code);
    Token* token;
    
    while ((token = lexer_next_token(lexer))->type != TOKEN_EOF) {
        printf("Token: type=%d, value='%s', line=%d\n", 
               token->type, token->value ? token->value : "(null)", token->line);
    }
    
    return 0;
}
