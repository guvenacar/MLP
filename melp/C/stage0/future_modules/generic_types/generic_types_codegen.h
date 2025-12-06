#ifndef GENERIC_TYPES_CODEGEN_H
#define GENERIC_TYPES_CODEGEN_H

#include "generic_types.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_generic_type(FILE* out, GenericType* type);
void codegen_type_instantiation(FILE* out, TypeInstantiation* inst);
void codegen_generic_function(FILE* out, GenericFunction* func);
void codegen_monomorphization(FILE* out, MonomorphContext* ctx);

// Type parameter codegen
void codegen_type_parameter(FILE* out, TypeParameter* param);
void codegen_type_constraint(FILE* out, TypeConstraint* constraint);

// Monomorphization
void codegen_specialize_type(FILE* out, GenericType* type, void** type_args, int arg_count);
void codegen_specialize_function(FILE* out, GenericFunction* func, void** type_args, int arg_count);

// Name mangling
void codegen_mangled_name(FILE* out, const char* base_name, void** type_args, int arg_count);

// Type info
void codegen_generic_type_info(FILE* out, void* type);
void codegen_type_size(FILE* out, void* type);

#endif // GENERIC_TYPES_CODEGEN_H
