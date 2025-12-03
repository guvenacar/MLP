#ifndef GENERATOR_CODEGEN_H
#define GENERATOR_CODEGEN_H

#include "generator.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_generator(FILE* out, Generator* gen);
void codegen_yield(FILE* out, Yield* yld);
void codegen_gen_call(FILE* out, GenCall* call);
void codegen_gen_state_machine(FILE* out, Generator* gen);
void codegen_gen_context_save(FILE* out, Generator* gen);
void codegen_gen_context_restore(FILE* out, Generator* gen);

// Yardımcı fonksiyonlar
void codegen_gen_prologue(FILE* out, Generator* gen);
void codegen_gen_epilogue(FILE* out, Generator* gen);
void codegen_yield_point(FILE* out, int yield_id);
const char* get_gen_mangled_name(Generator* gen);

#endif // GENERATOR_CODEGEN_H
