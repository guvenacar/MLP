/**
 * MLP Macro System Module - Code Generator
 * 
 * Generates assembly with macro expansions
 */

#include "macro_system.h"

// ============================================================================
// CODEGEN HELPERS
// ============================================================================

static void emit_header(FILE* out) {
    fprintf(out, "section .data\n");
    fprintf(out, "    macro_msg db \"Macro expanded\", 10, 0\n");
    fprintf(out, "\n");
}

static void emit_bss(FILE* out) {
    fprintf(out, "section .bss\n");
    fprintf(out, "    ; Macro expansion storage\n");
    fprintf(out, "\n");
}

static void emit_text_header(FILE* out) {
    fprintf(out, "section .text\n");
    fprintf(out, "    global _start\n\n");
}

// ============================================================================
// CODEGEN OPERATIONS
// ============================================================================

void macro_codegen_expansion(FILE* out, MacroInvocation* invoc) {
    fprintf(out, "    ; Macro invocation: %s\n", invoc->macro_name);
    fprintf(out, "    ; Type: %s\n", macro_type_str(invoc->type));
    fprintf(out, "    ; Expanded: %s\n", invoc->expanded);
    fprintf(out, "\n");
}

void macro_codegen_derive(FILE* out, DeriveMacro* derive) {
    fprintf(out, "    ; Derive macro: %s for %s\n", 
            derive->trait_name, derive->struct_name);
    fprintf(out, "    ; Generated: %s\n", derive->generated_code);
    fprintf(out, "\n");
}

void macro_codegen_attribute(FILE* out, AttributeMacro* attr) {
    fprintf(out, "    ; Attribute: #[%s]\n", attr->attr_name);
    fprintf(out, "    ; Target: %s (%s)\n", attr->target_name, attr->target_type);
    fprintf(out, "    ; Transform: %s\n", attr->transformed_code);
    fprintf(out, "\n");
}

// ============================================================================
// MAIN CODEGEN
// ============================================================================

void macro_generate_asm(const char* output_file, MacroSystemCtx* ctx) {
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot open %s for writing\n", output_file);
        return;
    }
    
    fprintf(out, "; MLP Macro System - Generated Assembly\n");
    fprintf(out, "; Module #56: Procedural, Derive, Attribute Macros\n\n");
    
    emit_header(out);
    emit_bss(out);
    emit_text_header(out);
    
    fprintf(out, "_start:\n");
    fprintf(out, "    ; Macro system initialized\n\n");
    
    // Generate declarative macros
    if (ctx->declarative_count > 0) {
        fprintf(out, "    ; Declarative macros (macro_rules!):\n");
        for (int i = 0; i < ctx->declarative_count; i++) {
            fprintf(out, "    ; %s: %d patterns\n", 
                    ctx->declarative[i].name, 
                    ctx->declarative[i].pattern_count);
        }
        fprintf(out, "\n");
    }
    
    // Generate procedural macros
    if (ctx->procedural_count > 0) {
        fprintf(out, "    ; Procedural macros:\n");
        for (int i = 0; i < ctx->procedural_count; i++) {
            fprintf(out, "    ; %s: %s -> %s\n",
                    ctx->procedural[i].name,
                    ctx->procedural[i].input_type,
                    ctx->procedural[i].output_type);
        }
        fprintf(out, "\n");
    }
    
    // Generate derive macros
    for (int i = 0; i < ctx->derive_count; i++) {
        macro_codegen_derive(out, &ctx->derives[i]);
    }
    
    // Generate attribute macros
    for (int i = 0; i < ctx->attribute_count; i++) {
        macro_codegen_attribute(out, &ctx->attributes[i]);
    }
    
    // Generate invocations
    for (int i = 0; i < ctx->invocation_count; i++) {
        macro_codegen_expansion(out, &ctx->invocations[i]);
    }
    
    // Built-in macros
    if (ctx->builtin_count > 0) {
        fprintf(out, "    ; Built-in macros:\n");
        for (int i = 0; i < ctx->builtin_count; i++) {
            fprintf(out, "    ; %s = %s\n",
                    ctx->builtins[i].name,
                    ctx->builtins[i].value);
        }
        fprintf(out, "\n");
    }
    
    // Exit
    fprintf(out, "    ; Exit program\n");
    fprintf(out, "    mov rax, 60\n");
    fprintf(out, "    xor rdi, rdi\n");
    fprintf(out, "    syscall\n");
    
    fclose(out);
}
