#ifndef VARIABLE_CODEGEN_H
#define VARIABLE_CODEGEN_H

#include "variable.h"
#include <stdio.h>

// Variable code generator structure
typedef struct {
    FILE* output;
    int data_section_active;
    int bss_section_active;
} VariableCodegen;

// Create/free codegen
VariableCodegen* variable_codegen_create(FILE* output);
void variable_codegen_free(VariableCodegen* codegen);

// Generate assembly for declaration
void variable_codegen_declaration(VariableCodegen* codegen, VariableDeclaration* decl);

// Section helpers
void variable_codegen_data_section(VariableCodegen* codegen);
void variable_codegen_bss_section(VariableCodegen* codegen);

#endif
