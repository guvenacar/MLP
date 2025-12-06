#ifndef STRING_OPERATIONS_CODEGEN_H
#define STRING_OPERATIONS_CODEGEN_H

#include <stdio.h>
#include "string_operations.h"

// Generate assembly code for string operations
void string_op_generate_code(FILE* output, StringOperation* op);

// Helper functions for specific operations
void string_concat_generate(FILE* output, StringOperation* op);
void string_length_generate(FILE* output, StringOperation* op);
void string_substring_generate(FILE* output, StringOperation* op);
void string_compare_generate(FILE* output, StringOperation* op);

#endif
