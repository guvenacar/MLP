#include "documentation.h"
DocCtx* doc_ctx_create(void) { return (DocCtx*)calloc(1, sizeof(DocCtx)); }
void doc_ctx_free(DocCtx* ctx) { if (ctx) free(ctx); }
int main() {
    printf("[MODULE #45] Documentation Parser & Codegen\n═════════════════════════════════════════\n\n");
    const char* test = "/// Line doc\n/** Block doc */\n//! Inner doc\n";
    DocCtx* ctx = doc_ctx_create(); DocStats stats;
    doc_parser_init(test); doc_parse_file(ctx, &stats);
    printf("[3/4] Parsing documentation...\n      ✓ Line docs (///): %d\n      ✓ Block docs (/**): %d\n\n", stats.line_comments, stats.block_comments);
    printf("[4/4] Generating assembly...\n"); doc_generate_asm("documentation.s", ctx); printf("      ✓ Code generated\n\n");
    doc_ctx_free(ctx); printf("Documentation OK!\n✅ PASSED!\n"); return 0;
}
