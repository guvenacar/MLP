#ifndef COMPARISON_H
#define COMPARISON_H

// Comparison operators
typedef enum {
    CMP_EQUAL,        // ==
    CMP_NOT_EQUAL,    // !=
    CMP_LESS,         // <
    CMP_LESS_EQUAL,   // <=
    CMP_GREATER,      // >
    CMP_GREATER_EQUAL // >=
} ComparisonOp;

// Comparison expression structure
typedef struct ComparisonExpr {
    ComparisonOp op;
    void* left;           // Can be ArithmeticExpr or variable
    void* right;          // Can be ArithmeticExpr or variable
    int left_is_literal;
    int right_is_literal;
    char* left_value;
    char* right_value;
    int is_float;
} ComparisonExpr;

// Free comparison expression
void comparison_expr_free(ComparisonExpr* expr);

#endif // COMPARISON_H
