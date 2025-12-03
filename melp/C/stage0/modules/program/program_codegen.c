#include "program_codegen.h"
#include "../../codegen.h"
#include <stdio.h>

// ============================================================================
// MAIN CODE GENERATION ENTRY POINT
// ============================================================================

// Wrapper function that delegates to codegen_generate() from codegen.c
// This provides a modular entry point while using existing code generation
void program_codegen_generate(Program* program, const char* output_file) {
    codegen_generate(program, output_file);
}
