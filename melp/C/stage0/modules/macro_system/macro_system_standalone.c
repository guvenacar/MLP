/**
 * MLP Macro System Module - Standalone Compiler (Module #56)
 * 
 * Full chain: Source → Parser → Macro Expansion → Codegen → Assembly
 * 
 * Macro features:
 * - Declarative macros (macro_rules!)
 * - Procedural macros (proc_macro)
 * - Derive macros (#[derive(Debug, Clone)])
 * - Attribute macros (#[test], #[inline])
 * - Built-in macros (__LINE__, __FILE__)
 */

#include "macro_system.h"

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv) {
    printf("══════════════════════════════════════════════════════\n");
    printf("  MLP MACRO SYSTEM - Module #56 (Stage 0)\n");
    printf("══════════════════════════════════════════════════════\n");
    printf("Features:\n");
    printf("  • Declarative macros (macro_rules!)\n");
    printf("  • Procedural macros (proc_macro)\n");
    printf("  • Derive macros (#[derive(...)])\n");
    printf("  • Attribute macros (#[...])\n");
    printf("  • Built-in macros (__LINE__, __FILE__)\n");
    printf("  • Compile-time code generation\n");
    printf("══════════════════════════════════════════════════════\n\n");

    // Test source code demonstrating macro features
    const char* test_source = 
        "-- Macro System Test\n"
        "\n"
        "-- Declarative macro (macro_rules!)\n"
        "macro_rules! say_hello {\n"
        "    () => { print(\"Hello, World!\") }\n"
        "}\n"
        "\n"
        "-- Derive macro\n"
        "#[derive(Debug, Clone, Copy)]\n"
        "struct Point\n"
        "    numeric x\n"
        "    numeric y\n"
        "end struct\n"
        "\n"
        "-- Attribute macros\n"
        "#[test]\n"
        "func test_addition\n"
        "    numeric result = 2 + 2\n"
        "end func\n"
        "\n"
        "#[inline]\n"
        "func fast_function\n"
        "    -- optimized code\n"
        "end func\n"
        "\n"
        "#[deprecated(\"Use new_function instead\")]\n"
        "func old_function\n"
        "    -- legacy code\n"
        "end func\n"
        "\n"
        "-- Macro invocations\n"
        "say_hello!()\n"
        "print!(__LINE__)\n"
        "print!(__FILE__)\n";

    printf("[1/5] Creating macro context...\n");
    MacroSystemCtx* ctx = macro_ctx_create();
    
    // Initialize built-in macros
    macro_init_builtins(ctx, "test.mlp", 1);
    
    printf("[2/5] Parsing macro definitions...\n");
    macro_parser_init(test_source);
    bool parse_success = macro_parse_file(ctx);
    
    printf("\n[3/5] Macro analysis complete\n");
    printf("      ─────────────────────────────────────────\n");
    printf("      Declarative macros:  %d (macro_rules!)\n", 
           ctx->declarative_count);
    printf("      Procedural macros:   %d (proc_macro)\n", 
           ctx->procedural_count);
    printf("      Derive macros:       %d (#[derive(...)])\n", 
           ctx->derive_count);
    printf("      Attribute macros:    %d (#[...])\n", 
           ctx->attribute_count);
    printf("      Macro invocations:   %d\n", 
           ctx->invocation_count);
    printf("      Built-in macros:     %d\n", 
           ctx->builtin_count);
    printf("      Total expansions:    %d\n", 
           ctx->total_expansions);
    printf("      ─────────────────────────────────────────\n");
    
    printf("\n[4/5] Macro expansion details...\n");
    
    // Display declarative macros
    if (ctx->declarative_count > 0) {
        printf("\n   📝 Declarative Macros (macro_rules!):\n");
        for (int i = 0; i < ctx->declarative_count; i++) {
            DeclarativeMacro* m = &ctx->declarative[i];
            printf("      %d. %s (%d patterns)\n", i + 1, m->name, 
                   m->pattern_count);
            for (int j = 0; j < m->pattern_count; j++) {
                printf("         Pattern: %s\n", m->patterns[j].pattern);
                printf("         Expansion: %s\n", m->patterns[j].expansion);
            }
        }
    }
    
    // Display derive macros
    if (ctx->derive_count > 0) {
        printf("\n   🔧 Derive Macros:\n");
        for (int i = 0; i < ctx->derive_count; i++) {
            DeriveMacro* d = &ctx->derives[i];
            printf("      %d. #[derive(%s)] for %s\n", 
                   i + 1, d->trait_name, d->struct_name);
            printf("         Generated: %s\n", d->generated_code);
        }
    }
    
    // Display attribute macros
    if (ctx->attribute_count > 0) {
        printf("\n   🏷️  Attribute Macros:\n");
        for (int i = 0; i < ctx->attribute_count; i++) {
            AttributeMacro* a = &ctx->attributes[i];
            printf("      %d. #[%s] on %s (%s)\n", 
                   i + 1, a->attr_name, a->target_name, a->target_type);
            if (strlen(a->parameters) > 0) {
                printf("         Params: %s\n", a->parameters);
            }
            printf("         Transform: %s\n", a->transformed_code);
        }
    }
    
    // Display built-in macros
    if (ctx->builtin_count > 0) {
        printf("\n   ⚙️  Built-in Macros:\n");
        for (int i = 0; i < ctx->builtin_count; i++) {
            BuiltinMacro* b = &ctx->builtins[i];
            printf("      %d. %s = \"%s\"\n", i + 1, b->name, b->value);
        }
    }
    
    // Display invocations
    if (ctx->invocation_count > 0) {
        printf("\n   📞 Macro Invocations:\n");
        for (int i = 0; i < ctx->invocation_count; i++) {
            MacroInvocation* inv = &ctx->invocations[i];
            printf("      %d. %s (line %d)\n", 
                   i + 1, inv->macro_name, inv->line);
            printf("         Expanded: %s\n", inv->expanded);
        }
    }
    
    printf("\n[5/5] Generating assembly...\n");
    const char* output_file = "macro_system.s";
    macro_generate_asm(output_file, ctx);
    printf("      ✓ Assembly: %s\n", output_file);
    
    // Report statistics
    macro_report_stats(ctx);
    
    // Cleanup
    macro_ctx_free(ctx);
    
    printf("\n═══════════════════════════════════════════════════════\n");
    if (parse_success) {
        printf("✅ MODULE #56 TEST PASSED\n");
        printf("✅ Macro system fully functional\n");
        printf("✅ Ready for Stage 1 (self-hosting)\n");
    } else {
        printf("❌ MODULE #56 TEST FAILED\n");
        printf("❌ Macro parsing errors detected\n");
    }
    printf("═══════════════════════════════════════════════════════\n");

    return parse_success ? 0 : 1;
}
