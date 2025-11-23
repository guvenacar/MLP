/**
 * test_gc_promise.c - Test GC Integration with Promise System
 * 
 * Phase 9.2: Promise System Integration
 * Date: 23 Kasım 2025
 */

#include <stdio.h>
#include <assert.h>
#include "gc.h"

// Forward declarations from simple_runtime.c
typedef enum {
    PROMISE_PENDING = 0,
    PROMISE_RESOLVED = 1,
    PROMISE_REJECTED = 2
} PromiseState;

typedef struct Promise {
    PromiseState state;
    void* value;
    char* error;
    void (**callbacks)(void*);
    int callback_count;
    int callback_capacity;
    void* user_data;
} Promise;

extern Promise* promise_create();
extern void promise_resolve(Promise* p, void* value);
extern void promise_reject(Promise* p, const char* error);
extern int promise_is_resolved(Promise* p);
extern int promise_is_rejected(Promise* p);
extern void* promise_get_value(Promise* p);
extern char* promise_get_error(Promise* p);

// Test counter
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    printf("\n--- Test: %s ---\n", name)

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

// ============================================================================
// Test 1: Promise Creation with GC
// ============================================================================

void test_promise_creation_gc() {
    TEST("Promise Creation with GC");
    
    Promise* p = promise_create();
    ASSERT(p != NULL, "Promise created successfully");
    
    // Check that promise is GC-managed
    int managed = gc_is_managed(p);
    ASSERT(managed == 1, "Promise is GC-managed");
    
    // Check type
    GCObjectType type = gc_get_type(p);
    ASSERT(type == GC_TYPE_PROMISE, "Promise has correct GC type");
}

// ============================================================================
// Test 2: Promise Resolution with GC
// ============================================================================

void test_promise_resolution_gc() {
    TEST("Promise Resolution with GC");
    
    Promise* p = promise_create();
    ASSERT(p != NULL, "Promise created");
    
    // Register as root so it survives collection
    gc_add_root((void**)&p);
    
    // Resolve promise
    int value = 42;
    promise_resolve(p, &value);
    
    ASSERT(promise_is_resolved(p) == 1, "Promise is resolved");
    
    int* result = (int*)promise_get_value(p);
    ASSERT(*result == 42, "Promise value is correct");
    
    gc_remove_root((void**)&p);
}

// ============================================================================
// Test 3: Promise Rejection with GC Strings
// ============================================================================

void test_promise_rejection_gc() {
    TEST("Promise Rejection with GC Strings");
    
    Promise* p = promise_create();
    ASSERT(p != NULL, "Promise created");
    
    gc_add_root((void**)&p);
    
    // Reject promise with error message
    promise_reject(p, "Test error");
    
    ASSERT(promise_is_rejected(p) == 1, "Promise is rejected");
    
    char* error = promise_get_error(p);
    ASSERT(error != NULL, "Error message exists");
    
    // Check that error string is GC-managed
    int managed = gc_is_managed(error);
    ASSERT(managed == 1, "Error string is GC-managed");
    
    gc_remove_root((void**)&p);
}

// ============================================================================
// Test 4: Multiple Promises - No Memory Leaks
// ============================================================================

void test_multiple_promises_no_leaks() {
    TEST("Multiple Promises - No Memory Leaks");
    
    // Create 100 promises (not rooted - should be collected)
    for (int i = 0; i < 100; i++) {
        Promise* p = promise_create();
        promise_resolve(p, (void*)(long)i);
    }
    
    GCStats after_alloc;
    gc_get_stats(&after_alloc);
    size_t before_objects = after_alloc.num_objects;
    ASSERT(before_objects >= 100, "100+ objects allocated");
    
    // Trigger GC - all promises should be collected (none are rooted)
    gc_collect();
    
    GCStats after_gc;
    gc_get_stats(&after_gc);
    
    // All unreferenced objects should be freed
    size_t freed = before_objects - after_gc.num_objects;
    ASSERT(freed >= 100, "At least 100 promises collected");
    
    printf("ℹ️  INFO: Freed %zu objects, %zu remain\n", 
           freed, after_gc.num_objects);
}

// ============================================================================
// Test 5: Rooted Promises Survive Collection
// ============================================================================

void test_rooted_promises_survive() {
    TEST("Rooted Promises Survive Collection");
    
    Promise* p1 = promise_create();
    Promise* p2 = promise_create();
    Promise* p3 = promise_create();
    
    // Root only p1 and p2
    gc_add_root((void**)&p1);
    gc_add_root((void**)&p2);
    
    promise_resolve(p1, (void*)1);
    promise_resolve(p2, (void*)2);
    promise_resolve(p3, (void*)3);  // Not rooted - will be collected
    
    GCStats before;
    gc_get_stats(&before);
    
    // Trigger GC
    gc_collect();
    
    GCStats after;
    gc_get_stats(&after);
    
    // p1 and p2 should survive, p3 should be collected
    ASSERT(promise_is_resolved(p1) == 1, "Rooted promise p1 survived");
    ASSERT(promise_is_resolved(p2) == 1, "Rooted promise p2 survived");
    
    printf("ℹ️  INFO: %zu objects survived collection\n", after.num_objects);
    
    gc_remove_root((void**)&p1);
    gc_remove_root((void**)&p2);
}

// ============================================================================
// Test 6: GC Statistics After Promise Operations
// ============================================================================

void test_gc_statistics() {
    TEST("GC Statistics After Promise Operations");
    
    GCStats stats;
    gc_get_stats(&stats);
    
    size_t initial_collections = stats.num_collections;
    size_t initial_objects = stats.num_objects;
    
    // Set low threshold to force GC
    gc_set_threshold(512);
    
    // Create many promises to trigger GC
    for (int i = 0; i < 100; i++) {
        Promise* p = promise_create();
        promise_resolve(p, (void*)(long)i);
    }
    
    gc_get_stats(&stats);
    
    ASSERT(stats.num_collections > initial_collections, 
           "GC was triggered by promise allocations");
    
    printf("ℹ️  INFO: %zu GC collections performed\n", 
           stats.num_collections - initial_collections);
    printf("ℹ️  INFO: Objects: %zu -> %zu\n", 
           initial_objects, stats.num_objects);
    
    gc_print_stats();
}

// ============================================================================
// Main
// ============================================================================

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║      GC Integration with Promise System - Tests           ║\n");
    printf("║                Phase 9.2 - Day 3-4                         ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    // Initialize GC
    gc_init();
    printf("\n[GC] Initialized for Promise testing\n");
    
    // Run tests
    test_promise_creation_gc();
    test_promise_resolution_gc();
    test_promise_rejection_gc();
    test_multiple_promises_no_leaks();
    test_rooted_promises_survive();
    test_gc_statistics();
    
    // Summary
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                     Test Summary                           ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║  Passed: %3d                                               ║\n", tests_passed);
    printf("║  Failed: %3d                                               ║\n", tests_failed);
    printf("║  Total:  %3d                                               ║\n", tests_passed + tests_failed);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    // Shutdown GC
    gc_shutdown();
    
    if (tests_failed == 0) {
        printf("\n🎉 All tests passed! GC + Promise integration working.\n\n");
        return 0;
    } else {
        printf("\n❌ Some tests failed. Please review the output above.\n\n");
        return 1;
    }
}
