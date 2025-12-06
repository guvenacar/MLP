#include "advanced_numeric.h"

int main() {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("[MODULE #48] Advanced Numeric Types - Parser & Codegen\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    // Test source code with various numeric types
    const char* test_src = 
        "let x: u8 = 255\n"
        "let y: i32 = -42\n"
        "let z: f64 = 3.14159\n"
        "let w: u128 = 123456789\n"
        "let a: i64 = -999\n"
        "let b: f32 = 2.718\n";
    
    printf("[1/4] Creating context...\n");
    NumCtx* ctx = num_ctx_create();
    if (!ctx) {
        printf("      ✗ Failed to create context\n");
        return 1;
    }
    printf("      ✓ Context created\n\n");
    
    printf("[2/4] Initializing parser...\n");
    num_parser_init(test_src);
    printf("      ✓ Parser initialized\n\n");
    
    printf("[3/4] Parsing numeric types...\n");
    NumStats stats;
    num_parse_file(ctx, &stats);
    printf("      ✓ Unsigned types (u8-u128): %d\n", stats.u_types);
    printf("      ✓ Signed types (i8-i128): %d\n", stats.i_types);
    printf("      ✓ Float types (f32-f128): %d\n", stats.f_types);
    printf("      Total operations: %d\n\n", ctx->op_count);
    
    printf("[4/4] Generating assembly...\n");
    num_generate_asm("advanced_numeric.s", ctx);
    printf("      ✓ Assembly code generated → advanced_numeric.s\n\n");
    
    // Cleanup
    num_ctx_free(ctx);
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("✅ Advanced Numeric Module - PASSED!\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    return 0;
}
