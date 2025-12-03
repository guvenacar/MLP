#ifndef STATEMENT_CODEGEN_H
#define STATEMENT_CODEGEN_H

#include "statement.h"
#include <stdio.h>

// Generate code for statement
void statement_generate_code(FILE* output, Statement* stmt);

#endif
