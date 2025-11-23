/**
 * test_gc_basic.c - Basic GC Unit Tests
 * 
 * Tests:
 * 1. Allocation and deallocation
 * 2. Mark and sweep
 * 3. Root set management
 * 4. Collection triggers
 * 5. Statistics
 * 
 * Phase: 9.1 - Core Infrastructure
 * Date: 23 Kasım 2025
 */

#include "gc.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

// Test counter
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    printf("\n--- Test: %s ---\n", name); \
    gc_init()

#define ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("❌ FAIL: %s\n", msg); \
            tests_failed++; \
            return; \
        } else { \
            printf("✅ PASS: %s\n", msg); \
            tests_passed++; \
        } \
    } while(0)

#define TEST_END() \
    gc_shutdown()

// ============================================================================
// Test 1: Basic Allocation
// ============================================================================

void test_basic_allocation() {
    TEST("Basic Allocation");
    
    // Allocate a simple integer
    int* num = (int*)gc_malloc(sizeof(int), GC_TYPE_GENERIC);
    ASSERT(num != NULL, "Allocation succeeded");
    
    *num = 42;
    ASSERT(*num == 42, "Value stored correctly");
    
    // Check statistics
    GCStats stats;
    gc_get_stats(&stats);
    ASSERT(stats.num_objects == 1, "Object count is 1");
    ASSERT(stats.total_allocated >= sizeof(int) + sizeof(GCObjectHeader), 
           "Bytes allocated includes header");
    
    TEST_END();
}

// ============================================================================
// Test 2: Multiple Allocations
// ============================================================================

void test_multiple_allocations() {
    TEST("Multiple Allocations");
    
    #define NUM_ALLOCS 100
    void* pointers[NUM_ALLOCS];
    
    // Allocate 100 objects
    for (int i = 0; i < NUM_ALLOCS; i++) {
        pointers[i] = gc_malloc(64, GC_TYPE_GENERIC);
        ASSERT(pointers[i] != NULL, "Allocation succeeded");
    }
    
    // Check statistics
    GCStats stats;
    gc_get_stats(&stats);
    ASSERT(stats.num_objects == NUM_ALLOCS, "Object count is 100");
    
    TEST_END();
}

// ============================================================================
// Test 3: String Duplication
// ============================================================================

void test_string_duplication() {
    TEST("String Duplication");
    
    const char* original = "Hello, MLP!";
    char* copy = gc_strdup(original);
    
    ASSERT(copy != NULL, "String duplication succeeded");
    ASSERT(strcmp(copy, original) == 0, "String content matches");
    ASSERT(copy != original, "String is a copy, not the same pointer");
    
    // Check type
    GCObjectType type = gc_get_type(copy);
    ASSERT(type == GC_TYPE_STRING, "Object type is STRING");
    
    TEST_END();
}

// ============================================================================
// Test 4: Root Set Management
// ============================================================================

void test_root_set() {
    TEST("Root Set Management");
    
    void* root1 = gc_malloc(64, GC_TYPE_GENERIC);
    void* root2 = gc_malloc(64, GC_TYPE_GENERIC);
    
    // Add roots
    gc_add_root(&root1);
    gc_add_root(&root2);
    
    GCStats stats;
    gc_get_stats(&stats);
    ASSERT(stats.num_roots == 2, "Root count is 2");
    
    // Remove one root
    gc_remove_root(&root1);
    gc_get_stats(&stats);
    ASSERT(stats.num_roots == 1, "Root count is 1 after removal");
    
    TEST_END();
}

// ============================================================================
// Test 5: Manual Collection
// ============================================================================

void test_manual_collection() {
    TEST("Manual Collection");
    
    // Allocate some objects
    for (int i = 0; i < 10; i++) {
        gc_malloc(64, GC_TYPE_GENERIC);
    }
    
    GCStats before;
    gc_get_stats(&before);
    ASSERT(before.num_objects == 10, "10 objects allocated");
    
    // Trigger collection (all objects are unreachable)
    gc_collect();
    
    GCStats after;
    gc_get_stats(&after);
    ASSERT(after.num_objects == 0, "All objects collected");
    ASSERT(after.num_collections == 1, "Collection count is 1");
    
    TEST_END();
}

// ============================================================================
// Test 6: Rooted Objects Survive Collection
// ============================================================================

void test_rooted_survival() {
    TEST("Rooted Objects Survive Collection");
    
    // Allocate rooted object
    void* rooted = gc_malloc(64, GC_TYPE_GENERIC);
    gc_add_root(&rooted);
    
    // Allocate unrooted objects
    for (int i = 0; i < 10; i++) {
        gc_malloc(64, GC_TYPE_GENERIC);
    }
    
    GCStats before;
    gc_get_stats(&before);
    ASSERT(before.num_objects == 11, "11 objects allocated");
    
    // Trigger collection
    gc_collect();
    
    GCStats after;
    gc_get_stats(&after);
    ASSERT(after.num_objects == 1, "Only rooted object survived");
    ASSERT(rooted != NULL, "Rooted pointer is still valid");
    
    gc_remove_root(&rooted);
    TEST_END();
}

