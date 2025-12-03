#include "pattern_matching_codegen.h"
#include <string.h>

// Match için kod üret
void codegen_match(FILE* out, Match* match) {
    fprintf(out, "    ; Match expression\n");
    
    // İfadeyi değerlendir
    fprintf(out, "    ; Evaluate match expr\n");
    // match->expr burada üretilecek
    fprintf(out, "    mov [rbp - 8], rax    ; Match değeri\n");
    
    // Linear search strategy (basit)
    codegen_match_linear(out, match);
    
    // Exhaustive check
    if (match->exhaustive) {
        codegen_exhaustive_check(out, match);
    }
}

// Match arm için kod üret
void codegen_pattern_match_arm(FILE* out, MatchArm* arm, int index) {
    fprintf(out, ".match_arm_%d:\n", index);
    
    // Her pattern'i test et (OR semantiği)
    for (int i = 0; i < arm->pattern_count; i++) {
        codegen_pattern_test(out, arm->patterns[i]);
        fprintf(out, "    test rax, rax\n");
        fprintf(out, "    jnz .arm_%d_guard    ; Pattern eşleşti\n", index);
    }
    
    // Hiçbir pattern eşleşmedi
    fprintf(out, "    jmp .match_arm_%d\n", index + 1);
    
    // Guard kontrolü
    fprintf(out, ".arm_%d_guard:\n", index);
    if (arm->guard) {
        codegen_guard_test(out, arm->guard);
        fprintf(out, "    test rax, rax\n");
        fprintf(out, "    jz .match_arm_%d    ; Guard başarısız\n", index + 1);
    }
    
    // Binding'leri yap
    for (int i = 0; i < arm->pattern_count; i++) {
        codegen_pattern_bind(out, arm->patterns[i]);
    }
    
    // Arm body
    fprintf(out, "    ; Match arm body\n");
    // arm->body burada üretilecek
    
    fprintf(out, "    jmp .match_end\n");
}

// Pattern için kod üret
void codegen_pattern(FILE* out, Pattern* pattern) {
    switch (pattern->type) {
        case PAT_WILDCARD:
            fprintf(out, "    ; Pattern: _\n");
            fprintf(out, "    mov rax, 1    ; Wildcard her zaman eşleşir\n");
            break;
            
        case PAT_LITERAL:
            codegen_literal_match(out, pattern);
            break;
            
        case PAT_VARIABLE:
            fprintf(out, "    ; Pattern: %s\n", pattern->var_name);
            fprintf(out, "    mov rax, 1    ; Variable her zaman eşleşir\n");
            break;
            
        case PAT_STRUCT:
            codegen_struct_match(out, pattern);
            break;
            
        case PAT_ARRAY:
            codegen_array_match(out, pattern);
            break;
            
        case PAT_RANGE:
            codegen_range_match(out, pattern);
            break;
            
        default:
            fprintf(out, "    xor rax, rax    ; Unknown pattern\n");
            break;
    }
}

// Destructure için kod üret
void codegen_destructure(FILE* out, Destructure* dest) {
    fprintf(out, "    ; Destructure\n");
    
    // Değeri yükle
    fprintf(out, "    mov rax, [rbp - 8]    ; Value\n");
    
    // Pattern test
    codegen_pattern_test(out, dest->pattern);
    
    // Binding'leri yap
    codegen_pattern_bind(out, dest->pattern);
}

// Pattern test
void codegen_pattern_test(FILE* out, Pattern* pattern) {
    fprintf(out, "    ; Pattern test\n");
    fprintf(out, "    mov rbx, [rbp - 8]    ; Match değeri\n");
    
    codegen_pattern(out, pattern);
    // Sonuç rax'te (0 veya 1)
}

// Pattern binding
void codegen_pattern_bind(FILE* out, Pattern* pattern) {
    if (pattern->type == PAT_VARIABLE && pattern->var_name) {
        fprintf(out, "    ; Bind: %s\n", pattern->var_name);
        fprintf(out, "    mov rax, [rbp - 8]\n");
        fprintf(out, "    mov [rbp - 16], rax    ; %s\n", pattern->var_name);
    }
    
    // Sub-patterns için recursive binding
    for (int i = 0; i < pattern->sub_count; i++) {
        codegen_pattern_bind(out, pattern->sub_patterns[i]);
    }
}

// Guard test
void codegen_guard_test(FILE* out, void* guard) {
    fprintf(out, "    ; Guard test\n");
    // guard expression burada üretilecek
    fprintf(out, "    ; Result in rax\n");
}

// Linear matching strategy
void codegen_match_linear(FILE* out, Match* match) {
    fprintf(out, "    ; Linear match\n");
    
    for (int i = 0; i < match->arm_count; i++) {
        codegen_pattern_match_arm(out, match->arms[i], i);
    }
    
    fprintf(out, ".match_arm_%d:    ; No match\n", match->arm_count);
    fprintf(out, "    ; Match exhausted\n");
    
    fprintf(out, ".match_end:\n");
}

