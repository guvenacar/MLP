#ifndef FUNCTIONS_CODEGEN_H
#define FUNCTIONS_CODEGEN_H

#include <stdio.h>
#include "functions.h"

// Generate assembly code for function declaration
void function_generate_declaration(FILE* output, FunctionDeclaration* func);

// Generate assembly code for function call
void function_generate_call(FILE* output, FunctionCall* call);

// Generate assembly code for return statement
void function_generate_return(FILE* output, ReturnStatement* ret);

// Helper: Generate function prologue (stack setup)
void function_generate_prologue(FILE* output, FunctionDeclaration* func);

// Helper: Generate function epilogue (stack cleanup, return)
void function_generate_epilogue(FILE* output, FunctionDeclaration* func);

#endif
