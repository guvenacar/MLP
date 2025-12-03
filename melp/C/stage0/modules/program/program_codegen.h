#ifndef PROGRAM_CODEGEN_H
#define PROGRAM_CODEGEN_H

#include "../../parser.h"
#include "../../codegen.h"
#include <stdio.h>

// ============================================================================
// MAIN CODE GENERATION ENTRY POINT (entry from main.c)
// ============================================================================

// Main code generation function - delegates to codegen_generate()
void program_codegen_generate(Program* program, const char* output_file);

#endif
