#ifndef FUNCTION_CODEGEN_H
#define FUNCTION_CODEGEN_H

#include "function.h"
#include <stdio.h>

// Code generation for functions
// Generates assembly with proper calling convention (System V AMD64 ABI)

// Generate function prologue (stack frame setup)
void codegen_function_prologue(FILE* output, Function* func);

// Generate function epilogue (stack frame cleanup)
void codegen_function_epilogue(FILE* output, Function* func);

// Generate function body
void codegen_function_body(FILE* output, Function* func);

// Generate function call
void codegen_function_call(FILE* output, FunctionCall* call);

// Generate return statement
void codegen_return_statement(FILE* output, struct Expression* return_expr);

#endif
