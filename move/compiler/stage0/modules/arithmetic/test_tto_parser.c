#include "arithmetic_parser.h"
#include "arithmetic.h"
#include "../codegen_context/codegen_context.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper: Print TTO type info
void print_tto_info(ArithmeticExpr* expr, int indent) {
    if (!expr) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    if (expr->is_literal) {
        printf("Literal: %s", expr->value);
    } else if (expr->value) {
        printf("Variable: %s", expr->value);
    } else {
        printf("BinaryOp: ");
        switch (expr->op) {
            case ARITH_ADD: printf("+"); break;
            case ARITH_SUB: printf("-"); break;
            case ARITH_MUL: printf("*"); break;
            case ARITH_DIV: printf("/"); break;
            case ARITH_MOD: printf("%%"); break;
            case ARITH_POW: printf("**"); break;
            default: printf("?"); break;
        }
    }
    
    if (expr->tto_analyzed && expr->tto_info) {
        printf(" → TTO: ");
        switch (expr->tto_info->type) {
            case INTERNAL_TYPE_INT64:
                printf("INT64 (register)");
                if (expr->tto_info->is_constant) {
                    printf(" [const: %ld]", expr->tto_info->const_value.int_value);
                }
                break;
            case INTERNAL_TYPE_DOUBLE:
                printf("DOUBLE (xmm)");
                if (expr->tto_info->is_constant) {
                    printf(" [const: %.2f]", expr->tto_info->const_value.double_value);
                }
                break;
            case INTERNAL_TYPE_BIGDECIMAL:
                printf("BIGDECIMAL (heap)");
                break;
            default:
                printf("UNKNOWN");
        }
        if (expr->needs_overflow_check) {
            printf(" [overflow_check]");
        }
    } else {
        printf(" → TTO: NOT ANALYZED");
    }
    printf("\n");
    
    if (expr->left) {
        print_tto_info(expr->left, indent + 1);
    }
    if (expr->right) {
        print_tto_info(expr->right, indent + 1);
    }
}

int main() {
    printf("=== TTO Parser Integration Test ===\n\n");
    
    // Test 1: Integer literals
    printf("Test 1: 100 + 200\n");
    const char* input1 = "100 + 200";
    Lexer* lexer1 = lexer_create(input1);
    ArithmeticParser* parser1 = arithmetic_parser_create(lexer1);
    ArithmeticExpr* expr1 = arithmetic_parse_expression(parser1);
    print_tto_info(expr1, 0);
    arithmetic_expr_free(expr1);
    arithmetic_parser_free(parser1);
    lexer_free(lexer1);
    printf("\n");
    
    // Test 2: Float literals
    printf("Test 2: 3.14 * 2.0\n");
    const char* input2 = "3.14 * 2.0";
    Lexer* lexer2 = lexer_create(input2);
    ArithmeticParser* parser2 = arithmetic_parser_create(lexer2);
    ArithmeticExpr* expr2 = arithmetic_parse_expression(parser2);
    print_tto_info(expr2, 0);
    arithmetic_expr_free(expr2);
    arithmetic_parser_free(parser2);
    lexer_free(lexer2);
    printf("\n");
    
    // Test 3: Mixed int and float
    printf("Test 3: 10 + 3.5\n");
    const char* input3 = "10 + 3.5";
    Lexer* lexer3 = lexer_create(input3);
    ArithmeticParser* parser3 = arithmetic_parser_create(lexer3);
    ArithmeticExpr* expr3 = arithmetic_parse_expression(parser3);
    print_tto_info(expr3, 0);
    arithmetic_expr_free(expr3);
    arithmetic_parser_free(parser3);
    lexer_free(lexer3);
    printf("\n");
    
    // Test 4: Variable
    printf("Test 4: x + 42\n");
    const char* input4 = "x + 42";
    Lexer* lexer4 = lexer_create(input4);
    ArithmeticParser* parser4 = arithmetic_parser_create(lexer4);
    ArithmeticExpr* expr4 = arithmetic_parse_expression(parser4);
    print_tto_info(expr4, 0);
    arithmetic_expr_free(expr4);
    arithmetic_parser_free(parser4);
    lexer_free(lexer4);
    printf("\n");
    
    // Test 5: Complex expression
    printf("Test 5: (100 + 200) * 3\n");
    const char* input5 = "(100 + 200) * 3";
    Lexer* lexer5 = lexer_create(input5);
    ArithmeticParser* parser5 = arithmetic_parser_create(lexer5);
    ArithmeticExpr* expr5 = arithmetic_parse_expression(parser5);
    print_tto_info(expr5, 0);
    arithmetic_expr_free(expr5);
    arithmetic_parser_free(parser5);
    lexer_free(lexer5);
    printf("\n");
    
    printf("=== All TTO Parser Tests Complete ===\n");
    return 0;
}
