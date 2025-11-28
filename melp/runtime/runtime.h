#ifndef RUNTIME_H
#define RUNTIME_H

#include <stddef.h>

// Phase 1: Memory + Print
void* mlp_malloc(size_t size);
void mlp_free(void* ptr);
void print_number(long num);

#endif // RUNTIME_H