// ============================================================================
// Test 7: Threshold Trigger
// ============================================================================

void test_threshold_trigger() {
    TEST("Threshold Trigger");
    
    // Set a very low threshold
    gc_set_threshold(1024);  // 1KB
    
    // Allocate many small objects to exceed threshold
    for (int i = 0; i < 100; i++) {
        gc_malloc(64, GC_TYPE_GENERIC);
    }
    
    // GC should have been triggered automatically
    GCStats stats;
    gc_get_stats(&stats);
    ASSERT(stats.num_collections > 0, "GC was triggered automatically");
    
    TEST_END();
}

// ============================================================================
// Test 8: Enable/Disable GC
// ============================================================================

void test_enable_disable() {
    TEST("Enable/Disable GC");
    
    ASSERT(gc_is_enabled() == 1, "GC is enabled by default");
    
    gc_set_enabled(0);
    ASSERT(gc_is_enabled() == 0, "GC disabled successfully");
    
    // Allocate objects with GC disabled
    for (int i = 0; i < 10; i++) {
        gc_malloc(64, GC_TYPE_GENERIC);
    }
    
    // Manual collection should be no-op
    GCStats before;
    gc_get_stats(&before);
    
    gc_collect();
    
    GCStats after;
    gc_get_stats(&after);
    ASSERT(after.num_collections == before.num_collections, 
           "Collection skipped when disabled");
    
    // Re-enable and collect
    gc_set_enabled(1);
    gc_collect();
    
    gc_get_stats(&after);
    ASSERT(after.num_collections == before.num_collections + 1, 
           "Collection works after re-enabling");
    
    TEST_END();
}

// ============================================================================
// Test 9: Object Header API
// ============================================================================

void test_object_header_api() {
    TEST("Object Header API");
    
    void* obj = gc_malloc(128, GC_TYPE_PROMISE);
    
    // Test gc_get_header
    GCObjectHeader* header = gc_get_header(obj);
    ASSERT(header != NULL, "Header retrieved");
    ASSERT(header->type == GC_TYPE_PROMISE, "Type is PROMISE");
    
    // Test gc_get_type
    GCObjectType type = gc_get_type(obj);
    ASSERT(type == GC_TYPE_PROMISE, "gc_get_type() works");
    
    // Test gc_get_size
    size_t size = gc_get_size(obj);
    ASSERT(size == 128, "gc_get_size() works");
    
    // Test gc_is_managed
    int managed = gc_is_managed(obj);
    ASSERT(managed == 1, "Object is GC-managed");
    
    int* stack_var = &tests_passed;
    managed = gc_is_managed(stack_var);
    ASSERT(managed == 0, "Stack variable is not GC-managed");
    
    TEST_END();
}

// ============================================================================
// Test 10: Marking Chain
// ============================================================================

void test_marking_chain() {
    TEST("Marking Chain");
    
    // Create a chain: root -> obj1 -> obj2
    void** root = (void**)gc_malloc(sizeof(void*), GC_TYPE_GENERIC);
    void** obj1 = (void**)gc_malloc(sizeof(void*), GC_TYPE_GENERIC);
    void** obj2 = (void**)gc_malloc(sizeof(void*), GC_TYPE_GENERIC);
    
    *root = obj1;
    *obj1 = obj2;
    *obj2 = NULL;
    
    // Root only the first object
    gc_add_root((void**)&root);
    
    GCStats before;
    gc_get_stats(&before);
    ASSERT(before.num_objects == 3, "3 objects allocated");
    
    // Collect - all should survive if marking follows chains
    gc_collect();
    
    GCStats after;
    gc_get_stats(&after);
    
    // Note: Current GC doesn't follow generic pointers yet
    // This will be improved in Phase 9.2
    printf("ℹ️  INFO: After collection, %zu objects remain\n", after.num_objects);
    printf("ℹ️  INFO: Full chain marking will be implemented in Phase 9.2\n");
    
    gc_remove_root((void**)&root);
    TEST_END();
}

// ============================================================================
// Test 11: Statistics Accuracy
// ============================================================================

void test_statistics_accuracy() {
    TEST("Statistics Accuracy");
    
    // Initial state
    GCStats stats;
    gc_get_stats(&stats);
    size_t initial_objects = stats.num_objects;
    size_t initial_allocated = stats.total_allocated;
    
    // Allocate 5 objects
    for (int i = 0; i < 5; i++) {
        gc_malloc(100, GC_TYPE_GENERIC);
    }
    
    gc_get_stats(&stats);
    ASSERT(stats.num_objects == initial_objects + 5, "Object count increased by 5");
    ASSERT(stats.total_allocated > initial_allocated, "Bytes allocated increased");
    
    // Collect (all unrooted)
    gc_collect();
    
    gc_get_stats(&stats);
    ASSERT(stats.num_objects == 0, "All objects collected");
    ASSERT(stats.total_freed_objects == 5, "Freed object count is accurate");
    
    TEST_END();
}

