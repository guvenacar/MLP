#include "enum_codegen.h"
#include <string.h>

// Enum definition için kod üret
void codegen_enum_def(FILE* out, EnumDef* def) {
    fprintf(out, "    ; Enum: %s\n", def->name);
    fprintf(out, "    ; Variants: %d\n", def->variant_count);
    fprintf(out, "    ; Tagged union: %s\n", 
            def->is_tagged_union ? "yes" : "no");
    
    // Enum layout
    codegen_enum_layout(out, def);
    
    // Tag table
    codegen_enum_tag_table(out, def);
}

// Enum variant için kod üret
void codegen_enum_variant(FILE* out, EnumVariant* variant) {
    fprintf(out, "    ; Variant: %s\n", variant->name);
    fprintf(out, "    ; Tag: %d\n", variant->tag);
    fprintf(out, "    ; Has data: %s\n", 
            variant->has_data ? "yes" : "no");
    
    if (variant->has_data) {
        fprintf(out, "    ; Data size: %zu\n", variant->data_size);
    }
}

// Enum value için kod üret
void codegen_enum_value(FILE* out, EnumValue* value) {
    fprintf(out, "    ; Enum value\n");
    fprintf(out, "    ; Current tag: %d\n", value->current_tag);
    
    // Value allocation
    fprintf(out, "    mov rdi, %zu\n", 
            sizeof(int) + value->enum_def->max_data_size);
    fprintf(out, "    call malloc\n");
    
    // Set tag
    fprintf(out, "    mov dword [rax], %d\n", value->current_tag);
    
    // Set data (if any)
    if (value->current_data) {
        fprintf(out, "    ; Copy data\n");
        fprintf(out, "    lea rdi, [rax + 4]\n");
        fprintf(out, "    mov rsi, [rel .enum_data]\n");
        
        EnumVariant* variant = enum_def_get_variant_by_tag(
            value->enum_def, value->current_tag);
        
        if (variant) {
            fprintf(out, "    mov rdx, %zu\n", variant->data_size);
            fprintf(out, "    call memcpy\n");
        }
    }
}

// Enum match için kod üret
void codegen_enum_match(FILE* out, EnumMatch* match) {
    fprintf(out, "    ; Enum match\n");
    
    // Get tag
    fprintf(out, "    mov rax, [rbp - 8]    ; Enum value\n");
    fprintf(out, "    mov edi, [rax]        ; Tag\n");
    
    // Match dispatch
    codegen_match_dispatch(out, match);
}

// Enum cast için kod üret
void codegen_enum_cast(FILE* out, EnumCast* cast) {
    fprintf(out, "    ; Enum cast to: %s\n", cast->target_variant);
    
    if (cast->is_safe) {
        codegen_safe_cast(out, cast);
    } else {
        codegen_unsafe_cast(out, cast);
    }
}

// Enum layout (tagged union)
void codegen_enum_layout(FILE* out, EnumDef* def) {
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".enum_%s_layout:\n", def->name);
    fprintf(out, "    dd %d    ; Tag size (4 bytes)\n", 4);
    fprintf(out, "    dq %zu   ; Max data size\n", def->max_data_size);
    fprintf(out, "    dq %zu   ; Total size\n", 
            4 + def->max_data_size);
    fprintf(out, "section .text\n");
}

// Enum tag table
void codegen_enum_tag_table(FILE* out, EnumDef* def) {
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".enum_%s_tags:\n", def->name);
    
    for (int i = 0; i < def->variant_count; i++) {
        EnumVariant* variant = def->variants[i];
        fprintf(out, "    dd %d    ; %s\n", variant->tag, variant->name);
    }
    
    fprintf(out, ".enum_%s_tag_count: dd %d\n", 
            def->name, def->variant_count);
    fprintf(out, "section .text\n");
}

