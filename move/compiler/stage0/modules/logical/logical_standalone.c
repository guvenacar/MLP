#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"

// Logical Expression Structure
typedef struct LogicalExpr {
    char result[256];      // variable to store boolean result
    char left[256];        // left operand (variable or literal)
    char op[8];            // logical operator: and, or, not
    char right[256];       // right operand (empty for unary NOT)
    int is_unary;          // 1 for NOT, 0 for AND/OR
    struct LogicalExpr *next;
} LogicalExpr;

static LogicalExpr *logical_head = NULL;
static LogicalExpr *logical_tail = NULL;

// Parse logical expressions: boolean result = a and b, boolean x = not y
void logical_parse(Lexer *lexer) {
    Token *tok = lexer_next_token(lexer);
    
    while (tok->type != TOKEN_EOF) {
        // Look for: boolean result = left op right  OR  boolean result = not value
        if (tok->type == TOKEN_BOOLEAN) {
            Token *name_tok = lexer_next_token(lexer);
            if (name_tok->type != TOKEN_IDENTIFIER) {
                token_free(tok);
                tok = name_tok;
                continue;
            }
            
            LogicalExpr *expr = malloc(sizeof(LogicalExpr));
            strcpy(expr->result, name_tok->value);
            expr->next = NULL;
            expr->is_unary = 0;
            expr->right[0] = '\0';
            
            Token *assign_tok = lexer_next_token(lexer);
            if (assign_tok->type != TOKEN_ASSIGN) {
                free(expr);
                token_free(tok);
                token_free(name_tok);
                tok = assign_tok;
                continue;
            }
            
            Token *first_tok = lexer_next_token(lexer);
            
            // Check if it's unary NOT
            if (first_tok->type == TOKEN_NOT) {
                expr->is_unary = 1;
                strcpy(expr->op, "not");
                
                Token *operand_tok = lexer_next_token(lexer);
                if (operand_tok->type == TOKEN_IDENTIFIER || operand_tok->type == TOKEN_NUMBER ||
                    operand_tok->type == TOKEN_TRUE || operand_tok->type == TOKEN_FALSE) {
                    strcpy(expr->left, operand_tok->value);
                } else {
                    free(expr);
                    token_free(tok);
                    token_free(name_tok);
                    token_free(assign_tok);
                    token_free(first_tok);
                    tok = operand_tok;
                    continue;
                }
                
                // Add to linked list
                if (logical_head == NULL) {
                    logical_head = expr;
                    logical_tail = expr;
                } else {
                    logical_tail->next = expr;
                    logical_tail = expr;
                }
                
                printf("  ✓ Logical: %s = not %s\n", expr->result, expr->left);
                
                token_free(tok);
                token_free(name_tok);
                token_free(assign_tok);
                token_free(first_tok);
                tok = operand_tok;
                
            } else if (first_tok->type == TOKEN_IDENTIFIER || first_tok->type == TOKEN_NUMBER ||
                       first_tok->type == TOKEN_TRUE || first_tok->type == TOKEN_FALSE) {
                // Binary AND/OR
                strcpy(expr->left, first_tok->value);
                
                Token *op_tok = lexer_next_token(lexer);
                if (op_tok->type == TOKEN_AND || op_tok->type == TOKEN_OR) {
                    strcpy(expr->op, op_tok->value);
                } else {
                    free(expr);
                    token_free(tok);
                    token_free(name_tok);
                    token_free(assign_tok);
                    token_free(first_tok);
                    tok = op_tok;
                    continue;
                }
                
                Token *right_tok = lexer_next_token(lexer);
                if (right_tok->type == TOKEN_IDENTIFIER || right_tok->type == TOKEN_NUMBER ||
                    right_tok->type == TOKEN_TRUE || right_tok->type == TOKEN_FALSE) {
                    strcpy(expr->right, right_tok->value);
                } else {
                    free(expr);
                    token_free(tok);
                    token_free(name_tok);
                    token_free(assign_tok);
                    token_free(first_tok);
                    token_free(op_tok);
                    tok = right_tok;
                    continue;
                }
                
                // Add to linked list
                if (logical_head == NULL) {
                    logical_head = expr;
                    logical_tail = expr;
                } else {
                    logical_tail->next = expr;
                    logical_tail = expr;
                }
                
                printf("  ✓ Logical: %s = %s %s %s\n", expr->result, expr->left, expr->op, expr->right);
                
                token_free(tok);
                token_free(name_tok);
                token_free(assign_tok);
                token_free(first_tok);
                token_free(op_tok);
                tok = right_tok;
                
            } else {
                free(expr);
                token_free(tok);
                token_free(name_tok);
                token_free(assign_tok);
                tok = first_tok;
                continue;
            }
        }
        
        Token *next_tok = lexer_next_token(lexer);
        token_free(tok);
        tok = next_tok;
    }
    
    token_free(tok);
}

