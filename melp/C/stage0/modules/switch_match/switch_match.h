#ifndef SWITCH_MATCH_H
#define SWITCH_MATCH_H

#include <stddef.h>
#include "../../lexer.h"

// Case tipi
typedef enum CaseType {
    CASE_VALUE,         // Değer eşleşmesi (switch)
    CASE_PATTERN,       // Pattern eşleşmesi (match)
    CASE_RANGE,         // Aralık (x..y)
    CASE_CONDITION,     // Koşul (when x > 5)
    CASE_DEFAULT        // Varsayılan (else)
} CaseType;

// Tek bir case
typedef struct Case {
    CaseType type;          // Case tipi
    void* pattern;          // Eşleşme deseni
    void* condition;        // Koşul (opsiyonel)
    void* body;             // Case gövdesi
    struct Case* next;      // Sonraki case
} Case;

// Switch ifadesi (C-style)
typedef struct Switch {
    void* expression;       // Test edilecek ifade
    Case* cases;            // Case listesi
    void* default_case;     // Default case (opsiyonel)
    int fall_through;       // Fall-through davranışı
} Switch;

// Match ifadesi (pattern matching)
typedef struct SwitchMatch {
    void* expression;       // Test edilecek ifade
    Case* patterns;         // Pattern listesi
    int exhaustive;         // Tüm durumlar kapsamlı mı?
} SwitchMatch;

// Aralık tanımı
typedef struct Range {
    long start;             // Başlangıç değeri
    long end;               // Bitiş değeri
    int inclusive;          // Bitiş dahil mi?
} Range;

// API fonksiyonları
Switch* switch_create(void* expression);
SwitchMatch* match_create(void* expression, int exhaustive);
Case* case_create(CaseType type, void* pattern);
Range* range_create(long start, long end, int inclusive);

void switch_add_case(Switch* sw, Case* case_item);
void match_add_pattern(SwitchMatch* match, Case* pattern);

void switch_free(Switch* sw);
void match_free(SwitchMatch* match);
void case_free(Case* case_item);
void range_free(Range* range);

#endif // SWITCH_MATCH_H
