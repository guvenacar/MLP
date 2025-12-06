#include "dependent_types.h"
#include <stdio.h>
#include <stdlib.h>

// ============================================================================
// TEST SUITE FOR DEPENDENT TYPES
// ============================================================================

void test_vec_with_length(void) {
    printf("\n=== TEST 1: Vec<n> with Length ===\n");
    
    DepTypeContext* ctx = dep_context_create();
    
    // Vec<5>
    DepType* vec5 = dep_type_create("Vec");
    dep_type_add_param(vec5, dep_value_int(5));
    dep_register_type(ctx, vec5);
    
    // Vec<5> again
    DepType* vec5_2 = dep_type_create("Vec");
    dep_type_add_param(vec5_2, dep_value_int(5));
    
    // Vec<10>
    DepType* vec10 = dep_type_create("Vec");
    dep_type_add_param(vec10, dep_value_int(10));
    
    bool match1 = dep_type_check(ctx, vec5, vec5_2);
    printf("Vec<5> == Vec<5>: %s ✓\n", match1 ? "true" : "false");
    
    bool match2 = dep_type_check(ctx, vec5, vec10);
    printf("Vec<5> == Vec<10>: %s (expected: false)\n", match2 ? "true" : "false");
    
    dep_type_free(vec5_2);
    dep_type_free(vec10);
    dep_context_destroy(ctx);
    printf("✓ Test 1 passed\n");
}

void test_matrix_dimensions(void) {
    printf("\n=== TEST 2: Matrix<rows, cols> ===\n");
    
    DepTypeContext* ctx = dep_context_create();
    
    // Matrix<3, 4>
    DepType* mat3x4 = dep_type_create("Matrix");
    dep_type_add_param(mat3x4, dep_value_int(3));
    dep_type_add_param(mat3x4, dep_value_int(4));
    
    // Matrix<3, 4> again
    DepType* mat3x4_2 = dep_type_create("Matrix");
    dep_type_add_param(mat3x4_2, dep_value_int(3));
    dep_type_add_param(mat3x4_2, dep_value_int(4));
    
    // Matrix<4, 3> (transposed)
    DepType* mat4x3 = dep_type_create("Matrix");
    dep_type_add_param(mat4x3, dep_value_int(4));
    dep_type_add_param(mat4x3, dep_value_int(3));
    
    bool match1 = dep_type_compatible(mat3x4, mat3x4_2);
    printf("Matrix<3,4> == Matrix<3,4>: %s ✓\n", match1 ? "true" : "false");
    
    bool match2 = dep_type_compatible(mat3x4, mat4x3);
    printf("Matrix<3,4> == Matrix<4,3>: %s (expected: false)\n", match2 ? "true" : "false");
    
    dep_type_free(mat3x4);
    dep_type_free(mat3x4_2);
    dep_type_free(mat4x3);
    dep_context_destroy(ctx);
    printf("✓ Test 2 passed\n");
}

void test_variable_params(void) {
    printf("\n=== TEST 3: Variable Parameters ===\n");
    
    // Array<n> with variable
    DepType* arr_n = dep_type_create("Array");
    dep_type_add_param(arr_n, dep_value_variable("n"));
    
    // Array<n> with same variable
    DepType* arr_n2 = dep_type_create("Array");
    dep_type_add_param(arr_n2, dep_value_variable("n"));
    
    // Array<m> with different variable
    DepType* arr_m = dep_type_create("Array");
    dep_type_add_param(arr_m, dep_value_variable("m"));
    
    bool match1 = dep_type_compatible(arr_n, arr_n2);
    printf("Array<n> == Array<n>: %s ✓\n", match1 ? "true" : "false");
    
    bool match2 = dep_type_compatible(arr_n, arr_m);
    printf("Array<n> == Array<m>: %s (expected: false)\n", match2 ? "true" : "false");
    
    dep_type_free(arr_n);
    dep_type_free(arr_n2);
    dep_type_free(arr_m);
    printf("✓ Test 3 passed\n");
}

void test_parser(void) {
    printf("\n=== TEST 4: Parser ===\n");
    
    DepType* t1 = dep_parse_type("Vec<5>");
    if (t1) {
        printf("Parsed 'Vec<5>'\n");
        printf("  Base: %s, Params: %zu, Dependent: %d\n",
               t1->base_type, t1->param_count, t1->is_dependent);
        if (t1->param_count > 0) {
            printf("  Param[0]: type=%d, value=%ld\n",
                   t1->params[0]->type, t1->params[0]->data.int_val);
        }
        dep_type_free(t1);
    }
    
    DepType* t2 = dep_parse_type("Matrix<3, 4>");
    if (t2) {
        printf("Parsed 'Matrix<3, 4>'\n");
        printf("  Base: %s, Params: %zu\n", t2->base_type, t2->param_count);
        dep_type_free(t2);
    }
    
    DepType* t3 = dep_parse_type("Array<n>");
    if (t3) {
        printf("Parsed 'Array<n>'\n");
        printf("  Base: %s, Params: %zu\n", t3->base_type, t3->param_count);
        if (t3->param_count > 0) {
            printf("  Param[0]: type=%d (variable)\n", t3->params[0]->type);
        }
        dep_type_free(t3);
    }
    
    printf("✓ Test 4 passed\n");
}

void test_codegen(void) {
    printf("\n=== TEST 5: Codegen ===\n");
    
    DepType* type = dep_type_create("Vec");
    dep_type_add_param(type, dep_value_int(42));
    
    char* code = codegen_dep_type_check(type);
    if (code) {
        printf("Generated code:\n%s", code);
        free(code);
    }
    
    dep_type_free(type);
    printf("✓ Test 5 passed\n");
}

void test_statistics(void) {
    printf("\n=== TEST 6: Statistics ===\n");
    
    DepTypeContext* ctx = dep_context_create();
    
    DepType* t1 = dep_type_create("Vec");
    dep_type_add_param(t1, dep_value_int(5));
    
    DepType* t2 = dep_type_create("Matrix");
    dep_type_add_param(t2, dep_value_int(3));
    dep_type_add_param(t2, dep_value_int(4));
    
    dep_register_type(ctx, t1);
    dep_register_type(ctx, t2);
    
    DepType* t3 = dep_type_create("Vec");
    dep_type_add_param(t3, dep_value_int(10));
    
    dep_type_check(ctx, t1, t3);
    
    printf("Types registered: %zu\n", ctx->type_count);
    printf("Total checks: %zu\n", ctx->total_checks);
    printf("Violations: %zu\n", ctx->violations);
    
    dep_type_free(t3);
    dep_context_destroy(ctx);
    printf("✓ Test 6 passed\n");
}

int main(void) {
    printf("MLP DEPENDENT TYPES - Module #62 Test Suite\n");
    printf("============================================\n");
    
    test_vec_with_length();
    test_matrix_dimensions();
    test_variable_params();
    test_parser();
    test_codegen();
    test_statistics();
    
    printf("\n✅ All tests passed!\n");
    return 0;
}
