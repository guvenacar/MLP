#ifndef ARITHMETIC_CODEGEN_H
#define ARITHMETIC_CODEGEN_H

#include "arithmetic_parser.h"
#include <stdio.h>

void codegen_expression(FILE* f, Expression* expr);

#endif
