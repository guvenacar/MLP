#ifndef COMPARISON_PARSER_H
#define COMPARISON_PARSER_H

#include "comparison.h"
#include "../../lexer.h"

// Forward declarations
typedef struct Parser Parser;
typedef struct Expression Expression;

typedef struct ComparisonExpr {
    Expression* left;
    Expression* right;
    ComparisonOp op;
} ComparisonExpr;

ComparisonExpr* comparison_parse(Parser* parser);
void comparison_free(ComparisonExpr* cmp);

#endif
