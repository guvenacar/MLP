#ifndef EXPRESSION_CODEGEN_H
#define EXPRESSION_CODEGEN_H

#include "expression.h"
#include <stdio.h>

// Generate code for expression
void expression_generate_code(FILE* output, Expression* expr);

#endif
