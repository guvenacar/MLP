#ifndef NULL_SAFETY_H
#define NULL_SAFETY_H

#include <stddef.h>
#include "../../lexer.h"

// Optional tip durumu
typedef enum OptionalState {
    OPT_NONE,           // Değer yok (null)
    OPT_SOME            // Değer var
} OptionalState;

// Optional tip
typedef struct Optional {
    OptionalState state;    // Durum
    void* value;            // Değer (varsa)
    char* type_name;        // Tip adı
    size_t value_size;      // Değer boyutu
} Optional;

// Null kontrol tipi
typedef enum NullCheckType {
    CHECK_IS_NULL,      // value == null
    CHECK_IS_NOT_NULL,  // value != null
    CHECK_HAS_VALUE     // optional.has_value()
} NullCheckType;

// Null kontrol
typedef struct NullCheck {
    void* expr;             // Kontrol edilecek ifade
    NullCheckType type;     // Kontrol tipi
    int result;             // Sonuç (0 veya 1)
} NullCheck;

// Null-coalescing operatör (a ?? b)
typedef struct NullCoalesce {
    void* left;         // Sol operand
    void* right;        // Sağ operand (fallback)
    void* result;       // Sonuç
} NullCoalesce;

// Safe navigation operatör (a?.b)
typedef struct SafeNav {
    void* object;       // Nesne
    char* member;       // Üye ismi
    int chain;          // Zincirleme mi? (a?.b?.c)
} SafeNav;

// Null assertion (value!)
typedef struct NullAssert {
    void* expr;         // İfade
    char* message;      // Hata mesajı
} NullAssert;

// API fonksiyonları
Optional* optional_create(void* value, const char* type_name, size_t value_size);
Optional* optional_none(const char* type_name);
NullCheck* null_check_create(void* expr, NullCheckType type);
NullCoalesce* null_coalesce_create(void* left, void* right);
SafeNav* safe_nav_create(void* object, const char* member);
NullAssert* null_assert_create(void* expr, const char* message);

void optional_free(Optional* opt);
void null_check_free(NullCheck* check);
void null_coalesce_free(NullCoalesce* coal);
void safe_nav_free(SafeNav* nav);
void null_assert_free(NullAssert* assert);

// Optional operasyonları
int optional_has_value(Optional* opt);
void* optional_get(Optional* opt);
void* optional_get_or(Optional* opt, void* default_value);
Optional* optional_map(Optional* opt, void* (*transform)(void*));
Optional* optional_filter(Optional* opt, int (*predicate)(void*));

#endif // NULL_SAFETY_H
