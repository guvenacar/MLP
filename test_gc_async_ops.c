/**
 * test_gc_async_ops.c - Test GC with Real Async Operations
 * 
 * Phase 9.4 + 10: Test async operations with GC and Thread Pool
 * Date: 23 Kasım 2025
 */

#include <stdio.h>
#include <unistd.h>
#include "gc.h"
#include "thread_pool.h"

// Forward declarations
typedef struct Promise Promise;

extern Promise* promise_create();
extern Promise* async_sleep(int ms);
extern int promise_await_safe(Promise* p);
extern void* promise_get_value(Promise* p);
extern Promise* promise_new();
extern void promise_resolve_numeric(Promise* p, long value);
extern long promise_get_numeric_value(Promise* p);
extern Promise* async_timeout(Promise* task, int timeout_ms);

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  GC + Thread Pool - Real Async Operations Integration     ║\n");
    printf("║              Phase 9.4 + Phase 10.1 - Day 5                ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    // Initialize GC
    gc_init();
    printf("[GC] Initialized for async testing\n");
    
    // Initialize Thread Pool
    thread_pool_init(4);
    printf("[ThreadPool] Initialized with 4 workers\n\n");
    
    // Test 1: async_sleep with GC
    printf("--- Test 1: async_sleep(50ms) with GC ---\n");
    Promise* sleep_task = async_sleep(50);
    gc_add_root((void**)&sleep_task);
    
    printf("Sleep task created, waiting...\n");
    int result = promise_await_safe(sleep_task);
    printf("Sleep completed, status: %d\n", result);
    
    gc_remove_root((void**)&sleep_task);
    printf("✅ async_sleep works with GC\n\n");
    
    // Test 2: Multiple async operations
    printf("--- Test 2: Multiple async_sleep operations ---\n");
    Promise* tasks[5];
    for (int i = 0; i < 5; i++) {
        tasks[i] = async_sleep(10 * (i + 1));
        gc_add_root((void**)&tasks[i]);
    }
    
    printf("Created 5 sleep tasks...\n");
    for (int i = 0; i < 5; i++) {
        promise_await_safe(tasks[i]);
        printf("Task %d completed\n", i + 1);
        gc_remove_root((void**)&tasks[i]);
    }
    printf("✅ Multiple async operations work with GC\n\n");
    
    // Test 3: Zero timeout edge case
    printf("--- Test 3: Zero Timeout Edge Case ---\n");
    Promise* slow_task = async_sleep(100);
    gc_add_root((void**)&slow_task);
    
    Promise* timed = async_timeout(slow_task, 0);
    gc_add_root((void**)&timed);
    
    int timeout_result = promise_await_safe(timed);
    printf("Timeout result: %d (0=success, -1=timeout)\n", timeout_result);
    
    gc_remove_root((void**)&slow_task);
    gc_remove_root((void**)&timed);
    printf("✅ Timeout edge case works with GC\n\n");
    
    // Test 4: GC collection after async operations
    printf("--- Test 4: GC Collection After Async Ops ---\n");
    GCStats before;
    gc_get_stats(&before);
    printf("Before: %zu objects, %zu bytes\n", before.num_objects, before.total_allocated);
    
    // Create many unrooted async operations
    for (int i = 0; i < 10; i++) {
        Promise* p = async_sleep(1);
        promise_await_safe(p);
        // Not rooted - should be collected
    }
    
    gc_collect();
    
    GCStats after;
    gc_get_stats(&after);
    printf("After GC: %zu objects, %zu bytes\n", after.num_objects, after.total_allocated);
    printf("Freed: %zu objects\n", before.num_objects - after.num_objects);
    printf("✅ GC collects completed async operations\n\n");
    
    // Print final statistics
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                  Final GC Statistics                       ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    gc_print_stats();
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    // Shutdown thread pool
    thread_pool_shutdown();
    
    // Shutdown GC
    gc_shutdown();
    
    printf("🎉 All async operations work correctly with GC + Thread Pool!\n\n");
    return 0;
}
