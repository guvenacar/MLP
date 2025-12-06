#ifndef ENUM_H
#define ENUM_H

#include <stddef.h>
#include "../../lexer.h"

// Enum variant (tek bir seçenek)
typedef struct EnumVariant {
    char* name;                 // Variant adı (Red, Green, Blue)
    int tag;                    // Discriminant/tag değeri
    void* associated_data;      // Associated data (Rust-style)
    size_t data_size;           // Associated data boyutu
    int has_data;               // Data var mı?
} EnumVariant;

// Enum definition
typedef struct EnumDef {
    char* name;                 // Enum adı
    EnumVariant** variants;     // Variant listesi
    int variant_count;          // Variant sayısı
    int is_tagged_union;        // Tagged union mı? (Rust-style)
    size_t max_data_size;       // En büyük variant data boyutu
} EnumDef;

// Enum value (runtime instance)
typedef struct EnumValue {
    EnumDef* enum_def;          // Enum definition
    int current_tag;            // Şu anki tag
    void* current_data;         // Şu anki data
} EnumValue;

// Enum match (pattern matching için)
typedef struct EnumMatch {
    EnumValue* value;           // Match edilecek value
    void** arm_bodies;          // Her variant için body
    int arm_count;              // Arm sayısı
    void* default_body;         // Default case
} EnumMatch;

// Enum cast (variant dönüşüm)
typedef struct EnumCast {
    EnumValue* value;           // Cast edilecek value
    char* target_variant;       // Hedef variant
    int is_safe;                // Safe cast mı?
} EnumCast;

// API fonksiyonları
EnumVariant* enum_variant_create(const char* name, int tag);
EnumDef* enum_def_create(const char* name);
EnumValue* enum_value_create(EnumDef* enum_def, int tag);
EnumMatch* enum_match_create(EnumValue* value);
EnumCast* enum_cast_create(EnumValue* value, const char* target);

void enum_variant_free(EnumVariant* variant);
void enum_def_free(EnumDef* def);
void enum_value_free(EnumValue* value);
void enum_match_free(EnumMatch* match);
void enum_cast_free(EnumCast* cast);

// Enum variant operasyonları
void enum_variant_set_data(EnumVariant* variant, void* data, size_t size);
int enum_variant_has_data(EnumVariant* variant);

// Enum definition operasyonları
void enum_def_add_variant(EnumDef* def, EnumVariant* variant);
EnumVariant* enum_def_find_variant(EnumDef* def, const char* name);
EnumVariant* enum_def_get_variant_by_tag(EnumDef* def, int tag);

// Enum value operasyonları
void enum_value_set_variant(EnumValue* value, int tag, void* data);
int enum_value_get_tag(EnumValue* value);
void* enum_value_get_data(EnumValue* value);
int enum_value_is_variant(EnumValue* value, const char* variant_name);

// Enum match operasyonları
void enum_match_add_arm(EnumMatch* match, int tag, void* body);
void* enum_match_execute(EnumMatch* match);

// Enum cast operasyonları
void* enum_cast_execute(EnumCast* cast);
int enum_cast_is_valid(EnumCast* cast);

// Utility
const char* enum_variant_name(EnumValue* value);
int enum_variant_count(EnumDef* def);

#endif // ENUM_H
