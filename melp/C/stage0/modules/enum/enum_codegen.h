#ifndef ENUM_CODEGEN_H
#define ENUM_CODEGEN_H

#include "enum.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_enum_def(FILE* out, EnumDef* def);
void codegen_enum_variant(FILE* out, EnumVariant* variant);
void codegen_enum_value(FILE* out, EnumValue* value);
void codegen_enum_match(FILE* out, EnumMatch* match);
void codegen_enum_cast(FILE* out, EnumCast* cast);

// Enum layout (tagged union)
void codegen_enum_layout(FILE* out, EnumDef* def);
void codegen_enum_tag_table(FILE* out, EnumDef* def);

// Variant operasyonları
void codegen_variant_constructor(FILE* out, EnumDef* def, EnumVariant* variant);
void codegen_variant_destructor(FILE* out, EnumDef* def, EnumVariant* variant);

// Match codegen
void codegen_match_dispatch(FILE* out, EnumMatch* match);
void codegen_enum_match_arm(FILE* out, EnumVariant* variant, void* body);

// Cast codegen
void codegen_safe_cast(FILE* out, EnumCast* cast);
void codegen_unsafe_cast(FILE* out, EnumCast* cast);

// Utility
void codegen_enum_size(FILE* out, EnumDef* def);
void codegen_enum_alignment(FILE* out, EnumDef* def);

#endif // ENUM_CODEGEN_H
