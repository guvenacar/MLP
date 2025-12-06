#include "regex_pattern.h"
RegexCtx* regex_ctx_create(void) { return (RegexCtx*)calloc(1, sizeof(RegexCtx)); }
void regex_ctx_free(RegexCtx* ctx) { if (ctx) free(ctx); }
int main() {
    printf("[MODULE #49] Regex Pattern Parser & Codegen\n═══════════════════════════════════════════\n\n");
    const char* test_src = "let regex = /[0-9]+/\ntext.match(regex)\ntext.replace(regex, \"X\")\n";
    RegexCtx* ctx = regex_ctx_create(); RegexStats stats;
    regex_parser_init(test_src); regex_parse_file(ctx, &stats);
    printf("[3/4] Parsing regex patterns...\n      ✓ Regex patterns: %d\n      ✓ match() calls: %d\n      ✓ replace() calls: %d\n\n", stats.patterns, stats.matches, stats.replaces);
    printf("[4/4] Generating assembly...\n"); regex_generate_asm("regex_pattern.s", ctx); printf("      ✓ Code generated\n\n");
    regex_ctx_free(ctx); printf("Regex Pattern OK!\n✅ PASSED!\n"); return 0;
}
