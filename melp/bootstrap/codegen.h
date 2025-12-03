// MELP Bootstrap Compiler - Code Generator Header

#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdarg.h>
#include "parser.h"

void codegen_init(const char *filename);
void codegen(ASTNode *ast);
void codegen_close();

#endif
