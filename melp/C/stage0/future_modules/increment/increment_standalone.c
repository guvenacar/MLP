#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"

// Increment/Decrement Expression Structure
typedef struct IncrementExpr {
    char variable[256];    // variable name
    int is_increment;      // 1 for ++, 0 for --
    int is_prefix;         // 1 for ++x, 0 for x++
    struct IncrementExpr *next;
} IncrementExpr;

static IncrementExpr *incr_head = NULL;
static IncrementExpr *incr_tail = NULL;

// Parse increment/decrement: x++, ++x, x--, --x
void increment_parse(Lexer *lexer) {
    Token *tok = lexer_next_token(lexer);
    Token *prev = NULL;
    
    while (tok->type != TOKEN_EOF) {
        // Check for prefix: ++ x or -- x
        if ((strcmp(tok->value, "++") == 0 || strcmp(tok->value, "--") == 0)) {
            Token *var_tok = lexer_next_token(lexer);
            if (var_tok->type == TOKEN_IDENTIFIER) {
                IncrementExpr *expr = malloc(sizeof(IncrementExpr));
                strcpy(expr->variable, var_tok->value);
                expr->is_increment = (strcmp(tok->value, "++") == 0);
                expr->is_prefix = 1;
                expr->next = NULL;
                
                if (incr_head == NULL) {
                    incr_head = expr;
                    incr_tail = expr;
                } else {
                    incr_tail->next = expr;
                    incr_tail = expr;
                }
                
                printf("  ✓ %s%s (prefix)\n", tok->value, expr->variable);
                
                token_free(tok);
                token_free(var_tok);
                tok = lexer_next_token(lexer);
                prev = NULL;
                continue;
            }
            token_free(var_tok);
        }
        
        // Check for postfix: x ++ or x --
        if (prev && prev->type == TOKEN_IDENTIFIER) {
            if (strcmp(tok->value, "++") == 0 || strcmp(tok->value, "--") == 0) {
                IncrementExpr *expr = malloc(sizeof(IncrementExpr));
                strcpy(expr->variable, prev->value);
                expr->is_increment = (strcmp(tok->value, "++") == 0);
                expr->is_prefix = 0;
                expr->next = NULL;
                
                if (incr_head == NULL) {
                    incr_head = expr;
                    incr_tail = expr;
                } else {
                    incr_tail->next = expr;
                    incr_tail = expr;
                }
                
                printf("  ✓ %s%s (postfix)\n", expr->variable, tok->value);
                
                token_free(prev);
                token_free(tok);
                tok = lexer_next_token(lexer);
                prev = NULL;
                continue;
            }
        }
        
        if (prev) token_free(prev);
        prev = tok;
        tok = lexer_next_token(lexer);
    }
    
    if (prev) token_free(prev);
    token_free(tok);
}

// Generate assembly for increment/decrement
void increment_codegen(FILE *out) {
    int incr_count = 0;
    IncrementExpr *expr = incr_head;
    while (expr != NULL) {
        incr_count++;
        expr = expr->next;
    }
    
    if (incr_count == 0) {
        return;
    }
    
    fprintf(out, "\n  ; Increment/Decrement operations (%d expressions)\n", incr_count);
    
    expr = incr_head;
    while (expr != NULL) {
        if (expr->is_increment) {
            fprintf(out, "  ; %s%s\n", expr->is_prefix ? "++" : "", expr->variable);
            fprintf(out, "  mov rax, [%s]\n", expr->variable);
            fprintf(out, "  inc rax\n");
            fprintf(out, "  mov [%s], rax\n", expr->variable);
        } else {
            fprintf(out, "  ; %s%s\n", expr->is_prefix ? "--" : "", expr->variable);
            fprintf(out, "  mov rax, [%s]\n", expr->variable);
            fprintf(out, "  dec rax\n");
            fprintf(out, "  mov [%s], rax\n", expr->variable);
        }
        
        expr = expr->next;
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    printf("🔧 Increment/Decrement Module\n");
    printf("==============================\n");
    printf("Input: %s\n\n", argv[1]);
    
    // Read source file
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open input file\n");
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *src = malloc(sz + 1);
    fread(src, 1, sz, f);
    src[sz] = 0;
    fclose(f);
    
    // Create lexer and parse
    Lexer *lexer = lexer_create(src);
    increment_parse(lexer);
    
    // Count expressions
    int incr_count = 0;
    IncrementExpr *expr = incr_head;
    while (expr != NULL) {
        incr_count++;
        expr = expr->next;
    }
    printf("  ✓ Increment/Decrement expressions found: %d\n\n", incr_count);
    
    // Generate assembly
    FILE *out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot open output file\n");
        lexer_free(lexer);
        free(src);
        return 1;
    }
    
    increment_codegen(out);
    
    fclose(out);
    lexer_free(lexer);
    free(src);
    
    printf("✅ Complete!\n");
    return 0;
}
