#ifndef LOGICAL_CODEGEN_H
#define LOGICAL_CODEGEN_H

#include <stdio.h>
#include "logical.h"

// Generate assembly code for logical expression
// Result will be in rax (0 for false, 1 for true)
void logical_generate_code(FILE* output, LogicalExpr* expr);

#endif // LOGICAL_CODEGEN_H
