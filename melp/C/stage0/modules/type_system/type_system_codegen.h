#ifndef TYPE_SYSTEM_CODEGEN_H
#define TYPE_SYSTEM_CODEGEN_H

#include "type_system.h"
#include <stdio.h>

// Generate type metadata for runtime type checking
void codegen_type_metadata(FILE* out, Type* type);

// Generate type checking code
void codegen_type_check(FILE* out, Type* expected, Type* actual);

#endif
