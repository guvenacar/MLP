#ifndef NULL_SAFETY_CODEGEN_H
#define NULL_SAFETY_CODEGEN_H

#include "null_safety.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_optional(FILE* out, Optional* opt);
void codegen_null_safety_check(FILE* out, NullCheck* check);
void codegen_null_coalesce(FILE* out, NullCoalesce* coal);
void codegen_safe_nav(FILE* out, SafeNav* nav);
void codegen_null_assert(FILE* out, NullAssert* assert);

// Optional operasyonları
void codegen_optional_create(FILE* out, void* value, const char* type);
void codegen_optional_get(FILE* out, Optional* opt);
void codegen_optional_has_value(FILE* out, Optional* opt);
void codegen_optional_get_or(FILE* out, Optional* opt, void* default_val);

// Yardımcı fonksiyonlar
void codegen_null_safety_check_expr(FILE* out, void* expr);
void codegen_panic_on_null(FILE* out, const char* message);

#endif // NULL_SAFETY_CODEGEN_H