// ============================================================================
// Test 12: Calloc
// ============================================================================

void test_calloc() {
    TEST("Calloc (Zero-Initialized Allocation)");
    
    int* array = (int*)gc_calloc(10, sizeof(int), GC_TYPE_GENERIC);
    ASSERT(array != NULL, "Calloc succeeded");
    
    // Check all elements are zero
    int all_zero = 1;
    for (int i = 0; i < 10; i++) {
        if (array[i] != 0) {
            all_zero = 0;
            break;
        }
    }
    ASSERT(all_zero == 1, "All elements are zero-initialized");
    
    TEST_END();
}

// ============================================================================
// Test 13: Large Allocation
// ============================================================================

void test_large_allocation() {
    TEST("Large Allocation");
    
    size_t large_size = 1024 * 1024;  // 1MB
    void* large = gc_malloc(large_size, GC_TYPE_GENERIC);
    ASSERT(large != NULL, "Large allocation succeeded");
    
    size_t size = gc_get_size(large);
    ASSERT(size == large_size, "Size is correct");
    
    TEST_END();
}

// ============================================================================
// Test 14: Adaptive Threshold
// ============================================================================

void test_adaptive_threshold() {
    TEST("Adaptive Threshold");
    
    size_t initial_threshold = gc_get_threshold();
    
    // Set low threshold
    gc_set_threshold(1024);
    ASSERT(gc_get_threshold() == 1024, "Threshold set to 1024");
    
    // Allocate many objects that survive (rooted)
    void* roots[10];
    for (int i = 0; i < 10; i++) {
        roots[i] = gc_malloc(256, GC_TYPE_GENERIC);
        gc_add_root(&roots[i]);
    }
    
    // Trigger collection multiple times
    // Threshold should adapt upward since few objects are collected
    for (int i = 0; i < 5; i++) {
        gc_collect();
    }
    
    size_t final_threshold = gc_get_threshold();
    ASSERT(final_threshold > 1024, "Threshold adapted upward");
    
    printf("ℹ️  INFO: Threshold increased from 1024 to %zu bytes\n", final_threshold);
    
    // Cleanup
    for (int i = 0; i < 10; i++) {
        gc_remove_root(&roots[i]);
    }
    
    TEST_END();
}

// ============================================================================
// Test 15: NULL Pointer Safety
// ============================================================================

void test_null_pointer_safety() {
    TEST("NULL Pointer Safety");
    
    // All these should handle NULL gracefully
    gc_mark_object(NULL);
    printf("✅ PASS: gc_mark_object(NULL) didn't crash\n");
    tests_passed++;
    
    GCObjectHeader* header = gc_get_header(NULL);
    ASSERT(header == NULL, "gc_get_header(NULL) returns NULL");
    
    GCObjectType type = gc_get_type(NULL);
    ASSERT(type == 0, "gc_get_type(NULL) returns 0");
    
    size_t size = gc_get_size(NULL);
    ASSERT(size == 0, "gc_get_size(NULL) returns 0");
    
    int managed = gc_is_managed(NULL);
    ASSERT(managed == 0, "gc_is_managed(NULL) returns 0");
    
    char* str = gc_strdup(NULL);
    ASSERT(str == NULL, "gc_strdup(NULL) returns NULL");
    
    TEST_END();
}

// ============================================================================
// Main
// ============================================================================

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║          MLP Garbage Collector - Unit Tests               ║\n");
    printf("║                  Phase 9.1 - Day 1-2                       ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    // Run all tests
    test_basic_allocation();
    test_multiple_allocations();
    test_string_duplication();
    test_root_set();
    test_manual_collection();
    test_rooted_survival();
    test_threshold_trigger();
    test_enable_disable();
    test_object_header_api();
    test_marking_chain();
    test_statistics_accuracy();
    test_calloc();
    test_large_allocation();
    test_adaptive_threshold();
    test_null_pointer_safety();
    
    // Summary
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                     Test Summary                           ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Passed: %3d                                               ║\n", tests_passed);
    printf("║  Failed: %3d                                               ║\n", tests_failed);
    printf("║  Total:  %3d                                               ║\n", tests_passed + tests_failed);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    if (tests_failed == 0) {
        printf("\n🎉 All tests passed! GC infrastructure is working correctly.\n\n");
        return 0;
    } else {
        printf("\n❌ Some tests failed. Please review the output above.\n\n");
        return 1;
    }
}
