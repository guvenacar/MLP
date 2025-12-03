#ifndef ARRAY_CODEGEN_H
#define ARRAY_CODEGEN_H

#include "array.h"
#include <stdio.h>

// Code generation for arrays, lists, and tuples

// Generate array allocation and initialization
// Stack array: Fixed size, compile-time known
void codegen_array_stack(FILE* output, Array* arr);

// Heap array: Dynamic size, runtime allocation
void codegen_array_heap(FILE* output, Array* arr);

// Generate list allocation (always heap)
void codegen_list_create(FILE* output, List* list);

// Generate tuple allocation (always stack)
void codegen_tuple_create(FILE* output, Tuple* tuple);

// Generate index access
// Array: mov rax, [array_base + index*8]
void codegen_array_index(FILE* output, const char* array_name, int index);

// Generate list index access (with type checking)
void codegen_list_index(FILE* output, const char* list_name, int index);

#endif
