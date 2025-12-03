#ifndef LOGICAL_PARSER_H
#define LOGICAL_PARSER_H

#include "logical.h"
#include "../../parser.h"

// Expression from arithmetic module
typedef struct Expression Expression;

typedef struct LogicalExpr {
    LogicalOp op;
    Expression* left;   // NULL for unary NOT
    Expression* right;  // Operand for NOT, right side for AND/OR
} LogicalExpr;

LogicalExpr* logical_parse(Parser* parser);
void logical_free(LogicalExpr* logical);

#endif
