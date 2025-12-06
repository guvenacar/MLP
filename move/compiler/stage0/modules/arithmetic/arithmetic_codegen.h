#ifndef ARITHMETIC_CODEGEN_H
#define ARITHMETIC_CODEGEN_H

#include <stdio.h>
#include "arithmetic.h"

// Generate assembly code for an arithmetic expression
// Result will be in r8 (for integers) or xmm0 (for floats)
void arithmetic_generate_code(FILE* output, ArithmeticExpr* expr);

// Generate assignment: variable = expression
void arithmetic_generate_assignment(FILE* output, const char* var_name, ArithmeticExpr* expr);

#endif // ARITHMETIC_CODEGEN_H
