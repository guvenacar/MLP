#ifndef COMPARISON_CODEGEN_H
#define COMPARISON_CODEGEN_H

#include <stdio.h>
#include "comparison.h"

// Generate assembly code for comparison expression
// Result will be in rax (0 for false, 1 for true)
void comparison_generate_code(FILE* output, ComparisonExpr* expr);

// Generate conditional jump based on comparison
// Jumps to label if comparison is true
void comparison_generate_conditional_jump(FILE* output, ComparisonExpr* expr, const char* label);

#endif // COMPARISON_CODEGEN_H