// Generate assembly for logical expressions
void logical_codegen(FILE *out) {
    int logical_count = 0;
    LogicalExpr *expr = logical_head;
    while (expr != NULL) {
        logical_count++;
        expr = expr->next;
    }
    
    if (logical_count == 0) {
        return;
    }
    
    fprintf(out, "\n  ; Logical operations (%d expressions)\n", logical_count);
    
    expr = logical_head;
    while (expr != NULL) {
        if (expr->is_unary) {
            // NOT operation: result = not operand
            fprintf(out, "  ; %s = not %s\n", expr->result, expr->left);
            
            // Load operand
            if (strcmp(expr->left, "true") == 0) {
                fprintf(out, "  mov rax, 1\n");
            } else if (strcmp(expr->left, "false") == 0) {
                fprintf(out, "  mov rax, 0\n");
            } else if (expr->left[0] >= '0' && expr->left[0] <= '9') {
                fprintf(out, "  mov rax, %s\n", expr->left);
            } else {
                fprintf(out, "  mov rax, [%s]\n", expr->left);
            }
            
            // NOT: xor with 1 to flip boolean
            fprintf(out, "  xor rax, 1\n");
            fprintf(out, "  mov [%s], rax\n", expr->result);
            
        } else {
            // AND/OR operation: result = left op right
            fprintf(out, "  ; %s = %s %s %s\n", expr->result, expr->left, expr->op, expr->right);
            
            // Load left operand
            if (strcmp(expr->left, "true") == 0) {
                fprintf(out, "  mov rax, 1\n");
            } else if (strcmp(expr->left, "false") == 0) {
                fprintf(out, "  mov rax, 0\n");
            } else if (expr->left[0] >= '0' && expr->left[0] <= '9') {
                fprintf(out, "  mov rax, %s\n", expr->left);
            } else {
                fprintf(out, "  mov rax, [%s]\n", expr->left);
            }
            
            // Load right operand
            if (strcmp(expr->right, "true") == 0) {
                fprintf(out, "  mov rbx, 1\n");
            } else if (strcmp(expr->right, "false") == 0) {
                fprintf(out, "  mov rbx, 0\n");
            } else if (expr->right[0] >= '0' && expr->right[0] <= '9') {
                fprintf(out, "  mov rbx, %s\n", expr->right);
            } else {
                fprintf(out, "  mov rbx, [%s]\n", expr->right);
            }
            
            // Perform operation
            if (strcmp(expr->op, "&&") == 0 || strcmp(expr->op, "and") == 0) {
                fprintf(out, "  and rax, rbx      ; logical AND\n");
            } else if (strcmp(expr->op, "||") == 0 || strcmp(expr->op, "or") == 0) {
                fprintf(out, "  or rax, rbx       ; logical OR\n");
            }
            
            // Normalize result to 0 or 1
            fprintf(out, "  test rax, rax\n");
            fprintf(out, "  setne al\n");
            fprintf(out, "  movzx rax, al\n");
            fprintf(out, "  mov [%s], rax\n", expr->result);
        }
        
        expr = expr->next;
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    printf("🔧 Logical Module - #1 P0\n");
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
    logical_parse(lexer);
    
    // Count logical expressions
    int logical_count = 0;
    LogicalExpr *expr = logical_head;
    while (expr != NULL) {
        logical_count++;
        expr = expr->next;
    }
    printf("  ✓ Logical expressions found: %d\n\n", logical_count);
    
    // Generate assembly
    FILE *out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot open output file\n");
        lexer_free(lexer);
        free(src);
        return 1;
    }
    
    logical_codegen(out);
    
    fclose(out);
    lexer_free(lexer);
    free(src);
    
    printf("✅ Complete!\n");
    return 0;
}
