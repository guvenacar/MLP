#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>

// Code generator struct for x86-64 assembly
typedef struct {
    FILE* output;           // Output file for assembly code
    int label_counter;      // Counter for unique labels
    int temp_counter;       // Counter for temporary variables
} CodeGen;

// Constructor/destructor
CodeGen* codegen_create(FILE* output);
void codegen_free(CodeGen* gen);

#endif // CODEGEN_H
