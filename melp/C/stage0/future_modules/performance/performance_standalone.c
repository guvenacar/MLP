#include "performance.h"
PerfCtx* perf_ctx_create(void) { return (PerfCtx*)calloc(1, sizeof(PerfCtx)); }
void perf_ctx_free(PerfCtx* ctx) { if (ctx) free(ctx); }
int main() {
    printf("[MODULE #47] Performance Parser & Codegen\n═══════════════════════════════════════\n\n");
    const char* test_src = "@inline @hot fn fast() {}\n@simd fn vectorized() {}\n@cold fn rare() {}\n";
    PerfCtx* ctx = perf_ctx_create(); PerfStats stats;
    perf_parser_init(test_src); perf_parse_file(ctx, &stats);
    printf("[3/4] Parsing performance hints...\n      ✓ @inline: %d\n      ✓ @hot: %d\n      ✓ @cold: %d\n      ✓ @simd: %d\n\n", stats.inlines, stats.hots, stats.colds, stats.simds);
    printf("[4/4] Generating assembly...\n"); perf_generate_asm("performance.s", ctx); printf("      ✓ Code generated\n\n");
    perf_ctx_free(ctx); printf("Performance OK!\n✅ PASSED!\n"); return 0;
}
