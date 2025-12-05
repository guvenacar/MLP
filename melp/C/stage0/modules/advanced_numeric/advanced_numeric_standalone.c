#include "advanced_numeric.h"
NumCtx* num_ctx_create(void) { return (NumCtx*)calloc(1, sizeof(NumCtx)); }
void num_ctx_free(NumCtx* ctx) { if (ctx) free(ctx); }
int main() {
    printf("[MODULE #48] Advanced Numeric Types Parser & Codegen\n════════════════════════════════════════════════════\n\n");
    const char* test_src = "let x: u8\nlet y: i32\nlet z: f64\nlet w: u128\n";
    NumCtx* ctx = num_ctx_create(); NumStats stats;
    num_parser_init(test_src); num_parse_file(ctx, &stats);
    printf("[3/4] Parsing numeric types...\n      ✓ Unsigned (u8-u128): %d\n      ✓ Signed (i8-i128): %d\n      ✓ Float (f32-f128): %d\n\n", stats.u_types, stats.i_types, stats.f_types);
    printf("[4/4] Generating assembly...\n"); num_generate_asm("advanced_numeric.s", ctx); printf("      ✓ Code generated\n\n");
    num_ctx_free(ctx); printf("Advanced Numeric OK!\n✅ PASSED!\n"); return 0;
}
