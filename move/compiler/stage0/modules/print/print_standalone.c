#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"
#include "print_parser.h"
#include "print_codegen.h"

int main(int argc, char **argv) {
    if(argc < 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    printf("🔧 Print Module - Parser & Codegen\n");
    printf("====================================\n");
    printf("Input:  %s\n", argv[1]);
    printf("Output: %s\n\n", argv[2]);
    
    // Read source file
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open %s\n", argv[1]);
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *src = malloc(sz + 1);
    fread(src, 1, sz, f);
    src[sz] = 0;
    fclose(f);
    
    // Create lexer
    Lexer* lexer = lexer_create(src);
    
    // Open output file
    FILE *out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create %s\n", argv[2]);
        free(src);
        lexer_free(lexer);
        return 1;
    }
    
    // Parse and generate code for all print statements
    int stmt_count = 0;
    Token* tok;
    
    while (1) {
        tok = lexer_next_token(lexer);
        
        if (tok->type == TOKEN_EOF) {
            token_free(tok);
            break;
        }
        
        if (tok->type == TOKEN_PRINT) {
            // Put token back by recreating lexer at current position
            // (Simple approach: adjust position back)
            lexer->pos -= strlen("print");
            token_free(tok);
            
            // Parse print statement
            PrintStatement* stmt = parse_print_statement(lexer);
            if (stmt) {
                printf("  ✓ Parsed print statement\n");
                
                // Generate code
                codegen_print_statement(out, stmt);
                
                // Free statement
                if (stmt->value) free(stmt->value);
                free(stmt);
                
                stmt_count++;
            } else {
                fprintf(stderr, "Error: Failed to parse print statement\n");
                free(src);
                lexer_free(lexer);
                fclose(out);
                return 1;
            }
        } else {
            token_free(tok);
        }
    }
    
    printf("\n✅ Generated %d print statements\n", stmt_count);
    
    // Finalize code generation (add exit)
    codegen_print_finalize(out);
    
    // Cleanup
    free(src);
    lexer_free(lexer);
    fclose(out);
    
    return 0;
}

