#include "logical_parser.h"
#include <stdlib.h>

// Logical expression parsing
// Stage 1: Parse logical operations (AND, OR, NOT)

void logical_free(LogicalExpr* logical) {
    if (!logical) return;
    // Note: left and right expressions freed by expression_free
    free(logical);
}

// TODO: Implement logical parsing in Stage 1
LogicalExpr* logical_parse(Parser* parser) {
    // Placeholder: will be implemented with full expression parsing
    return NULL;
}
