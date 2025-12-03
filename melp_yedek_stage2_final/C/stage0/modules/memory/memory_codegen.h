#ifndef MEMORY_CODEGEN_H
#define MEMORY_CODEGEN_H

#include "memory.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_memory_alloc(FILE* out, MemoryAlloc* alloc);
void codegen_memory_free(FILE* out, MemoryFree* free_op);
void codegen_memory_copy(FILE* out, MemoryCopy* copy);
void codegen_memory_zero(FILE* out, MemoryZero* zero);
void codegen_memory_realloc(FILE* out, MemoryRealloc* realloc_op);

// Yardımcı fonksiyonlar
void codegen_malloc_call(FILE* out, size_t size);
void codegen_free_call(FILE* out, const char* var_name);
void codegen_memset_call(FILE* out, const char* var_name, int value, size_t size);

#endif // MEMORY_CODEGEN_H
