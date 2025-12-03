#ifndef COMPARISON_PARSER_H
#define COMPARISON_PARSER_H

#include "comparison.h"
#include "../../parser.h"

// Expression from arithmetic module
typedef struct Expression Expression;

typedef struct ComparisonExpr {
    Expression* left;
    Expression* right;
    ComparisonOp op;
} ComparisonExpr;

ComparisonExpr* comparison_parse(Parser* parser);
void comparison_free(ComparisonExpr* cmp);

#endif
