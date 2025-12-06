#ifndef ARRAY_OPERATIONS_CODEGEN_H
#define ARRAY_OPERATIONS_CODEGEN_H

#include <stdio.h>
#include "array_operations.h"

// Generate assembly code for array operations
void array_op_generate_code(FILE* output, ArrayOperation* op);

// Helper functions for specific operations
void array_push_generate(FILE* output, ArrayOperation* op);
void array_pop_generate(FILE* output, ArrayOperation* op);
void array_slice_generate(FILE* output, ArrayOperation* op);
void array_length_generate(FILE* output, ArrayOperation* op);

#endif
