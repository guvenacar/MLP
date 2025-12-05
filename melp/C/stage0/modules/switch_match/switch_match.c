#include "switch_match.h"
#include <stdlib.h>
#include <string.h>

// Switch oluştur
Switch* switch_create(void* expression) {
    Switch* sw = malloc(sizeof(Switch));
    sw->expression = expression;
    sw->cases = NULL;
    sw->default_case = NULL;
    sw->fall_through = 0; // Varsayılan: fall-through yok
    return sw;
}

// Match oluştur
SwitchMatch* match_create(void* expression, int exhaustive) {
    SwitchMatch* match = malloc(sizeof(SwitchMatch));
    match->expression = expression;
    match->patterns = NULL;
    match->exhaustive = exhaustive;
    return match;
}

// Case oluştur
Case* case_create(CaseType type, void* pattern) {
    Case* case_item = malloc(sizeof(Case));
    case_item->type = type;
    case_item->pattern = pattern;
    case_item->condition = NULL;
    case_item->body = NULL;
    case_item->next = NULL;
    return case_item;
}

// Aralık oluştur
Range* range_create(long start, long end, int inclusive) {
    Range* range = malloc(sizeof(Range));
    range->start = start;
    range->end = end;
    range->inclusive = inclusive;
    return range;
}

// Switch'e case ekle
void switch_add_case(Switch* sw, Case* case_item) {
    if (!sw || !case_item) return;
    
    if (sw->cases == NULL) {
        sw->cases = case_item;
    } else {
        Case* current = sw->cases;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = case_item;
    }
}

// Match'e pattern ekle
void match_add_pattern(SwitchMatch* match, Case* pattern) {
    if (!match || !pattern) return;
    
    if (match->patterns == NULL) {
        match->patterns = pattern;
    } else {
        Case* current = match->patterns;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = pattern;
    }
}

// Bellek temizleme
void switch_free(Switch* sw) {
    if (!sw) return;
    
    Case* current = sw->cases;
    while (current != NULL) {
        Case* next = current->next;
        case_free(current);
        current = next;
    }
    
    free(sw);
}

void match_free(SwitchMatch* match) {
    if (!match) return;
    
    Case* current = match->patterns;
    while (current != NULL) {
        Case* next = current->next;
        case_free(current);
        current = next;
    }
    
    free(match);
}

void case_free(Case* case_item) {
    if (!case_item) return;
    // pattern, condition, body parser tarafından yönetilir
    free(case_item);
}

void range_free(Range* range) {
    if (!range) return;
    free(range);
}
