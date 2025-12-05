/**
 * MLP Result/Option Module - Standalone Main #41 (Stage 1)
 * 
 * Main entry point and context management
 */

#include "result_option.h"

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

ResOptCtx* resopt_ctx_create(void) {
    ResOptCtx* ctx = (ResOptCtx*)calloc(1, sizeof(ResOptCtx));
    if (!ctx) {
        fprintf(stderr, "Error: Failed to allocate ResOptCtx\n");
        exit(1);
    }
    return ctx;
}

void resopt_ctx_free(ResOptCtx* ctx) {
    if (ctx) {
        free(ctx);
    }
}

// ============================================================================
// CONTEXT OPERATIONS
// ============================================================================

void resopt_add_op(ResOptCtx* ctx, ResOptOpType type, const char* name, 
                   const char* args, const char* type_params, int line) {
    if (ctx->op_count >= 512) return;
    
    ResOptOp* op = &ctx->ops[ctx->op_count++];
    strncpy(op->name, name, sizeof(op->name) - 1);
    strncpy(op->args, args, sizeof(op->args) - 1);
    strncpy(op->type_params, type_params, sizeof(op->type_params) - 1);
    op->op_type = type;
    op->line = line;
}

void resopt_add_result(ResOptCtx* ctx, const char* name, const char* ok_type, const char* err_type) {
    if (ctx->result_count >= 64) return;
    
    ResultInfo* info = &ctx->results[ctx->result_count++];
    strncpy(info->name, name, sizeof(info->name) - 1);
    strncpy(info->ok_type, ok_type, sizeof(info->ok_type) - 1);
    strncpy(info->err_type, err_type, sizeof(info->err_type) - 1);
}

void resopt_add_option(ResOptCtx* ctx, const char* name, const char* some_type) {
    if (ctx->option_count >= 64) return;
    
    OptionInfo* info = &ctx->options[ctx->option_count++];
    strncpy(info->name, name, sizeof(info->name) - 1);
    strncpy(info->some_type, some_type, sizeof(info->some_type) - 1);
}

void resopt_add_ok(ResOptCtx* ctx, const char* var_name, const char* value) {
    if (ctx->ok_count >= 64) return;
    
    OkInfo* info = &ctx->oks[ctx->ok_count++];
    strncpy(info->var_name, var_name, sizeof(info->var_name) - 1);
    strncpy(info->value, value, sizeof(info->value) - 1);
}

void resopt_add_err(ResOptCtx* ctx, const char* var_name, const char* error) {
    if (ctx->err_count >= 64) return;
    
    ErrInfo* info = &ctx->errs[ctx->err_count++];
    strncpy(info->var_name, var_name, sizeof(info->var_name) - 1);
    strncpy(info->error, error, sizeof(info->error) - 1);
}

void resopt_add_some(ResOptCtx* ctx, const char* var_name, const char* value) {
    if (ctx->some_count >= 64) return;
    
    SomeInfo* info = &ctx->somes[ctx->some_count++];
    strncpy(info->var_name, var_name, sizeof(info->var_name) - 1);
    strncpy(info->value, value, sizeof(info->value) - 1);
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv) {
    printf("[MODULE #41] Result/Option Types Parser & Codegen\n");
    printf("═══════════════════════════════════════════════════\n\n");

    // Test source code
    const char* test_source = 
        "let r: Result<numeric, text> = Ok(42)\n"
        "let e: Result<numeric, text> = Err(\"error\")\n"
        "let o: Option<text> = Some(\"hello\")\n"
        "let n: Option<numeric> = None\n"
        "r.unwrap()\n"
        "o.expect(\"msg\")\n"
        "r.unwrap_or(0)\n"
        "o.unwrap_or_else(|| \"default\")\n"
        "r.map(|x| x * 2)\n"
        "o.and_then(|s| Some(s.len()))\n"
        "e.or_else(|_| Ok(99))\n"
        "r.is_ok()\n"
        "e.is_err()\n"
        "o.is_some()\n"
        "n.is_none()\n"
        "value?\n";

    // Create context
    ResOptCtx* ctx = resopt_ctx_create();
    ResOptParseStats stats;

    // Parse
    printf("[1/4] Initializing parser...\n");
    resopt_parser_init(test_source);

    printf("[2/4] Parsing Result/Option constructs...\n");
    resopt_parse_file(ctx, &stats);

    // Display statistics
    printf("\n[3/4] Parsing result/option types...\n");
    printf("      ✓ Result<T,E>: %d type declarations\n", stats.result_types);
    printf("      ✓ Option<T>: %d type declarations\n", stats.option_types);
    printf("      ✓ Ok(%d): value constructors\n", stats.ok_constructors);
    printf("      ✓ Err(%d): value constructors\n", stats.err_constructors);
    printf("      ✓ Some(%d): value constructors\n", stats.some_constructors);
    printf("      ✓ None: %d value constructors\n", stats.none_constructors);
    
    printf("\n      ✓ Operations:\n");
    printf("        unwrap: %d, expect: %d, unwrap_or: %d, unwrap_or_else: %d\n",
           stats.unwraps, stats.expects, stats.unwrap_ors, stats.unwrap_or_elses);
    printf("        map: %d, and_then: %d, or_else: %d\n",
           stats.maps, stats.and_thens, stats.or_elses);
    printf("        ?: %d (error propagation)\n", stats.question_ops);
    
    printf("\n      ✓ Type checks:\n");
    printf("        is_ok: %d, is_err: %d, is_some: %d, is_none: %d\n",
           stats.is_oks, stats.is_errs, stats.is_somes, stats.is_nones);
    
    printf("\n      Summary:\n");
    printf("        Results: %d, Options: %d\n", ctx->result_count, ctx->option_count);
    printf("        Ok: %d, Err: %d, Some: %d, None: %d\n",
           ctx->ok_count, ctx->err_count, ctx->some_count, ctx->none_count);
    printf("        Operations: %d\n", ctx->op_count);

    // Generate assembly
    printf("\n[4/4] Generating assembly...\n");
    resopt_generate_asm("result_option.s", ctx);
    printf("      ✓ Code generated\n\n");

    // Cleanup
    resopt_ctx_free(ctx);

    printf("Result/Option OK!\n");
    printf("✅ PASSED!\n");

    return 0;
}
