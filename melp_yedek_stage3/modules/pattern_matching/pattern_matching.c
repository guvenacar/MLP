#include "pattern_matching.h"
#include <stdlib.h>
#include <string.h>

// Pattern oluştur
Pattern* pattern_create(PatternType type) {
    Pattern* pat = malloc(sizeof(Pattern));
    pat->type = type;
    pat->value = NULL;
    pat->var_name = NULL;
    pat->guard = NULL;
    pat->sub_patterns = NULL;
    pat->sub_count = 0;
    pat->is_rest = 0;
    return pat;
}

// Match oluştur
Match* pattern_match_create(void* expr) {
    Match* match = malloc(sizeof(Match));
    match->expr = expr;
    match->arms = NULL;
    match->arm_count = 0;
    match->exhaustive = 0;
    return match;
}

// Match arm oluştur
MatchArm* match_arm_create(void) {
    MatchArm* arm = malloc(sizeof(MatchArm));
    arm->patterns = NULL;
    arm->pattern_count = 0;
    arm->guard = NULL;
    arm->body = NULL;
    return arm;
}

// Match result oluştur
MatchResult* match_result_create(void) {
    MatchResult* result = malloc(sizeof(MatchResult));
    result->matched = 0;
    result->bindings = NULL;
    result->binding_names = NULL;
    result->binding_count = 0;
    return result;
}

// Destructure oluştur
Destructure* destructure_create(Pattern* pattern, void* value) {
    Destructure* dest = malloc(sizeof(Destructure));
    dest->pattern = pattern;
    dest->value = value;
    dest->success = 0;
    return dest;
}

// Pattern'e sub-pattern ekle
void pattern_add_sub(Pattern* pat, Pattern* sub) {
    if (!pat) return;
    
    pat->sub_patterns = realloc(pat->sub_patterns,
                               sizeof(Pattern*) * (pat->sub_count + 1));
    pat->sub_patterns[pat->sub_count] = sub;
    pat->sub_count++;
}

// Pattern guard ayarla
void pattern_set_guard(Pattern* pat, void* guard) {
    if (!pat) return;
    pat->guard = guard;
}

// Pattern variable name ayarla
void pattern_set_var_name(Pattern* pat, const char* name) {
    if (!pat) return;
    pat->var_name = strdup(name);
}

// Match'e arm ekle
void match_add_arm(Match* match, MatchArm* arm) {
    if (!match) return;
    
    match->arms = realloc(match->arms,
                         sizeof(MatchArm*) * (match->arm_count + 1));
    match->arms[match->arm_count] = arm;
    match->arm_count++;
}

// Arm'a pattern ekle
void match_arm_add_pattern(MatchArm* arm, Pattern* pattern) {
    if (!arm) return;
    
    arm->patterns = realloc(arm->patterns,
                           sizeof(Pattern*) * (arm->pattern_count + 1));
    arm->patterns[arm->pattern_count] = pattern;
    arm->pattern_count++;
}

// Pattern eşleştirme
MatchResult* pattern_match(Pattern* pattern, void* value) {
    MatchResult* result = match_result_create();
    
    if (!pattern || !value) {
        result->matched = 0;
        return result;
    }
    
    switch (pattern->type) {
        case PAT_WILDCARD:
            result->matched = 1;
            break;
            
        case PAT_VARIABLE:
            result->matched = 1;
            // Binding ekle
            result->binding_count = 1;
            result->binding_names = malloc(sizeof(char*));
            result->binding_names[0] = strdup(pattern->var_name);
            result->bindings = malloc(sizeof(void*));
            result->bindings[0] = value;
            break;
            
        case PAT_LITERAL:
            // Basit karşılaştırma (şimdilik)
            result->matched = (pattern->value == value);
            break;
            
        default:
            result->matched = 0;
            break;
    }
    
    // Guard kontrolü
    if (result->matched && pattern->guard) {
        // Guard evaluation burada
        // Şimdilik guard yok sayılıyor
    }
    
    return result;
}

// Pattern eşleşme kontrolü (boolean)
int pattern_matches(Pattern* pattern, void* value) {
    MatchResult* result = pattern_match(pattern, value);
    int matched = result->matched;
    match_result_free(result);
    return matched;
}

// Destructuring yap
int destructure_execute(Destructure* dest) {
    if (!dest || !dest->pattern) return 0;
    
    MatchResult* result = pattern_match(dest->pattern, dest->value);
    dest->success = result->matched;
    
    match_result_free(result);
    return dest->success;
}

// Destructuring'den binding al
void* destructure_get_binding(Destructure* dest, const char* name) {
    if (!dest || !dest->success || !name) return NULL;
    
    // Şimdilik NULL dön
    // Gerçek implementasyonda binding lookup
    return NULL;
}

// Bellek temizleme
void pattern_free(Pattern* pat) {
    if (!pat) return;
    
    if (pat->var_name) free(pat->var_name);
    
    if (pat->sub_patterns) {
        for (int i = 0; i < pat->sub_count; i++) {
            pattern_free(pat->sub_patterns[i]);
        }
        free(pat->sub_patterns);
    }
    
    free(pat);
}

void pattern_match_free(Match* match) {
    if (!match) return;
    
    if (match->arms) {
        for (int i = 0; i < match->arm_count; i++) {
            match_arm_free(match->arms[i]);
        }
        free(match->arms);
    }
    
    free(match);
}

void match_arm_free(MatchArm* arm) {
    if (!arm) return;
    
    if (arm->patterns) {
        for (int i = 0; i < arm->pattern_count; i++) {
            pattern_free(arm->patterns[i]);
        }
        free(arm->patterns);
    }
    
    free(arm);
}

void match_result_free(MatchResult* result) {
    if (!result) return;
    
    if (result->bindings) free(result->bindings);
    
    if (result->binding_names) {
        for (int i = 0; i < result->binding_count; i++) {
            free(result->binding_names[i]);
        }
        free(result->binding_names);
    }
    
    free(result);
}

void destructure_free(Destructure* dest) {
    if (!dest) return;
    free(dest);
}
