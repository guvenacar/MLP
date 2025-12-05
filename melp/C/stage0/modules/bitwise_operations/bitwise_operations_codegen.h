#ifndef BITWISE_OPERATIONS_CODEGEN_H
#define BITWISE_OPERATIONS_CODEGEN_H

#include "bitwise_operations.h"
#include <stdio.h>

// Generate x86-64 assembly for bitwise operations
void codegen_bitwise_operation(FILE* out, BitwiseExpr* expr);

#endif
