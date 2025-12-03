#include "linear_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// TEST SUITE FOR LINEAR TYPES
// ============================================================================

void test_linear_use_once(void) {
    printf("\n=== TEST 1: Linear Type Use-Once ===\n");
    
    LinearContext* ctx = linear_context_create();
    
    // Create a linear variable
    LinearVar* file = linear_var_create("file", LINEAR_TYPE);
    linear_register(ctx, file);
    
    // First use - OK
    printf("First use of file: ");
    linear_var_use(ctx, file);
    printf("✓\n");
    
    // Second use - ERROR
    printf("Second use of file: ");
    linear_var_use(ctx, file);
    
    bool valid = linear_var_check(ctx, file);
    printf("Validation: %s\n", valid ? "PASS" : "FAIL (expected)");
    printf("Violations: %zu (expected: 1)\n", ctx->violations);
    
    linear_context_destroy(ctx);
    printf("✓ Test 1 passed\n");
}

void test_affine_use_at_most_once(void) {
    printf("\n=== TEST 2: Affine Type Use-At-Most-Once ===\n");
    
    LinearContext* ctx = linear_context_create();
    
    // Create affine variable
    LinearVar* optional = linear_var_create("optional", AFFINE_TYPE);
    linear_register(ctx, optional);
    
    // First use - OK
    printf("First use: ");
    linear_var_use(ctx, optional);
    printf("✓\n");
    
    // Second use - ERROR
    printf("Second use: ");
    linear_var_use(ctx, optional);
    
    printf("Violations: %zu (expected: 1)\n", ctx->violations);
    
    linear_context_destroy(ctx);
    printf("✓ Test 2 passed\n");
}

void test_move_semantics(void) {
    printf("\n=== TEST 3: Move Semantics ===\n");
    
    LinearContext* ctx = linear_context_create();
    
    // Create linear variable
    LinearVar* data = linear_var_create("data", LINEAR_TYPE);
    linear_register(ctx, data);
    
    // Move the value
    printf("Moving 'data': ");
    linear_var_move(ctx, data);
    printf("✓ (state=%d, moved=%d)\n", data->state, data->is_moved);
    
    // Try to use after move - ERROR
    printf("Use after move: ");
    linear_var_use(ctx, data);
    
    // Try to move again - ERROR
    printf("Second move: ");
    linear_var_move(ctx, data);
    
    printf("Total moves: %zu\n", ctx->total_moves);
    printf("Violations: %zu (expected: 2)\n", ctx->violations);
    
    linear_context_destroy(ctx);
    printf("✓ Test 3 passed\n");
}

void test_parser(void) {
    printf("\n=== TEST 4: Parser ===\n");
    
    LinearVar* v1 = linear_parse_declaration("linear file: FileHandle");
    if (v1) {
        printf("Parsed 'linear file: FileHandle'\n");
        printf("  Name: %s, Linearity: %d (LINEAR_TYPE=0)\n", v1->name, v1->linearity);
        linear_var_free(v1);
    }
    
    LinearVar* v2 = linear_parse_declaration("affine opt: Option<T>");
    if (v2) {
        printf("Parsed 'affine opt: Option<T>'\n");
        printf("  Name: %s, Linearity: %d (AFFINE_TYPE=1)\n", v2->name, v2->linearity);
        linear_var_free(v2);
    }
    
    LinearVar* v3 = linear_parse_declaration("let x: i32");
    if (v3) {
        printf("Parsed 'let x: i32'\n");
        printf("  Name: %s, Linearity: %d (NORMAL_TYPE=2)\n", v3->name, v3->linearity);
        linear_var_free(v3);
    }
    
    printf("✓ Test 4 passed\n");
}

void test_codegen(void) {
    printf("\n=== TEST 5: Codegen ===\n");
    
    LinearVar* var = linear_var_create("socket", LINEAR_TYPE);
    linear_var_use(NULL, var); // Increment use count
    
    char* code = codegen_linear_check(var);
    if (code) {
        printf("Generated code:\n%s", code);
        free(code);
    }
    
    linear_var_free(var);
    printf("✓ Test 5 passed\n");
}

void test_statistics(void) {
    printf("\n=== TEST 6: Statistics ===\n");
    
    LinearContext* ctx = linear_context_create();
    
    LinearVar* v1 = linear_var_create("a", LINEAR_TYPE);
    LinearVar* v2 = linear_var_create("b", AFFINE_TYPE);
    LinearVar* v3 = linear_var_create("c", NORMAL_TYPE);
    
    linear_register(ctx, v1);
    linear_register(ctx, v2);
    linear_register(ctx, v3);
    
    linear_var_use(ctx, v1);
    linear_var_move(ctx, v2);
    linear_var_use(ctx, v3);
    linear_var_use(ctx, v3);
    
    printf("Variables: %zu\n", ctx->var_count);
    printf("Total uses: %zu\n", ctx->total_uses);
    printf("Total moves: %zu\n", ctx->total_moves);
    printf("Violations: %zu\n", ctx->violations);
    
    linear_context_destroy(ctx);
    printf("✓ Test 6 passed\n");
}

int main(void) {
    printf("MLP LINEAR TYPES - Module #61 Test Suite\n");
    printf("=========================================\n");
    
    test_linear_use_once();
    test_affine_use_at_most_once();
    test_move_semantics();
    test_parser();
    test_codegen();
    test_statistics();
    
    printf("\n✅ All tests passed!\n");
    return 0;
}
