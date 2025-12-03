#ifndef STRING_OPS_CODEGEN_H
#define STRING_OPS_CODEGEN_H

#include "string_ops.h"
#include <stdio.h>

// Code generation for string operations
// Calls C runtime functions from runtime library

// Generate string concatenation (calls mlp_string_concat)
void codegen_string_concat(FILE* output, StringOpExpr* expr);

// Generate string length (calls mlp_string_length)
void codegen_string_length(FILE* output, StringOpExpr* expr);

// Generate substring (calls mlp_string_substr)
void codegen_string_substr(FILE* output, StringOpExpr* expr);

// Generate string split (calls mlp_string_split)
void codegen_string_split(FILE* output, StringOpExpr* expr);

// Generic string operation codegen dispatcher
void codegen_string_operation(FILE* output, StringOpExpr* expr);

#endif
