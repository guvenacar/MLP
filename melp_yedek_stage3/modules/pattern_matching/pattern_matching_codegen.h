#ifndef PATTERN_MATCHING_CODEGEN_H
#define PATTERN_MATCHING_CODEGEN_H

#include "pattern_matching.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_match(FILE* out, Match* match);
void codegen_pattern_match_arm(FILE* out, MatchArm* arm, int index);
void codegen_pattern(FILE* out, Pattern* pattern);
void codegen_destructure(FILE* out, Destructure* dest);

// Pattern matching yardımcıları
void codegen_pattern_test(FILE* out, Pattern* pattern);
void codegen_pattern_bind(FILE* out, Pattern* pattern);
void codegen_guard_test(FILE* out, void* guard);

// Eşleştirme stratejileri
void codegen_match_linear(FILE* out, Match* match);
void codegen_match_decision_tree(FILE* out, Match* match);

// Özel pattern tipleri
void codegen_literal_match(FILE* out, Pattern* pattern);
void codegen_struct_match(FILE* out, Pattern* pattern);
void codegen_array_match(FILE* out, Pattern* pattern);
void codegen_range_match(FILE* out, Pattern* pattern);

// Exhaustiveness check
void codegen_exhaustive_check(FILE* out, Match* match);

#endif // PATTERN_MATCHING_CODEGEN_H
