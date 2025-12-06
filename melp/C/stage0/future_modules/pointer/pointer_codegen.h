#ifndef POINTER_CODEGEN_H
#define POINTER_CODEGEN_H

#include "pointer.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_pointer_declaration(FILE* out, Pointer* ptr);
void codegen_address_of(FILE* out, AddressOf* addr);
void codegen_dereference(FILE* out, Dereference* deref);
void codegen_pointer_arith(FILE* out, PointerArith* arith);
void codegen_null_check(FILE* out, const char* ptr_name);

#endif // POINTER_CODEGEN_H
