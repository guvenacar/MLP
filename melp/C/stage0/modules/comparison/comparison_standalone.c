#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"

// Comparison Expression Structure
typedef struct ComparisonExpr {
    char result[256];      // variable to store boolean result
    char left[256];        // left operand (variable or literal)
    char op[3];            // comparison operator: ==, !=, <, >, <=, >=
    char right[256];       // right operand
    struct ComparisonExpr *next;
} ComparisonExpr;

static ComparisonExpr *comparison_head = NULL;
static ComparisonExpr *comparison_tail = NULL;

// Parse comparison expressions: boolean result = (a == b)
void comparison_parse(Lexer *lexer) {
    Token *tok = lexer_next_token(lexer);
    
    while (tok->type != TOKEN_EOF) {
        // Look for: boolean result = ( left op right )
        if (tok->type == TOKEN_BOOLEAN) {
            Token *name_tok = lexer_next_token(lexer); // variable name
            if (name_tok->type != TOKEN_IDENTIFIER) {
                token_free(tok);
                tok = name_tok;
                continue;
            }
            
            ComparisonExpr *expr = malloc(sizeof(ComparisonExpr));
            strcpy(expr->result, name_tok->value);
            expr->next = NULL;
            
            Token *assign_tok = lexer_next_token(lexer); // =
            if (assign_tok->type != TOKEN_ASSIGN) {
                free(expr);
                token_free(tok);
                token_free(name_tok);
                tok = assign_tok;
                continue;
            }
            
            Token *lparen_tok = lexer_next_token(lexer); // (
            if (lparen_tok->type != TOKEN_LPAREN) {
                free(expr);
                token_free(tok);
                token_free(name_tok);
                token_free(assign_tok);
                tok = lparen_tok;
                continue;
            }
            
            Token *left_tok = lexer_next_token(lexer); // left operand
            if (left_tok->type == TOKEN_IDENTIFIER || left_tok->type == TOKEN_NUMBER) {
                strcpy(expr->left, left_tok->value);
            } else {
                free(expr);
                token_free(tok);
                token_free(name_tok);
                token_free(assign_tok);
                token_free(lparen_tok);
                tok = left_tok;
                continue;
            }
            
            Token *op_tok = lexer_next_token(lexer); // operator
            if (op_tok->type == TOKEN_EQUAL || op_tok->type == TOKEN_NOT_EQUAL ||
                op_tok->type == TOKEN_LESS || op_tok->type == TOKEN_GREATER ||
                op_tok->type == TOKEN_LESS_EQUAL || op_tok->type == TOKEN_GREATER_EQUAL) {
                strcpy(expr->op, op_tok->value);
            } else {
                free(expr);
                token_free(tok);
                token_free(name_tok);
                token_free(assign_tok);
                token_free(lparen_tok);
                token_free(left_tok);
                tok = op_tok;
                continue;
            }
            
            Token *right_tok = lexer_next_token(lexer); // right operand
            if (right_tok->type == TOKEN_IDENTIFIER || right_tok->type == TOKEN_NUMBER) {
                strcpy(expr->right, right_tok->value);
            } else {
                free(expr);
                token_free(tok);
                token_free(name_tok);
                token_free(assign_tok);
                token_free(lparen_tok);
                token_free(left_tok);
                token_free(op_tok);
                tok = right_tok;
                continue;
            }
            
            Token *rparen_tok = lexer_next_token(lexer); // )
            if (rparen_tok->type != TOKEN_RPAREN) {
                free(expr);
                token_free(tok);
                token_free(name_tok);
                token_free(assign_tok);
                token_free(lparen_tok);
                token_free(left_tok);
                token_free(op_tok);
                token_free(right_tok);
                tok = rparen_tok;
                continue;
            }
            
            // Add to linked list
            if (comparison_head == NULL) {
                comparison_head = expr;
                comparison_tail = expr;
            } else {
                comparison_tail->next = expr;
                comparison_tail = expr;
            }
            
            printf("  ✓ Comparison: %s = (%s %s %s)\n", expr->result, expr->left, expr->op, expr->right);
            
            // Free all tokens
            token_free(tok);
            token_free(name_tok);
            token_free(assign_tok);
            token_free(lparen_tok);
            token_free(left_tok);
            token_free(op_tok);
            token_free(right_tok);
            tok = rparen_tok; // Keep last token for next iteration
        }
        
        Token *next_tok = lexer_next_token(lexer);
        token_free(tok);
        tok = next_tok;
    }
    
    token_free(tok);
}

// Generate assembly for comparison expressions
void comparison_codegen(FILE *out) {
    int cmp_count = 0;
    ComparisonExpr *expr = comparison_head;
    while (expr != NULL) {
        cmp_count++;
        expr = expr->next;
    }
    
    if (cmp_count == 0) {
        return;
    }
    
    fprintf(out, "\n  ; Comparison operations (%d expressions)\n", cmp_count);
    
    expr = comparison_head;
    while (expr != NULL) {
        fprintf(out, "  ; %s = (%s %s %s)\n", expr->result, expr->left, expr->op, expr->right);
        
        // Load left operand into rax
        // Check if left is a number or variable
        if (expr->left[0] >= '0' && expr->left[0] <= '9') {
            fprintf(out, "  mov rax, %s\n", expr->left);
        } else {
            fprintf(out, "  mov rax, [%s]\n", expr->left);
        }
        
        // Load right operand into rbx
        if (expr->right[0] >= '0' && expr->right[0] <= '9') {
            fprintf(out, "  mov rbx, %s\n", expr->right);
        } else {
            fprintf(out, "  mov rbx, [%s]\n", expr->right);
        }
        
        // Compare and set result
        fprintf(out, "  cmp rax, rbx\n");
        
        // Use conditional set instructions
        if (strcmp(expr->op, "==") == 0) {
            fprintf(out, "  sete al       ; set al=1 if equal, 0 otherwise\n");
        } else if (strcmp(expr->op, "!=") == 0) {
            fprintf(out, "  setne al      ; set al=1 if not equal\n");
        } else if (strcmp(expr->op, "<") == 0) {
            fprintf(out, "  setl al       ; set al=1 if less than\n");
        } else if (strcmp(expr->op, ">") == 0) {
            fprintf(out, "  setg al       ; set al=1 if greater than\n");
        } else if (strcmp(expr->op, "<=") == 0) {
            fprintf(out, "  setle al      ; set al=1 if less or equal\n");
        } else if (strcmp(expr->op, ">=") == 0) {
            fprintf(out, "  setge al      ; set al=1 if greater or equal\n");
        }
        
        // Zero-extend al to rax and store result
        fprintf(out, "  movzx rax, al\n");
        fprintf(out, "  mov [%s], rax\n", expr->result);
        
        expr = expr->next;
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    printf("🔧 Comparison Module - #1 P0\n");
    printf("===================\n");
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
    comparison_parse(lexer);
    
    // Count comparisons
    int cmp_count = 0;
    ComparisonExpr *expr = comparison_head;
    while (expr != NULL) {
        cmp_count++;
        expr = expr->next;
    }
    printf("  ✓ Comparison expressions found: %d\n\n", cmp_count);
    
    // Generate assembly
    FILE *out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot open output file\n");
        lexer_free(lexer);
        free(src);
        return 1;
    }
    
    comparison_codegen(out);
    
    fclose(out);
    lexer_free(lexer);
    free(src);
    
    printf("✅ Complete!\n");
    return 0;
}
