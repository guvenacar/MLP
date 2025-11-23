/**
 * Thread Pool Unit Tests
 * 
 * Tests thread pool functionality and performance
 */

#include "thread_pool.h"
#include "gc.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

// Test counter
static int completed_tasks = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Simple worker function for testing
 */
void test_worker(void* args) {
    int* value = (int*)args;
    usleep(10000);  // Simulate 10ms work
    
    pthread_mutex_lock(&counter_mutex);
    completed_tasks++;
    pthread_mutex_unlock(&counter_mutex);
    
    printf("  Task %d completed\n", *value);
}

/**
 * Fast worker function for performance testing (no sleep)
 */
void fast_worker(void* args) {
    int* value = (int*)args;
    // No sleep - just increment counter
    pthread_mutex_lock(&counter_mutex);
    completed_tasks++;
    pthread_mutex_unlock(&counter_mutex);
}

/**
 * Get current time in microseconds
 */
long long get_time_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000LL + tv.tv_usec;
}

/**
 * Test 1: Basic thread pool initialization
 */
void test_init(void) {
    printf("\n=== Test 1: Thread Pool Initialization ===\n");
    
    int result = thread_pool_init(4);
    if (result == 0) {
        printf("✅ Thread pool initialized successfully\n");
    } else {
        printf("❌ Thread pool initialization failed\n");
    }
}

/**
 * Test 2: Submit and execute single task
 */
void test_single_task(void) {
    printf("\n=== Test 2: Single Task Execution ===\n");
    
    completed_tasks = 0;
    int* value = (int*)gc_malloc(sizeof(int), GC_TYPE_GENERIC);
    *value = 1;
    
    thread_pool_submit(test_worker, value, NULL);
    thread_pool_wait_idle();
    
    if (completed_tasks == 1) {
        printf("✅ Single task executed successfully\n");
    } else {
        printf("❌ Single task failed (completed: %d)\n", completed_tasks);
    }
}

/**
 * Test 3: Submit multiple tasks
 */
void test_multiple_tasks(void) {
    printf("\n=== Test 3: Multiple Tasks (10 tasks) ===\n");
    
    completed_tasks = 0;
    const int NUM_TASKS = 10;
    
    for (int i = 0; i < NUM_TASKS; i++) {
        int* value = (int*)gc_malloc(sizeof(int), GC_TYPE_GENERIC);
        *value = i + 1;
        thread_pool_submit(test_worker, value, NULL);
    }
    
    thread_pool_wait_idle();
    
    if (completed_tasks == NUM_TASKS) {
        printf("✅ All %d tasks executed successfully\n", NUM_TASKS);
    } else {
        printf("❌ Some tasks failed (completed: %d/%d)\n", completed_tasks, NUM_TASKS);
    }
}

/**
 * Test 4: Queue size monitoring
 */
void test_queue_size(void) {
    printf("\n=== Test 4: Queue Size Monitoring ===\n");
    
    completed_tasks = 0;
    
    // Submit tasks faster than they complete
    for (int i = 0; i < 20; i++) {
        int* value = (int*)gc_malloc(sizeof(int), GC_TYPE_GENERIC);
        *value = i + 1;
        thread_pool_submit(test_worker, value, NULL);
    }
    
    usleep(5000);  // Let some tasks start
    size_t queue_size = thread_pool_queue_size();
    int active = thread_pool_active_tasks();
    
    printf("  Queue size: %zu\n", queue_size);
    printf("  Active tasks: %d\n", active);
    
    thread_pool_wait_idle();
    
    if (completed_tasks == 20) {
        printf("✅ Queue monitoring works correctly\n");
    } else {
        printf("❌ Queue monitoring test failed\n");
    }
}

/**
 * Test 5: Performance benchmark (thread creation vs thread pool)
 */
void test_performance_benchmark(void) {
    printf("\n=== Test 5: Performance Benchmark ===\n");
    
    const int NUM_ITERATIONS = 1000;
    
    // Benchmark: Thread Pool (measure submission overhead only)
    completed_tasks = 0;
    long long start_pool = get_time_us();
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        int* value = (int*)gc_malloc(sizeof(int), GC_TYPE_GENERIC);
        *value = i;
        thread_pool_submit(fast_worker, value, NULL);
    }
    
    long long end_pool = get_time_us();
    long long submission_time = end_pool - start_pool;
    double avg_submission = (double)submission_time / NUM_ITERATIONS;
    
    // Wait for completion
    thread_pool_wait_idle();
    
    printf("\n  Thread Pool Submission Performance:\n");
    printf("  - Total submission time: %lld μs\n", submission_time);
    printf("  - Avg submission time: %.2f μs\n", avg_submission);
    printf("  - Submissions/sec: %.0f\n", 1000000.0 / avg_submission);
    printf("  - Tasks completed: %d/%d\n", completed_tasks, NUM_ITERATIONS);
    
    if (avg_submission < 10) {
        printf("✅ Thread pool submission excellent (%.2f μs < 10 μs target)\n", avg_submission);
    } else {
        printf("⚠️  Thread pool overhead: %.2f μs (target: <10μs)\n", avg_submission);
    }
}

/**
 * Test 6: Graceful shutdown
 */
void test_shutdown(void) {
    printf("\n=== Test 6: Graceful Shutdown ===\n");
    
    thread_pool_shutdown();
    
    if (global_thread_pool == NULL) {
        printf("✅ Thread pool shutdown successfully\n");
    } else {
        printf("❌ Thread pool shutdown failed\n");
    }
}

/**
 * Main test runner
 */
int main(void) {
    printf("╔════════════════════════════════════════╗\n");
    printf("║   Thread Pool Unit Tests              ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    // Initialize GC
    gc_init();
    
    // Run tests
    test_init();
    test_single_task();
    test_multiple_tasks();
    test_queue_size();
    test_performance_benchmark();
    test_shutdown();
    
    // Print GC statistics
    printf("\n=== GC Statistics ===\n");
    gc_print_stats();
    
    printf("\n🎉 All thread pool tests completed!\n");
    return 0;
}
