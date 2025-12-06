#include "switch_match_codegen.h"

// Switch için kod üret
void codegen_switch(FILE* out, Switch* sw) {
    fprintf(out, "    ; Switch ifadesi\n");
    
    // İfadeyi hesapla (rax'e)
    fprintf(out, "    ; Expression değerini hesapla\n");
    fprintf(out, "    mov rax, [rbp - 8]    ; Switch değeri\n");
    
    // Jump table mı yoksa linear search mı?
    if (should_use_jump_table(sw)) {
        codegen_jump_table(out, sw);
    } else {
        codegen_linear_search(out, sw);
    }
}

// Match için kod üret (pattern matching)
void codegen_switch_match(FILE* out, SwitchMatch* match) {
    fprintf(out, "    ; Match ifadesi (pattern matching)\n");
    
    // İfadeyi hesapla
    fprintf(out, "    mov rax, [rbp - 8]    ; Match değeri\n");
    
    // Her pattern için kontrol
    Case* pattern = match->patterns;
    int pattern_num = 0;
    
    while (pattern != NULL) {
        fprintf(out, "    ; Pattern %d kontrolü\n", pattern_num);
        
        switch (pattern->type) {
            case CASE_VALUE:
                // Değer eşleşmesi
                fprintf(out, "    cmp rax, %ld    ; Değer kontrolü\n", 
                        (long)pattern->pattern);
                fprintf(out, "    je .match_case_%d\n", pattern_num);
                break;
                
            case CASE_RANGE:
                // Aralık kontrolü
                codegen_range_check(out, (Range*)pattern->pattern);
                fprintf(out, "    je .match_case_%d\n", pattern_num);
                break;
                
            case CASE_CONDITION:
                // Koşul kontrolü
                fprintf(out, "    ; Koşul kontrolü\n");
                fprintf(out, "    test rax, rax\n");
                fprintf(out, "    jnz .match_case_%d\n", pattern_num);
                break;
                
            case CASE_DEFAULT:
                // Varsayılan (her zaman eşleşir)
                fprintf(out, "    jmp .match_case_%d\n", pattern_num);
                break;
                
            default:
                break;
        }
        
        pattern = pattern->next;
        pattern_num++;
    }
    
    // Hiçbir pattern eşleşmezse
    if (match->exhaustive) {
        fprintf(out, "    ; Exhaustive match - hata\n");
        fprintf(out, "    jmp .match_non_exhaustive_error\n");
    } else {
        fprintf(out, "    jmp .match_end\n");
    }
    
    // Case gövdelerini üret
    pattern = match->patterns;
    pattern_num = 0;
    while (pattern != NULL) {
        fprintf(out, ".match_case_%d:\n", pattern_num);
        fprintf(out, "    ; Pattern %d gövdesi\n", pattern_num);
        // Body burada üretilecek
        fprintf(out, "    jmp .match_end\n");
        
        pattern = pattern->next;
        pattern_num++;
    }
    
    fprintf(out, ".match_end:\n");
}

// Case için kod üret
void codegen_case(FILE* out, Case* case_item, int case_num) {
    fprintf(out, ".case_%d:\n", case_num);
    fprintf(out, "    ; Case %d gövdesi\n", case_num);
    
    // Body burada
    
    if (!case_item->next || case_item->type == CASE_DEFAULT) {
        fprintf(out, "    jmp .switch_end\n");
    }
}

// Aralık kontrolü için kod üret
void codegen_range_check(FILE* out, Range* range) {
    fprintf(out, "    ; Aralık kontrolü: %ld..%ld\n", range->start, range->end);
    
    // Alt sınır kontrolü
    fprintf(out, "    cmp rax, %ld\n", range->start);
    fprintf(out, "    jl .range_fail\n");
    
    // Üst sınır kontrolü
    if (range->inclusive) {
        fprintf(out, "    cmp rax, %ld\n", range->end);
        fprintf(out, "    jle .range_success\n");
    } else {
        fprintf(out, "    cmp rax, %ld\n", range->end);
        fprintf(out, "    jl .range_success\n");
    }
    
    fprintf(out, ".range_fail:\n");
    fprintf(out, "    xor rax, rax    ; Eşleşmedi\n");
    fprintf(out, "    jmp .range_done\n");
    fprintf(out, ".range_success:\n");
    fprintf(out, "    mov rax, 1    ; Eşleşti\n");
    fprintf(out, ".range_done:\n");
    fprintf(out, "    test rax, rax\n");
}

// Jump table için kod üret (optimize edilmiş)
void codegen_jump_table(FILE* out, Switch* sw) {
    fprintf(out, "    ; Jump table optimizasyonu\n");
    
    // Jump table adresi hesapla
    fprintf(out, "    lea rbx, [.jump_table]\n");
    fprintf(out, "    mov rcx, rax    ; Switch değeri\n");
    fprintf(out, "    shl rcx, 3    ; x8 (pointer boyutu)\n");
    fprintf(out, "    add rbx, rcx\n");
    fprintf(out, "    jmp [rbx]    ; İndirect jump\n");
    
    // Jump table tanımı
    fprintf(out, "section .data\n");
    fprintf(out, "    .jump_table:\n");
    
    Case* current = sw->cases;
    while (current != NULL) {
        fprintf(out, "        dq .case_%p\n", (void*)current);
        current = current->next;
    }
    
    fprintf(out, "section .text\n");
}

// Linear search için kod üret (genel durum)
void codegen_linear_search(FILE* out, Switch* sw) {
    fprintf(out, "    ; Linear search (genel durum)\n");
    
    Case* current = sw->cases;
    int case_num = 0;
    
    while (current != NULL) {
        if (current->type == CASE_VALUE) {
            fprintf(out, "    cmp rax, %ld\n", (long)current->pattern);
            fprintf(out, "    je .case_%d\n", case_num);
        } else if (current->type == CASE_DEFAULT) {
            fprintf(out, "    jmp .case_%d    ; Default\n", case_num);
        }
        
        current = current->next;
        case_num++;
    }
    
    // Hiçbir case eşleşmezse
    if (sw->default_case) {
        fprintf(out, "    jmp .case_default\n");
    } else {
        fprintf(out, "    jmp .switch_end\n");
    }
    
    // Case gövdeleri
    current = sw->cases;
    case_num = 0;
    while (current != NULL) {
        codegen_case(out, current, case_num);
        current = current->next;
        case_num++;
    }
    
    fprintf(out, ".switch_end:\n");
}

// Jump table kullanılmalı mı?
int should_use_jump_table(Switch* sw) {
    // Case sayısı > 10 ve değerler bitişik ise jump table kullan
    int case_count = 0;
    Case* current = sw->cases;
    
    while (current != NULL) {
        case_count++;
        current = current->next;
    }
    
    return case_count > 10; // Basit heuristic
}
