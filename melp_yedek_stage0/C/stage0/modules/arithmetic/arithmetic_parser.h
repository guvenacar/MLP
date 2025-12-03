#ifndef ARITHMETIC_PARSER_H
#define ARITHMETIC_PARSER_H

#include "arithmetic.h"
#include "../../lexer.h"

// Forward declarations from variable module
typedef struct Parser Parser;
typedef struct Declaration Declaration;

typedef struct Expression {
    enum {
        EXPR_NUMBER,
        EXPR_VARIABLE,
        EXPR_BINARY_OP
    } type;
    
    union {
        long number_value;
        char* var_name;
        struct {
            struct Expression* left;
            struct Expression* right;
            BinaryOp op;
        } binary;
    } data;
} Expression;

Expression* expression_parse(Parser* parser);
void expression_free(Expression* expr);

#endif
