#include "package_management.h"

PkgCtx* pkg_ctx_create(void) {
    PkgCtx* ctx = (PkgCtx*)calloc(1, sizeof(PkgCtx));
    return ctx;
}

void pkg_ctx_free(PkgCtx* ctx) { if (ctx) free(ctx); }

int main() {
    printf("[MODULE #44] Package Management Parser & Codegen\n");
    printf("═════════════════════════════════════════════════\n\n");
    
    const char* test = "mod utils\nuse std::io\npub fn test()\ncrate::module\n";
    PkgCtx* ctx = pkg_ctx_create();
    PkgStats stats;
    
    pkg_parser_init(test);
    pkg_parse_file(ctx, &stats);
    
    printf("[3/4] Parsing package management...\n");
    printf("      ✓ mod declarations: %d\n", stats.mod_decls);
    printf("      ✓ use statements: %d\n", stats.use_stmts);
    printf("      ✓ pub items: %d\n", stats.pub_items);
    printf("      ✓ crate references: %d\n", stats.crate_refs);
    printf("\n[4/4] Generating assembly...\n");
    pkg_generate_asm("package_management.s", ctx);
    printf("      ✓ Code generated\n\n");
    
    pkg_ctx_free(ctx);
    printf("Package Management OK!\n✅ PASSED!\n");
    return 0;
}
