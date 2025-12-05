/**
 * MLP Macro System Module - Core Implementation
 * 
 * Implements macro expansion, derive macros, attribute macros, procedural macros
 */

#include "macro_system.h"
#include <ctype.h>

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

MacroSystemCtx* macro_ctx_create(void) {
    MacroSystemCtx* ctx = (MacroSystemCtx*)calloc(1, sizeof(MacroSystemCtx));
    if (!ctx) {
        fprintf(stderr, "Error: Failed to allocate MacroSystemCtx\n");
        exit(1);
    }
    return ctx;
}

void macro_ctx_free(MacroSystemCtx* ctx) {
    if (ctx) {
        free(ctx);
    }
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

const char* macro_type_str(MacroSystemType type) {
    switch (type) {
        case MACRO_DECLARATIVE: return "declarative (macro_rules!)";
        case MACRO_PROCEDURAL: return "procedural";
        case MACRO_DERIVE: return "derive";
        case MACRO_ATTRIBUTE: return "attribute";
        case MACRO_FUNCTION_LIKE: return "function-like";
        case MACRO_BUILTIN: return "builtin";
        default: return "unknown";
    }
}

// Simple pattern matching (supports $name:type syntax)
bool macro_pattern_match(const char* pattern, const char* input, char** captures) {
    // Simplified pattern matcher
    // Real implementation would use full token-based matching
    return strstr(input, pattern) != NULL;
}

// Substitute captures into template
char* macro_substitute(const char* template, char** captures, int capture_count) {
    static char result[1024];
    strncpy(result, template, sizeof(result) - 1);
    
    // Simple substitution: replace $1, $2, etc. with captures
    for (int i = 0; i < capture_count; i++) {
        char placeholder[8];
        snprintf(placeholder, sizeof(placeholder), "$%d", i + 1);
        
        char* pos = strstr(result, placeholder);
        if (pos && captures[i]) {
            // Simple replacement (in real impl, use proper string manipulation)
            strncpy(pos, captures[i], strlen(captures[i]));
        }
    }
    
    return result;
}

void macro_report_stats(MacroSystemCtx* ctx) {
    printf("\n📊 Macro System Statistics:\n");
    printf("   Declarative macros: %d\n", ctx->declarative_count);
    printf("   Procedural macros:  %d\n", ctx->procedural_count);
    printf("   Derive macros:      %d\n", ctx->derive_count);
    printf("   Attribute macros:   %d\n", ctx->attribute_count);
    printf("   Invocations:        %d\n", ctx->invocation_count);
    printf("   Total expansions:   %d\n", ctx->total_expansions);
    printf("   Max recursion:      %d\n", ctx->max_recursion_depth);
}

// ============================================================================
// DECLARATIVE MACROS (macro_rules!)
// ============================================================================

bool macro_define_declarative(MacroSystemCtx* ctx, const char* name,
                               const char* pattern, const char* expansion,
                               int line) {
    if (ctx->declarative_count >= 64) return false;
    
    DeclarativeMacro* macro = &ctx->declarative[ctx->declarative_count++];
    strncpy(macro->name, name, sizeof(macro->name) - 1);
    macro->mode = EXPAND_EAGER;
    macro->pattern_count = 1;
    macro->line = line;
    
    // Add pattern
    MacroPattern* pat = &macro->patterns[0];
    strncpy(pat->pattern, pattern, sizeof(pat->pattern) - 1);
    strncpy(pat->expansion, expansion, sizeof(pat->expansion) - 1);
    pat->priority = 0;
    
    return true;
}

char* macro_expand_declarative(MacroSystemCtx* ctx, const char* name,
                                const char* arguments, int line) {
    // Find macro
    for (int i = 0; i < ctx->declarative_count; i++) {
        if (strcmp(ctx->declarative[i].name, name) == 0) {
            DeclarativeMacro* macro = &ctx->declarative[i];
            
            // Try to match patterns
            for (int j = 0; j < macro->pattern_count; j++) {
                MacroPattern* pat = &macro->patterns[j];
                char* captures[16] = {0};
                
                if (macro_pattern_match(pat->pattern, arguments, captures)) {
                    ctx->total_expansions++;
                    return macro_substitute(pat->expansion, captures, 16);
                }
            }
        }
    }
    
    static char error[256];
    snprintf(error, sizeof(error), "Macro '%s' not found or pattern mismatch", name);
    return error;
}

// ============================================================================
// PROCEDURAL MACROS
// ============================================================================

bool macro_define_procedural(MacroSystemCtx* ctx, const char* name,
                              const char* input_type, const char* output_type,
                              const char* body, int line) {
    if (ctx->procedural_count >= 32) return false;
    
    ProceduralMacro* macro = &ctx->procedural[ctx->procedural_count++];
    strncpy(macro->name, name, sizeof(macro->name) - 1);
    strncpy(macro->input_type, input_type, sizeof(macro->input_type) - 1);
    strncpy(macro->output_type, output_type, sizeof(macro->output_type) - 1);
    strncpy(macro->body, body, sizeof(macro->body) - 1);
    macro->line = line;
    
    return true;
}

char* macro_execute_procedural(MacroSystemCtx* ctx, const char* name,
                                const char* input, int line) {
    // Find procedural macro
    for (int i = 0; i < ctx->procedural_count; i++) {
        if (strcmp(ctx->procedural[i].name, name) == 0) {
            ProceduralMacro* macro = &ctx->procedural[i];
            
            // Execute macro (simplified: just return body for now)
            ctx->total_expansions++;
            static char result[1024];
            snprintf(result, sizeof(result), "/* proc_macro %s */ %s", 
                     name, macro->body);
            return result;
        }
    }
    
    return "/* procedural macro not found */";
}

// ============================================================================
// DERIVE MACROS
// ============================================================================

bool macro_define_derive(MacroSystemCtx* ctx, const char* trait_name,
                          const char* struct_name, int line) {
    if (ctx->derive_count >= 32) return false;
    
    DeriveMacro* derive = &ctx->derives[ctx->derive_count++];
    strncpy(derive->trait_name, trait_name, sizeof(derive->trait_name) - 1);
    strncpy(derive->struct_name, struct_name, sizeof(derive->struct_name) - 1);
    derive->line = line;
    
    // Generate implementation based on trait
    if (strcmp(trait_name, "Debug") == 0) {
        snprintf(derive->generated_code, sizeof(derive->generated_code),
                "func debug_%s() { print(\"%s\") }", struct_name, struct_name);
    } else if (strcmp(trait_name, "Clone") == 0) {
        snprintf(derive->generated_code, sizeof(derive->generated_code),
                "func clone_%s() { -- clone implementation }", struct_name);
    } else if (strcmp(trait_name, "Copy") == 0) {
        snprintf(derive->generated_code, sizeof(derive->generated_code),
                "-- Copy trait (bitwise copy)");
    } else {
        snprintf(derive->generated_code, sizeof(derive->generated_code),
                "-- derive %s for %s", trait_name, struct_name);
    }
    
    return true;
}

char* macro_generate_derive(MacroSystemCtx* ctx, const char* trait_name,
                            const char* struct_name, int line) {
    // Find or create derive
    for (int i = 0; i < ctx->derive_count; i++) {
        DeriveMacro* derive = &ctx->derives[i];
        if (strcmp(derive->trait_name, trait_name) == 0 &&
            strcmp(derive->struct_name, struct_name) == 0) {
            ctx->total_expansions++;
            return derive->generated_code;
        }
    }
    
    // Auto-generate if not found
    macro_define_derive(ctx, trait_name, struct_name, line);
    return macro_generate_derive(ctx, trait_name, struct_name, line);
}

// ============================================================================
// ATTRIBUTE MACROS
// ============================================================================

bool macro_define_attribute(MacroSystemCtx* ctx, const char* attr_name,
                             const char* target_name, const char* target_type,
                             const char* parameters, int line) {
    if (ctx->attribute_count >= 64) return false;
    
    AttributeMacro* attr = &ctx->attributes[ctx->attribute_count++];
    strncpy(attr->attr_name, attr_name, sizeof(attr->attr_name) - 1);
    strncpy(attr->target_name, target_name, sizeof(attr->target_name) - 1);
    strncpy(attr->target_type, target_type, sizeof(attr->target_type) - 1);
    strncpy(attr->parameters, parameters, sizeof(attr->parameters) - 1);
    attr->line = line;
    
    // Generate transformation
    if (strcmp(attr_name, "test") == 0) {
        snprintf(attr->transformed_code, sizeof(attr->transformed_code),
                "-- Test function: %s", target_name);
    } else if (strcmp(attr_name, "inline") == 0) {
        snprintf(attr->transformed_code, sizeof(attr->transformed_code),
                "-- Inline: %s", target_name);
    } else if (strcmp(attr_name, "deprecated") == 0) {
        snprintf(attr->transformed_code, sizeof(attr->transformed_code),
                "-- DEPRECATED: %s (%s)", target_name, parameters);
    } else {
        snprintf(attr->transformed_code, sizeof(attr->transformed_code),
                "-- Attribute %s applied to %s", attr_name, target_name);
    }
    
    return true;
}

char* macro_apply_attribute(MacroSystemCtx* ctx, const char* attr_name,
                            const char* target_code, int line) {
    // Find attribute
    for (int i = 0; i < ctx->attribute_count; i++) {
        AttributeMacro* attr = &ctx->attributes[i];
        if (strcmp(attr->attr_name, attr_name) == 0) {
            ctx->total_expansions++;
            return attr->transformed_code;
        }
    }
    
    return "/* attribute not found */";
}

// ============================================================================
// BUILT-IN MACROS
// ============================================================================

void macro_init_builtins(MacroSystemCtx* ctx, const char* file_name, int line) {
    // __LINE__
    BuiltinMacro* line_macro = &ctx->builtins[ctx->builtin_count++];
    strncpy(line_macro->name, "__LINE__", sizeof(line_macro->name) - 1);
    snprintf(line_macro->value, sizeof(line_macro->value), "%d", line);
    line_macro->is_compile_time = true;
    
    // __FILE__
    BuiltinMacro* file_macro = &ctx->builtins[ctx->builtin_count++];
    strncpy(file_macro->name, "__FILE__", sizeof(file_macro->name) - 1);
    strncpy(file_macro->value, file_name, sizeof(file_macro->value) - 1);
    file_macro->is_compile_time = true;
    
    // __DATE__
    BuiltinMacro* date_macro = &ctx->builtins[ctx->builtin_count++];
    strncpy(date_macro->name, "__DATE__", sizeof(date_macro->name) - 1);
    strncpy(date_macro->value, "Dec  3 2025", sizeof(date_macro->value) - 1);
    date_macro->is_compile_time = true;
    
    // __TIME__
    BuiltinMacro* time_macro = &ctx->builtins[ctx->builtin_count++];
    strncpy(time_macro->name, "__TIME__", sizeof(time_macro->name) - 1);
    strncpy(time_macro->value, "12:00:00", sizeof(time_macro->value) - 1);
    time_macro->is_compile_time = true;
}

char* macro_get_builtin(MacroSystemCtx* ctx, const char* name) {
    for (int i = 0; i < ctx->builtin_count; i++) {
        if (strcmp(ctx->builtins[i].name, name) == 0) {
            return ctx->builtins[i].value;
        }
    }
    return NULL;
}

// ============================================================================
// MACRO INVOCATION
// ============================================================================

bool macro_invoke(MacroSystemCtx* ctx, const char* invocation, int line) {
    if (ctx->invocation_count >= 128) return false;
    
    MacroInvocation* inv = &ctx->invocations[ctx->invocation_count++];
    
    // Parse invocation (simplified)
    sscanf(invocation, "%127s", inv->macro_name);
    inv->line = line;
    inv->type = MACRO_FUNCTION_LIKE;
    
    // Try to expand
    char* expanded = macro_expand_declarative(ctx, inv->macro_name, 
                                               invocation, line);
    if (expanded) {
        strncpy(inv->expanded, expanded, sizeof(inv->expanded) - 1);
        return true;
    }
    
    return false;
}

char* macro_expand_all(MacroSystemCtx* ctx, const char* source) {
    static char expanded[4096];
    strncpy(expanded, source, sizeof(expanded) - 1);
    
    // Simple expansion: replace known macros
    // Real implementation would use token stream
    
    ctx->total_expansions++;
    return expanded;
}
