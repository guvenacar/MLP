#ifndef STRUCT_CODEGEN_H
#define STRUCT_CODEGEN_H

#include "struct.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_struct_definition(FILE* out, StructDef* def);
void codegen_struct_allocation(FILE* out, StructInstance* inst);
void codegen_member_access(FILE* out, MemberAccess* access);
void codegen_member_assignment(FILE* out, MemberAccess* access, const char* value);

#endif // STRUCT_CODEGEN_H
