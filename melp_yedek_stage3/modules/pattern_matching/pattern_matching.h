#ifndef PATTERN_MATCHING_H
#define PATTERN_MATCHING_H

#include <stddef.h>
#include "../../lexer.h"

// Pattern tipi
typedef enum PatternType {
    PAT_LITERAL,        // 42, "text", true
    PAT_VARIABLE,       // x, name (binding)
    PAT_WILDCARD,       // _ (her şey eşleşir)
    PAT_STRUCT,         // Person { name, age }
    PAT_ARRAY,          // [x, y, z] veya [head, ...tail]
    PAT_RANGE,          // 1..10, 'a'..'z'
    PAT_GUARD,          // pattern when condition
    PAT_OR,             // pattern1 | pattern2
    PAT_TYPE            // value is Type
} PatternType;

// Pattern tanımı
typedef struct Pattern {
    PatternType type;
    void* value;            // Literal değer veya sub-pattern
    char* var_name;         // Variable binding adı
    void* guard;            // Guard koşulu (expression)
    struct Pattern** sub_patterns;  // Sub-patternler (struct, array için)
    int sub_count;          // Sub-pattern sayısı
    int is_rest;            // Rest pattern mı? (...tail)
} Pattern;

// Match ifadesi
typedef struct Match {
    void* expr;             // Eşleştirilecek ifade
    struct MatchArm** arms; // Match kolları
    int arm_count;          // Kol sayısı
    int exhaustive;         // Tam kapsamlı mı?
} Match;

// Match kolu (arm)
typedef struct MatchArm {
    Pattern** patterns;     // Patternler (| ile ayrılmış)
    int pattern_count;
    void* guard;            // Guard (when clause)
    void* body;             // Arm gövdesi
} MatchArm;

// Pattern eşleşme sonucu
typedef struct MatchResult {
    int matched;            // Eşleşti mi?
    void** bindings;        // Binding değerleri
    char** binding_names;   // Binding isimleri
    int binding_count;
} MatchResult;

// Destructuring (yapı ayrıştırma)
typedef struct Destructure {
    Pattern* pattern;       // Pattern
    void* value;            // Değer
    int success;            // Başarılı mı?
} Destructure;

// API fonksiyonları
Pattern* pattern_create(PatternType type);
Match* pattern_match_create(void* expr);
MatchArm* match_arm_create(void);
MatchResult* match_result_create(void);
Destructure* destructure_create(Pattern* pattern, void* value);

void pattern_free(Pattern* pat);
void pattern_match_free(Match* match);
void match_arm_free(MatchArm* arm);
void match_result_free(MatchResult* result);
void destructure_free(Destructure* dest);

// Pattern operasyonları
void pattern_add_sub(Pattern* pat, Pattern* sub);
void pattern_set_guard(Pattern* pat, void* guard);
void pattern_set_var_name(Pattern* pat, const char* name);

// Match operasyonları
void match_add_arm(Match* match, MatchArm* arm);
void match_arm_add_pattern(MatchArm* arm, Pattern* pattern);

// Eşleştirme
MatchResult* pattern_match(Pattern* pattern, void* value);
int pattern_matches(Pattern* pattern, void* value);

// Destructuring
int destructure_execute(Destructure* dest);
void* destructure_get_binding(Destructure* dest, const char* name);

#endif // PATTERN_MATCHING_H