// Variant constructor
void codegen_variant_constructor(FILE* out, EnumDef* def, EnumVariant* variant) {
    fprintf(out, "\n.enum_%s_%s_new:\n", def->name, variant->name);
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    
    // Allocate
    fprintf(out, "    mov rdi, %zu\n", 4 + def->max_data_size);
    fprintf(out, "    call malloc\n");
    
    // Set tag
    fprintf(out, "    mov dword [rax], %d\n", variant->tag);
    
    // Copy data (if parameter passed)
    if (variant->has_data) {
        fprintf(out, "    ; Copy variant data\n");
        fprintf(out, "    lea rdi, [rax + 4]\n");
        fprintf(out, "    mov rsi, [rbp + 16]   ; Data param\n");
        fprintf(out, "    mov rdx, %zu\n", variant->data_size);
        fprintf(out, "    call memcpy\n");
    }
    
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
}

// Variant destructor
void codegen_variant_destructor(FILE* out, EnumDef* def, EnumVariant* variant) {
    fprintf(out, "\n.enum_%s_%s_free:\n", def->name, variant->name);
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    
    fprintf(out, "    mov rdi, [rbp + 16]   ; Enum value\n");
    fprintf(out, "    call free\n");
    
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
}

// Match dispatch
void codegen_match_dispatch(FILE* out, EnumMatch* match) {
    fprintf(out, "    ; Match dispatch (tag in edi)\n");
    
    for (int i = 0; i < match->arm_count; i++) {
        fprintf(out, "    cmp edi, %d\n", i);
        fprintf(out, "    je .match_arm_%d\n", i);
    }
    
    // Default
    fprintf(out, "    jmp .match_default\n");
    
    // Arms
    for (int i = 0; i < match->arm_count; i++) {
        fprintf(out, "\n.match_arm_%d:\n", i);
        fprintf(out, "    ; Arm body\n");
        fprintf(out, "    jmp .match_end\n");
    }
    
    fprintf(out, "\n.match_default:\n");
    fprintf(out, "    ; Default body\n");
    
    fprintf(out, "\n.match_end:\n");
}

// Match arm
void codegen_enum_match_arm(FILE* out, EnumVariant* variant, void* body) {
    fprintf(out, "    ; Match arm: %s\n", variant->name);
    
    // Extract data (if any)
    if (variant->has_data) {
        fprintf(out, "    mov rax, [rbp - 8]    ; Enum value\n");
        fprintf(out, "    lea rax, [rax + 4]    ; Data pointer\n");
    }
    
    // Arm body burada
    fprintf(out, "    ; Arm body execution\n");
}

// Safe cast
void codegen_safe_cast(FILE* out, EnumCast* cast) {
    fprintf(out, "    ; Safe cast\n");
    
    // Tag check
    fprintf(out, "    mov rax, [rbp - 8]    ; Enum value\n");
    fprintf(out, "    mov edi, [rax]        ; Current tag\n");
    
    // Target variant
    EnumVariant* target = enum_def_find_variant(
        cast->value->enum_def, cast->target_variant);
    
    if (target) {
        fprintf(out, "    cmp edi, %d\n", target->tag);
        fprintf(out, "    jne .cast_error\n");
        
        // Extract data
        fprintf(out, "    lea rax, [rax + 4]    ; Data pointer\n");
        fprintf(out, "    jmp .cast_success\n");
        
        fprintf(out, "\n.cast_error:\n");
        fprintf(out, "    xor rax, rax\n");
        
        fprintf(out, "\n.cast_success:\n");
    }
}

// Unsafe cast
void codegen_unsafe_cast(FILE* out, EnumCast* cast) {
    fprintf(out, "    ; Unsafe cast (no tag check)\n");
    
    fprintf(out, "    mov rax, [rbp - 8]    ; Enum value\n");
    fprintf(out, "    lea rax, [rax + 4]    ; Data pointer\n");
}

// Enum size
void codegen_enum_size(FILE* out, EnumDef* def) {
    fprintf(out, "    mov rax, %zu    ; Enum size\n",
            4 + def->max_data_size);
}

// Enum alignment
void codegen_enum_alignment(FILE* out, EnumDef* def) {
    fprintf(out, "    mov rax, 8    ; Enum alignment\n");
}
