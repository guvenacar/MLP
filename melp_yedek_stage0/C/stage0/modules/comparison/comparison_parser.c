#include "comparison_parser.h"
#include <stdlib.h>

// Comparison expression parsing
// Stage 1: Parse comparison operations

void comparison_free(ComparisonExpr* cmp) {
    if (!cmp) return;
    // Note: left and right expressions freed by expression_free
    free(cmp);
}

// TODO: Implement comparison parsing in Stage 1
ComparisonExpr* comparison_parse(Parser* parser) {
    // Placeholder: will be implemented with full expression parsing
    return NULL;
}
