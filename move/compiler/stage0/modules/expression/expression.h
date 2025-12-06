#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdbool.h>
#include "../codegen_context/tto_types.h"

// Expression types
typedef enum {
    EXPR_NUMBER,
    EXPR_STRING,
    EXPR_BOOLEAN,
    EXPR_VARIABLE,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_CALL,
    EXPR_ARRAY_ACCESS,
    EXPR_STRUCT_ACCESS
} ExpressionType;

// Expression structure (simplified)
typedef struct Expression {
    ExpressionType type;
    union {
        long number_value;
        char* string_value;
        int boolean_value;
        char* var_name;
        struct {
            struct Expression* left;
            struct Expression* right;
            char* operator;
        } binary;
        struct {
            struct Expression* operand;
            char* operator;
        } unary;
        struct {
            char* func_name;
            struct Expression** args;
            int arg_count;
        } call;
    } data;
    
    // ========== Phase 2: TTO Integration ==========
    TTOTypeInfo* tto_info;               // Full TTO analysis result (heap allocated)
    bool tto_analyzed;                   // Has TTO analysis been performed?
    bool needs_overflow_check;           // Runtime overflow detection needed?
} Expression;

// Expression management
Expression* expression_create(ExpressionType type);
void expression_free(Expression* expr);

#endif
