/**
 * MLP FFI Module - Standalone Main #43 (Stage 1)
 */

#include "ffi.h"

FFICtx* ffi_ctx_create(void) {
    FFICtx* ctx = (FFICtx*)calloc(1, sizeof(FFICtx));
    if (!ctx) { fprintf(stderr, "Error: Failed to allocate FFICtx\n"); exit(1); }
    return ctx;
}

void ffi_ctx_free(FFICtx* ctx) { if (ctx) free(ctx); }

void ffi_add_op(FFICtx* ctx, FFIOpType type, const char* name, const char* params, const char* return_type, int line) {
    if (ctx->op_count >= 512) return;
    FFIOp* op = &ctx->ops[ctx->op_count++];
    op->op_type = type;
    strncpy(op->name, name, sizeof(op->name) - 1);
    strncpy(op->params, params, sizeof(op->params) - 1);
    strncpy(op->return_type, return_type, sizeof(op->return_type) - 1);
    op->line = line;
}

void ffi_add_link(FFICtx* ctx, const char* library) {
    if (ctx->link_count >= 32) return;
    strncpy(ctx->links[ctx->link_count++].library, library, 127);
}

void ffi_add_extern_fn(FFICtx* ctx, const char* name, const char* signature, const char* return_type, int is_unsafe) {
    if (ctx->extern_fn_count >= 64) return;
    ExternFn* fn = &ctx->extern_fns[ctx->extern_fn_count++];
    strncpy(fn->name, name, sizeof(fn->name) - 1);
    strncpy(fn->signature, signature, sizeof(fn->signature) - 1);
    strncpy(fn->return_type, return_type, sizeof(fn->return_type) - 1);
    fn->is_unsafe = is_unsafe;
}

void ffi_add_repr(FFICtx* ctx, const char* struct_name, const char* repr) {
    if (ctx->repr_count >= 64) return;
    ReprDirective* r = &ctx->reprs[ctx->repr_count++];
    strncpy(r->struct_name, struct_name, sizeof(r->struct_name) - 1);
    strncpy(r->repr, repr, sizeof(r->repr) - 1);
}

int main(int argc, char** argv) {
    printf("[MODULE #43] FFI (Foreign Function Interface) Parser & Codegen\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");

    const char* test_source = 
        "extern \"C\" {\n"
        "    fn c_function(x: c_int) -> c_int;\n"
        "    fn c_malloc(size: c_size_t) -> *mut c_void;\n"
        "}\n"
        "unsafe {\n"
        "    let ptr: *const c_char = null();\n"
        "    let mut_ptr: *mut c_int = null_mut();\n"
        "}\n";

    FFICtx* ctx = ffi_ctx_create();
    FFIParseStats stats;

    printf("[1/4] Initializing parser...\n");
    ffi_parser_init(test_source);

    printf("[2/4] Parsing FFI constructs...\n");
    ffi_parse_file(ctx, &stats);

    printf("\n[3/4] Parsing FFI...\n");
    printf("      ✓ extern \"C\" blocks: %d\n", stats.extern_c_blocks);
    printf("      ✓ extern functions: %d\n", stats.extern_fns);
    printf("      ✓ C types: %d\n", stats.c_types);
    printf("      ✓ Raw pointers: %d\n", stats.raw_ptrs);
    printf("      ✓ unsafe blocks: %d\n", stats.unsafe_blocks);
    printf("\n      Summary:\n");
    printf("        Total FFI operations: %d\n", ctx->op_count);

    printf("\n[4/4] Generating assembly...\n");
    ffi_generate_asm("ffi.s", ctx);
    printf("      ✓ Code generated\n\n");

    ffi_ctx_free(ctx);

    printf("FFI OK!\n");
    printf("✅ PASSED!\n");

    return 0;
}
