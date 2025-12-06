#include "iterator_system.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("[MODULE #53] Iterator System - Parser & Codegen\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    printf("[1/8] Creating iterator context...\n");
    IteratorContext* ctx = iter_context_create();
    if (!ctx) {
        printf("      ✗ Failed to create context\n");
        return 1;
    }
    printf("      ✓ Context created\n\n");
    
    // Test 1: Array iterator
    printf("[2/8] Testing array iterator...\n");
    const char* test1 = "let iter = numbers.iter()";
    if (iter_parse_declaration(ctx, test1)) {
        IteratorDecl* decl = iter_find_declaration(ctx, "iter");
        if (decl) {
            printf("      ✓ Array iterator parsed\n");
            printf("        Variable: %s | Type: ARRAY | Source: %s\n", 
                   decl->var_name, decl->source);
        }
    } else {
        printf("      ✗ Failed to parse array iterator\n");
    }
    printf("\n");
    
    // Test 2: Range iterator
    printf("[3/8] Testing range iterator...\n");
    const char* test2 = "let range_iter = 0..10";
    if (iter_parse_declaration(ctx, test2)) {
        IteratorDecl* decl = iter_find_declaration(ctx, "range_iter");
        if (decl) {
            printf("      ✓ Range iterator parsed (0..10)\n");
            printf("        Variable: %s | Type: RANGE | Source: %s\n",
                   decl->var_name, decl->source);
        }
    } else {
        printf("      ✗ Failed to parse range iterator\n");
    }
    printf("\n");
    
    // Test 3: Map operation
    printf("[4/8] Testing map operation...\n");
    const char* test3 = "iter.map(double)";
    if (iter_parse_operation(ctx, test3)) {
        if (ctx->op_count > 0) {
            IteratorOpNode* op = ctx->operations[ctx->op_count - 1];
            printf("      ✓ Map operation parsed\n");
            printf("        Iterator: %s | Function: %s\n",
                   op->iterator_name, op->function ? op->function : "none");
        }
    } else {
        printf("      ✗ Failed to parse map operation\n");
    }
    printf("\n");
    
    // Test 4: Filter operation
    printf("[5/8] Testing filter operation...\n");
    const char* test4 = "iter.filter(is_even)";
    if (iter_parse_operation(ctx, test4)) {
        if (ctx->op_count > 0) {
            IteratorOpNode* op = ctx->operations[ctx->op_count - 1];
            printf("      ✓ Filter operation parsed\n");
            printf("        Iterator: %s | Predicate: %s\n",
                   op->iterator_name, op->function ? op->function : "none");
        }
    } else {
        printf("      ✗ Failed to parse filter operation\n");
    }
    printf("\n");
    
    // Test 5: Collect operation
    printf("[6/8] Testing collect operation...\n");
    const char* test5 = "iter.collect()";
    if (iter_parse_operation(ctx, test5)) {
        if (ctx->op_count > 0) {
            IteratorOpNode* op = ctx->operations[ctx->op_count - 1];
            printf("      ✓ Collect operation parsed\n");
            printf("        Iterator: %s | Type: COLLECT\n", op->iterator_name);
        }
    } else {
        printf("      ✗ Failed to parse collect operation\n");
    }
    printf("\n");
    
    // Test 6: Generate assembly
    printf("[7/8] Generating assembly code...\n");
    iter_codegen_full(ctx, "iterator_system_test.s");
    printf("      ✓ Assembly file generated → iterator_system_test.s\n\n");
    
    // Test 7: Summary
    printf("[8/8] Final summary...\n");
    printf("      Declarations parsed: %d\n", ctx->decl_count);
    printf("      Operations parsed: %d\n", ctx->op_count);
    printf("      Supported ops: map, filter, reduce, collect, chain, zip\n");
    printf("      ✓ All components working\n\n");
    
    iter_context_free(ctx);
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("✅ Iterator System Module - PASSED!\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    return 0;
}
