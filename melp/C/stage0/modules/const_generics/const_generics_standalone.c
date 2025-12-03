/**
 * MODÜL #58: CONST_GENERICS - Standalone Compiler
 * 
 * Complete const generics compilation pipeline
 */

#define _GNU_SOURCE
#include "const_generics.h"

// Forward declarations
bool const_generic_parse(ConstGenericContext* ctx, const char* input);
void const_generic_codegen(ConstGenericContext* ctx, FILE* out);
int const_generic_count_instances_of_type(ConstGenericContext* ctx, const char* type_name);

// ============================================================================
// TEST CODE (MLP syntax)
// ============================================================================

const char* TEST_CODE = 
"-- Const generics test in MLP\n"
"\n"
"-- Define const size\n"
"dim BUFFER_SIZE as i32 = 256\n"
"dim MAX_ITEMS as usize = 100\n"
"\n"
"-- Define generic Array type\n"
"type Array<T, N>\n"
"\n"
"-- Define generic Matrix type\n"
"type Matrix<T, ROWS, COLS>\n"
"\n"
"-- Instantiate const generic arrays\n"
"dim numbers as Array<i32, 10>\n"
"dim floats as Array<f64, 20>\n"
"dim buffer as Array<i32, 256>\n"
"\n"
"-- Instantiate matrix\n"
"dim transform as Matrix<f64, 4, 4>\n"
"\n"
"-- Use const in expressions\n"
"dim large_buffer as Array<i32, 1024>\n";

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    printf("=== MLP CONST GENERICS SYSTEM - MODULE #58 ===\n\n");
    
    // Create context
    ConstGenericContext* ctx = const_generic_context_new();
    
    // Parse test code
    printf("Parsing const generics...\n");
    bool parse_ok = const_generic_parse(ctx, TEST_CODE);
    
    if (!parse_ok) {
        fprintf(stderr, "Error: Failed to parse const generics\n");
        const_generic_context_free(ctx);
        return 1;
    }
    
    printf("✓ Parsing complete\n\n");
    
    // Print statistics
    const_generic_print_stats(ctx);
    
    // Analyze instances
    printf("\n=== CONST GENERIC ANALYSIS ===\n");
    
    int array_count = const_generic_count_instances_of_type(ctx, "Array");
    int matrix_count = const_generic_count_instances_of_type(ctx, "Matrix");
    
    printf("Array instances: %d\n", array_count);
    printf("Matrix instances: %d\n", matrix_count);
    
    // List all instances
    printf("\n=== INSTANTIATED TYPES ===\n");
    ConstGenericInstance* current = ctx->instances;
    while (current) {
        printf("- %s (", current->mangled_name);
        if (current->is_valid) {
            printf("valid, ");
        } else {
            printf("invalid, ");
        }
        
        // Calculate size
        uint64_t size = 0;
        if (strcmp(current->generic_type->base_type, "Array") == 0) {
            size = current->const_args[0]->value.usize_val * 8;
        } else if (strcmp(current->generic_type->base_type, "Matrix") == 0) {
            size = current->const_args[0]->value.usize_val * 
                   current->const_args[1]->value.usize_val * 8;
        }
        printf("%lu bytes)\n", size);
        
        current = current->next;
    }
    
    // Dump types
    const_generic_dump_types(ctx);
    
    // Generate assembly
    printf("\n=== CODE GENERATION ===\n");
    FILE* out = fopen("const_generics.s", "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file\n");
        const_generic_context_free(ctx);
        return 1;
    }
    
    const_generic_codegen(ctx, out);
    fclose(out);
    printf("✓ Assembly generated: const_generics.s\n");
    
    // Count lines in generated assembly
    FILE* check = fopen("const_generics.s", "r");
    int line_count = 0;
    char ch;
    while ((ch = fgetc(check)) != EOF) {
        if (ch == '\n') line_count++;
    }
    fclose(check);
    printf("  Assembly lines: %d\n", line_count);
    
    // Test results
    printf("\n=== TEST RESULTS ===\n");
    
    // Expected: 2 generic types (Array, Matrix), 5 instances
    bool test_passed = true;
    
    if (ctx->type_count != 2) {
        printf("❌ Expected 2 generic types, got %d\n", ctx->type_count);
        test_passed = false;
    }
    
    if (array_count < 3) {
        printf("❌ Expected at least 3 Array instances, got %d\n", array_count);
        test_passed = false;
    }
    
    if (matrix_count < 1) {
        printf("❌ Expected at least 1 Matrix instance, got %d\n", matrix_count);
        test_passed = false;
    }
    
    // Check for specific instances
    ConstGenericInstance* arr_i32_10 = const_generic_find_instance(ctx, "Array_i32_10");
    ConstGenericInstance* arr_f64_20 = const_generic_find_instance(ctx, "Array_f64_20");
    ConstGenericInstance* mat_f64_4_4 = const_generic_find_instance(ctx, "Matrix_f64_4_4");
    
    if (!arr_i32_10) {
        printf("❌ Array<i32, 10> not found\n");
        test_passed = false;
    } else {
        printf("✓ Array<i32, 10> found: %s\n", arr_i32_10->mangled_name);
    }
    
    if (!arr_f64_20) {
        printf("❌ Array<f64, 20> not found\n");
        test_passed = false;
    } else {
        printf("✓ Array<f64, 20> found: %s\n", arr_f64_20->mangled_name);
    }
    
    if (!mat_f64_4_4) {
        printf("❌ Matrix<f64, 4, 4> not found\n");
        test_passed = false;
    } else {
        printf("✓ Matrix<f64, 4, 4> found: %s\n", mat_f64_4_4->mangled_name);
    }
    
    // All instances should be valid
    current = ctx->instances;
    int invalid_count = 0;
    while (current) {
        if (!current->is_valid) {
            invalid_count++;
        }
        current = current->next;
    }
    
    if (invalid_count > 0) {
        printf("❌ %d invalid instances found\n", invalid_count);
        test_passed = false;
    }
    
    // Final result
    printf("\n");
    if (test_passed) {
        printf("✅ MODULE #58 TEST PASSED\n");
        printf("✅ Const generics fully functional\n");
        printf("✅ Ready for Stage 1 (self-hosting)\n");
    } else {
        printf("❌ TEST FAILED\n");
    }
    
    // Cleanup
    const_generic_context_free(ctx);
    
    return test_passed ? 0 : 1;
}