// Decision tree strategy (optimizasyon)
void codegen_match_decision_tree(FILE* out, Match* match) {
    fprintf(out, "    ; Decision tree match (TODO)\n");
    // Daha verimli eşleştirme için decision tree
}

// Literal pattern eşleştirme
void codegen_literal_match(FILE* out, Pattern* pattern) {
    fprintf(out, "    ; Literal match\n");
    fprintf(out, "    mov rax, [rbp - 8]    ; Match değeri\n");
    fprintf(out, "    cmp rax, %ld    ; Literal\n", (long)pattern->value);
    fprintf(out, "    sete al\n");
    fprintf(out, "    movzx rax, al\n");
}

// Struct pattern eşleştirme
void codegen_struct_match(FILE* out, Pattern* pattern) {
    fprintf(out, "    ; Struct pattern match\n");
    fprintf(out, "    mov rax, [rbp - 8]    ; Struct pointer\n");
    
    // Her field için sub-pattern test
    for (int i = 0; i < pattern->sub_count; i++) {
        fprintf(out, "    mov rbx, [rax + %d]    ; Field %d\n", i * 8, i);
        fprintf(out, "    mov [rbp - 16], rbx\n");
        
        // Sub-pattern test (recursive)
        codegen_pattern_test(out, pattern->sub_patterns[i]);
        
        fprintf(out, "    test rax, rax\n");
        fprintf(out, "    jz .struct_no_match\n");
    }
    
    fprintf(out, "    mov rax, 1    ; Struct matched\n");
    fprintf(out, "    jmp .struct_match_done\n");
    fprintf(out, ".struct_no_match:\n");
    fprintf(out, "    xor rax, rax\n");
    fprintf(out, ".struct_match_done:\n");
}

// Array pattern eşleştirme
void codegen_array_match(FILE* out, Pattern* pattern) {
    fprintf(out, "    ; Array pattern match\n");
    fprintf(out, "    mov rax, [rbp - 8]    ; Array pointer\n");
    fprintf(out, "    mov rcx, [rax]        ; Array length\n");
    
    // Length check
    int expected_len = pattern->is_rest ? pattern->sub_count - 1 : pattern->sub_count;
    
    if (!pattern->is_rest) {
        fprintf(out, "    cmp rcx, %d\n", expected_len);
        fprintf(out, "    jne .array_no_match\n");
    } else {
        fprintf(out, "    cmp rcx, %d\n", expected_len);
        fprintf(out, "    jl .array_no_match    ; Too short\n");
    }
    
    // Element matching
    for (int i = 0; i < pattern->sub_count; i++) {
        if (pattern->sub_patterns[i]->is_rest) {
            fprintf(out, "    ; Rest pattern (...)\n");
            continue;
        }
        
        fprintf(out, "    mov rbx, [rax + %d]    ; Element %d\n", 
                (i + 1) * 8, i);
        fprintf(out, "    mov [rbp - 16], rbx\n");
        
        codegen_pattern_test(out, pattern->sub_patterns[i]);
        fprintf(out, "    test rax, rax\n");
        fprintf(out, "    jz .array_no_match\n");
    }
    
    fprintf(out, "    mov rax, 1    ; Array matched\n");
    fprintf(out, "    jmp .array_match_done\n");
    fprintf(out, ".array_no_match:\n");
    fprintf(out, "    xor rax, rax\n");
    fprintf(out, ".array_match_done:\n");
}

// Range pattern eşleştirme
void codegen_range_match(FILE* out, Pattern* pattern) {
    fprintf(out, "    ; Range pattern match\n");
    fprintf(out, "    mov rax, [rbp - 8]    ; Value\n");
    
    // Lower bound check
    fprintf(out, "    cmp rax, %ld    ; Lower bound\n", (long)pattern->value);
    fprintf(out, "    jl .range_no_match\n");
    
    // Upper bound check (pattern->value + 1 olarak varsayıyoruz)
    fprintf(out, "    cmp rax, %ld    ; Upper bound\n", 
            (long)pattern->value + 10);
    fprintf(out, "    jg .range_no_match\n");
    
    fprintf(out, "    mov rax, 1    ; In range\n");
    fprintf(out, "    jmp .range_match_done\n");
    fprintf(out, ".range_no_match:\n");
    fprintf(out, "    xor rax, rax\n");
    fprintf(out, ".range_match_done:\n");
}

// Exhaustive check
void codegen_exhaustive_check(FILE* out, Match* match) {
    fprintf(out, ".match_exhaustive_fail:\n");
    fprintf(out, "    ; Non-exhaustive match error\n");
    fprintf(out, "    lea rdi, [rel .msg_non_exhaustive]\n");
    fprintf(out, "    call puts\n");
    fprintf(out, "    mov rdi, 1\n");
    fprintf(out, "    call exit\n");
    
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".msg_non_exhaustive: db \"Match is not exhaustive\", 0\n");
    fprintf(out, "section .text\n");
}
