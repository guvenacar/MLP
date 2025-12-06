#ifndef LOGICAL_H
#define LOGICAL_H

// Logical operators
typedef enum {
    LOG_AND,    // AND / &&
    LOG_OR,     // OR / ||
    LOG_NOT     // NOT / !
} LogicalOp;

// Logical expression structure
typedef struct LogicalExpr {
    LogicalOp op;
    struct LogicalExpr* left;
    struct LogicalExpr* right;
    int is_literal;
    char* value;
} LogicalExpr;

// Free logical expression
void logical_expr_free(LogicalExpr* expr);

#endif // LOGICAL_H
