#ifndef LOGICAL_CODEGEN_H
#define LOGICAL_CODEGEN_H

#include "logical_parser.h"
#include <stdio.h>

void codegen_logical(FILE* f, LogicalExpr* logical);

#endif
