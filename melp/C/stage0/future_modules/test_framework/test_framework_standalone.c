#include "test_framework.h"
TestCtx* test_ctx_create(void) { return (TestCtx*)calloc(1, sizeof(TestCtx)); }
void test_ctx_free(TestCtx* ctx) { if (ctx) free(ctx); }
int main() {
    printf("[MODULE #46] Test Framework Parser & Codegen\n══════════════════════════════════════════\n\n");
    const char* test_src = "@test fn test_add() { assert!(1 == 1); }\n@bench fn bench_sort() {}\n";
    TestCtx* ctx = test_ctx_create(); TestStats stats;
    test_parser_init(test_src); test_parse_file(ctx, &stats);
    printf("[3/4] Parsing test framework...\n      ✓ @test functions: %d\n      ✓ assert! macros: %d\n      ✓ @bench functions: %d\n\n", stats.tests, stats.asserts, stats.benches);
    printf("[4/4] Generating assembly...\n"); test_generate_asm("test_framework.s", ctx); printf("      ✓ Code generated\n\n");
    test_ctx_free(ctx); printf("Test Framework OK!\n✅ PASSED!\n"); return 0;
}
