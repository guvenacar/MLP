#ifndef PRINT_CODEGEN_H
#define PRINT_CODEGEN_H

#include "print.h"
#include <stdio.h>

// Generate code for print statement
void codegen_print_statement(FILE* f, PrintStatement* stmt);

// Finalize code generation (add exit code)
void codegen_print_finalize(FILE* f);

#endif
