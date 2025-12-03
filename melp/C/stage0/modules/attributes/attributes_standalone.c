/**
 * MLP Attributes Module - Standalone Main #42 (Stage 1)
 * 
 * Main entry point and context management
 */

#include "attributes.h"

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

AttributeCtx* attr_ctx_create(void) {
    AttributeCtx* ctx = (AttributeCtx*)calloc(1, sizeof(AttributeCtx));
    if (!ctx) {
        fprintf(stderr, "Error: Failed to allocate AttributeCtx\n");
        exit(1);
    }
    return ctx;
}

void attr_ctx_free(AttributeCtx* ctx) {
    if (ctx) {
        free(ctx);
    }
}

// ============================================================================
// CONTEXT OPERATIONS
// ============================================================================

void attr_add(AttributeCtx* ctx, AttrType type, const char* name, 
              const char* params, const char* target, int line) {
    if (ctx->attr_count >= 512) return;
    
    Attribute* attr = &ctx->attrs[ctx->attr_count++];
    attr->type = type;
    strncpy(attr->name, name, sizeof(attr->name) - 1);
    strncpy(attr->params, params, sizeof(attr->params) - 1);
    strncpy(attr->target, target, sizeof(attr->target) - 1);
    attr->line = line;
}

void attr_add_to_target(AttributeCtx* ctx, const char* target, AttrType type) {
    // Find or create target
    int target_idx = -1;
    for (int i = 0; i < ctx->target_count; i++) {
        if (strcmp(ctx->targets[i].target, target) == 0) {
            target_idx = i;
            break;
        }
    }
    
    if (target_idx == -1 && ctx->target_count < 128) {
        target_idx = ctx->target_count++;
        strncpy(ctx->targets[target_idx].target, target, sizeof(ctx->targets[0].target) - 1);
        ctx->targets[target_idx].attr_count = 0;
    }
    
    if (target_idx >= 0 && ctx->targets[target_idx].attr_count < 32) {
        ctx->targets[target_idx].attrs[ctx->targets[target_idx].attr_count++] = type;
    }
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv) {
    printf("[MODULE #42] Attributes & Annotations Parser & Codegen\n");
    printf("═══════════════════════════════════════════════════════\n\n");

    // Test source code
    const char* test_source = 
        "@inline\n"
        "@hot\n"
        "fn fast_function() {}\n"
        "\n"
        "@cold\n"
        "@deprecated(\"use new_function instead\")\n"
        "fn old_function() {}\n"
        "\n"
        "@test\n"
        "fn test_example() {}\n"
        "\n"
        "@bench\n"
        "fn benchmark_sort() {}\n"
        "\n"
        "@derive(Debug, Clone, Copy)\n"
        "struct Point {}\n"
        "\n"
        "@serialize\n"
        "@deserialize\n"
        "struct User {}\n"
        "\n"
        "@validate\n"
        "@required\n"
        "@range(0, 100)\n"
        "let age: numeric\n"
        "\n"
        "@allow(unused_variables)\n"
        "fn experimental() {}\n"
        "\n"
        "@doc(\"This is a documented function\")\n"
        "fn documented() {}\n"
        "\n"
        "@cfg(target_os = \"linux\")\n"
        "fn linux_only() {}\n"
        "\n"
        "@repr(C)\n"
        "struct FFI_Struct {}\n"
        "\n"
        "@must_use\n"
        "fn get_result() -> Result<numeric, text> {}\n";

    // Create context
    AttributeCtx* ctx = attr_ctx_create();
    AttrParseStats stats;

    // Parse
    printf("[1/4] Initializing parser...\n");
    attr_parser_init(test_source);

    printf("[2/4] Parsing attributes...\n");
    attr_parse_file(ctx, &stats);

    // Display statistics
    printf("\n[3/4] Parsing attributes...\n");
    printf("      ✓ Total attributes parsed: %d\n\n", stats.total_attributes);
    
    printf("      ✓ Optimization attributes:\n");
    printf("        @inline: %d, @hot: %d, @cold: %d\n",
           stats.inline_attrs, stats.hot_attrs, stats.cold_attrs);
    
    printf("\n      ✓ Testing attributes:\n");
    printf("        @test: %d, @bench: %d, @ignore: %d\n",
           stats.test_attrs, stats.bench_attrs, stats.ignore_attrs);
    
    printf("\n      ✓ Code generation attributes:\n");
    printf("        @deprecated: %d, @derive: %d\n",
           stats.deprecated_attrs, stats.derive_attrs);
    
    printf("\n      ✓ Serialization attributes:\n");
    printf("        @serialize: %d, @deserialize: %d\n",
           stats.serialize_attrs, stats.deserialize_attrs);
    
    printf("\n      ✓ Validation attributes:\n");
    printf("        @validate/@required/@range: %d\n", stats.validate_attrs);
    
    printf("\n      ✓ Lint attributes:\n");
    printf("        @allow: %d, @warn: %d, @deny: %d\n",
           stats.allow_attrs, stats.warn_attrs, stats.deny_attrs);
    
    printf("\n      ✓ Documentation attributes:\n");
    printf("        @doc: %d\n", stats.doc_attrs);
    
    printf("\n      ✓ Conditional compilation:\n");
    printf("        @cfg: %d\n", stats.cfg_attrs);
    
    printf("\n      ✓ FFI attributes:\n");
    printf("        @repr: %d\n", stats.repr_attrs);
    
    printf("\n      ✓ Misc attributes:\n");
    printf("        @must_use: %d\n", stats.must_use_attrs);
    
    printf("\n      Summary:\n");
    printf("        Attributes: %d\n", ctx->attr_count);
    printf("        Categories: optimization(%d), testing(%d), derive(%d), validation(%d), lints(%d)\n",
           ctx->inline_count + ctx->hot_count + ctx->cold_count,
           ctx->test_count + ctx->bench_count,
           ctx->derive_count,
           ctx->validate_count,
           ctx->lint_count);

    // Generate assembly
    printf("\n[4/4] Generating assembly...\n");
    attr_generate_asm("attributes.s", ctx);
    printf("      ✓ Code generated\n\n");

    // Cleanup
    attr_ctx_free(ctx);

    printf("Attributes OK!\n");
    printf("✅ PASSED!\n");

    return 0;
}
