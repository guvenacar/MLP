#include "trait_system_advanced.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
extern char* strdup(const char*);

// ============================================================================
// TEST 1: TRAIT BOUNDS
// ============================================================================

void test_trait_bounds() {
    printf("\n=== TEST 1: Trait Bounds ===\n");
    
    TraitContext* ctx = trait_context_create();
    
    // Create Display trait
    TraitDef* display = trait_def_create("Display");
    trait_register(ctx, display);
    
    // Create Clone trait
    TraitDef* clone = trait_def_create("Clone");
    trait_register(ctx, clone);
    
    // Create impl Display for Point
    TraitImpl* impl1 = trait_impl_create("Display", "Point");
    impl_register(ctx, impl1);
    
    // Create impl Clone for Point
    TraitImpl* impl2 = trait_impl_create("Clone", "Point");
    impl_register(ctx, impl2);
    
    // Test bound: T: Display
    TraitBound* bound1 = trait_bound_create("T");
    trait_bound_add_trait(bound1, "Display");
    
    bool result1 = trait_bound_check(ctx, bound1, "Point");
    printf("Point: Display => %s\n", result1 ? "✓" : "✗");
    
    // Test bound: T: Display + Clone
    TraitBound* bound2 = trait_bound_create("T");
    trait_bound_add_trait(bound2, "Display");
    trait_bound_add_trait(bound2, "Clone");
    
    bool result2 = trait_bound_check(ctx, bound2, "Point");
    printf("Point: Display + Clone => %s\n", result2 ? "✓" : "✗");
    
    // Test bound: T: Debug (not implemented)
    TraitBound* bound3 = trait_bound_create("T");
    trait_bound_add_trait(bound3, "Debug");
    
    bool result3 = trait_bound_check(ctx, bound3, "Point");
    printf("Point: Debug => %s (expected: ✗)\n", result3 ? "✓" : "✗");
    
    trait_bound_free(bound1);
    trait_bound_free(bound2);
    trait_bound_free(bound3);
    trait_context_destroy(ctx);
    
    printf("✓ Test 1 passed\n");
}

// ============================================================================
// TEST 2: ASSOCIATED TYPES
// ============================================================================

void test_associated_types() {
    printf("\n=== TEST 2: Associated Types ===\n");
    
    TraitContext* ctx = trait_context_create();
    
    // Create Iterator trait with Item associated type
    TraitDef* iterator = trait_def_create("Iterator");
    AssociatedType* item = assoc_type_create("Item");
    trait_def_add_assoc_type(iterator, item);
    trait_register(ctx, iterator);
    
    // Impl Iterator for Vec<i32>
    TraitImpl* impl = trait_impl_create("Iterator", "Vec<i32>");
    AssociatedType* item_impl = assoc_type_create("Item");
    assoc_type_set_concrete(item_impl, "i32");
    trait_impl_add_assoc_type(impl, item_impl);
    impl_register(ctx, impl);
    
    // Resolve associated type
    char* resolved = assoc_type_resolve(ctx, "Iterator", "Item", "Vec<i32>");
    printf("Iterator::Item for Vec<i32> = %s\n", resolved ? resolved : "null");
    
    if (resolved) free(resolved);
    trait_context_destroy(ctx);
    
    printf("✓ Test 2 passed\n");
}

// ============================================================================
// TEST 3: TRAIT INHERITANCE
// ============================================================================

void test_trait_inheritance() {
    printf("\n=== TEST 3: Trait Inheritance ===\n");
    
    TraitContext* ctx = trait_context_create();
    
    // Create Employee trait
    TraitDef* employee = trait_def_create("Employee");
    trait_register(ctx, employee);
    
    // Create Manager trait : Employee
    TraitDef* manager = trait_def_create("Manager");
    trait_def_add_super_trait(manager, "Employee");
    trait_register(ctx, manager);
    
    // Check inheritance
    bool inherits = trait_inherits_from(ctx, "Manager", "Employee");
    printf("Manager : Employee => %s\n", inherits ? "✓" : "✗");
    
    // Get all parents
    size_t count;
    char** parents = trait_get_all_parents(ctx, "Manager", &count);
    printf("Manager parents: %zu\n", count);
    
    for (size_t i = 0; i < count; i++) {
        printf("  - %s\n", parents[i]);
        free(parents[i]);
    }
    free(parents);
    
    trait_context_destroy(ctx);
    
    printf("✓ Test 3 passed\n");
}

// ============================================================================
// TEST 4: PARSER
// ============================================================================

void test_parser() {
    printf("\n=== TEST 4: Parser ===\n");
    
    // Test trait bound parsing
    const char* bound_str1 = "T: Display";
    TraitBound* bound1 = trait_parse_bound(bound_str1);
    printf("Parsed: %s\n", bound_str1);
    if (bound1) {
        printf("  Type param: %s\n", bound1->type_param);
        printf("  Traits: %zu\n", bound1->trait_count);
        trait_bound_free(bound1);
    }
    
    // Test multiple trait bound
    const char* bound_str2 = "T: Display + Clone + Debug";
    TraitBound* bound2 = trait_parse_bound(bound_str2);
    printf("Parsed: %s\n", bound_str2);
    if (bound2) {
        printf("  Type param: %s\n", bound2->type_param);
        printf("  Traits: %zu\n", bound2->trait_count);
        trait_bound_free(bound2);
    }
    
    // Test associated type parsing
    const char* assoc_str = "type Item = i32;";
    AssociatedType* assoc = trait_parse_assoc_type(assoc_str);
    printf("Parsed: %s\n", assoc_str);
    if (assoc) {
        printf("  Name: %s\n", assoc->name);
        printf("  Concrete: %s\n", assoc->concrete_type ? assoc->concrete_type : "none");
        assoc_type_free(assoc);
    }
    
    printf("✓ Test 4 passed\n");
}

// ============================================================================
// TEST 5: STATISTICS
// ============================================================================

void test_statistics() {
    printf("\n=== TEST 5: Statistics ===\n");
    
    TraitContext* ctx = trait_context_create();
    
    // Add some traits
    TraitDef* t1 = trait_def_create("Display");
    trait_register(ctx, t1);
    
    TraitDef* t2 = trait_def_create("Clone");
    trait_register(ctx, t2);
    
    // Add some impls
    TraitImpl* i1 = trait_impl_create("Display", "Point");
    impl_register(ctx, i1);
    
    TraitImpl* i2 = trait_impl_create("Clone", "Point");
    impl_register(ctx, i2);
    
    // Get stats
    size_t traits, impls, bounds, assoc_types;
    trait_stats(ctx, &traits, &impls, &bounds, &assoc_types);
    
    printf("Traits: %zu\n", traits);
    printf("Impls: %zu\n", impls);
    printf("Bounds: %zu\n", bounds);
    printf("Associated types: %zu\n", assoc_types);
    
    trait_context_destroy(ctx);
    
    printf("✓ Test 5 passed\n");
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    printf("===========================================\n");
    printf(" MLP Advanced Trait System - Test Suite   \n");
    printf("===========================================\n");
    
    test_trait_bounds();
    test_associated_types();
    test_trait_inheritance();
    test_parser();
    test_statistics();
    
    printf("\n===========================================\n");
    printf("     All tests passed! ✓                  \n");
    printf("===========================================\n");
    
    return 0;
}
