/**
 * Phase 5.2 C Unit Tests - Memory Management
 * Tests: mlp_malloc, mlp_free, mlp_realloc, mlp_calloc,
 *        get_allocated_bytes, check_memory_leaks
 * 
 * NOTE: This test file is written BEFORE implementation exists.
 *       It will be compiled and run once Claude #1 completes runtime implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations from runtime.c (will be implemented by Claude #1)
extern void* mlp_malloc(long size);
extern void mlp_free(void* ptr);
extern void* mlp_realloc(void* ptr, long size);
extern void* mlp_calloc(long count, long size);
extern long get_allocated_bytes(void);
extern long check_memory_leaks(void);

// Test counter
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    printf("Testing %s... ", name); \
    if (test_##name()) { \
        printf("PASS\n"); \
        tests_passed++; \
    } else { \
        printf("FAIL\n"); \
        tests_failed++; \
    }

// ==================== Malloc Tests ====================

int test_malloc_basic() {
    void* ptr = mlp_malloc(100);
    if (ptr == NULL) return 0;

    long allocated = get_allocated_bytes();
    if (allocated < 100) {
        mlp_free(ptr);
        return 0;
    }

    mlp_free(ptr);
    return 1;
}

int test_malloc_zero() {
    void* ptr = mlp_malloc(0);
    // malloc(0) behavior: should return NULL or valid pointer
    // We expect NULL for zero size
    if (ptr != NULL) {
        mlp_free(ptr);
        return 0;
    }
    return 1;
}

int test_malloc_negative() {
    void* ptr = mlp_malloc(-10);
    // Negative size should return NULL
    if (ptr != NULL) {
        mlp_free(ptr);
        return 0;
    }
    return 1;
}

int test_malloc_large() {
    void* ptr = mlp_malloc(1024 * 1024);  // 1MB
    if (ptr == NULL) return 0;

    long allocated = get_allocated_bytes();
    if (allocated < 1024 * 1024) {
        mlp_free(ptr);
        return 0;
    }

    mlp_free(ptr);
    return 1;
}

int test_malloc_very_large() {
    // Test 10MB allocation
    void* ptr = mlp_malloc(10 * 1024 * 1024);
    if (ptr == NULL) return 0;

    long allocated = get_allocated_bytes();
    if (allocated < 10 * 1024 * 1024) {
        mlp_free(ptr);
        return 0;
    }

    mlp_free(ptr);
    return 1;
}

int test_malloc_small() {
    // Test very small allocations
    void* ptr1 = mlp_malloc(1);
    void* ptr2 = mlp_malloc(8);
    void* ptr3 = mlp_malloc(16);

    if (!ptr1 || !ptr2 || !ptr3) {
        mlp_free(ptr1);
        mlp_free(ptr2);
        mlp_free(ptr3);
        return 0;
    }

    mlp_free(ptr1);
    mlp_free(ptr2);
    mlp_free(ptr3);
    return 1;
}

// ==================== Free Tests ====================

int test_free_basic() {
    void* ptr = mlp_malloc(100);
    if (ptr == NULL) return 0;

    long before = get_allocated_bytes();
    mlp_free(ptr);
    long after = get_allocated_bytes();

    return after < before;
}

int test_free_null() {
    // Should not crash
    mlp_free(NULL);
    return 1;
}

int test_free_multiple() {
    void* ptr1 = mlp_malloc(100);
    void* ptr2 = mlp_malloc(200);
    void* ptr3 = mlp_malloc(300);

    if (!ptr1 || !ptr2 || !ptr3) {
        mlp_free(ptr1);
        mlp_free(ptr2);
        mlp_free(ptr3);
        return 0;
    }

    long before = get_allocated_bytes();
    mlp_free(ptr1);
    mlp_free(ptr2);
    mlp_free(ptr3);
    long after = get_allocated_bytes();

    return after < before;
}

int test_free_in_reverse_order() {
    void* ptr1 = mlp_malloc(100);
    void* ptr2 = mlp_malloc(200);
    void* ptr3 = mlp_malloc(300);

    if (!ptr1 || !ptr2 || !ptr3) {
        mlp_free(ptr1);
        mlp_free(ptr2);
        mlp_free(ptr3);
        return 0;
    }

    // Free in reverse order
    mlp_free(ptr3);
    mlp_free(ptr2);
    mlp_free(ptr1);

    long allocated = get_allocated_bytes();
    // All should be freed
    return 1;  // Just verify no crash
}

// ==================== Calloc Tests ====================

int test_calloc_basic() {
    void* ptr = mlp_calloc(10, 10);
    if (ptr == NULL) return 0;

    // Check that memory is zeroed
    unsigned char* bytes = (unsigned char*)ptr;
    for (int i = 0; i < 100; i++) {
        if (bytes[i] != 0) {
            mlp_free(ptr);
            return 0;
        }
    }

    mlp_free(ptr);
    return 1;
}

int test_calloc_zero_count() {
    void* ptr = mlp_calloc(0, 10);
    // calloc(0, x) should return NULL
    if (ptr != NULL) {
        mlp_free(ptr);
        return 0;
    }
    return 1;
}

int test_calloc_zero_size() {
    void* ptr = mlp_calloc(10, 0);
    // calloc(x, 0) should return NULL
    if (ptr != NULL) {
        mlp_free(ptr);
        return 0;
    }
    return 1;
}

int test_calloc_large() {
    // Allocate 1000 elements of 1024 bytes each
    void* ptr = mlp_calloc(1000, 1024);
    if (ptr == NULL) return 0;

    // Verify some bytes are zero
    unsigned char* bytes = (unsigned char*)ptr;
    int all_zero = 1;
    for (int i = 0; i < 1000; i += 100) {
        if (bytes[i] != 0) {
            all_zero = 0;
            break;
        }
    }

    mlp_free(ptr);
    return all_zero;
}

int test_calloc_tracking() {
    long before = get_allocated_bytes();
    
    void* ptr = mlp_calloc(100, 10);
    if (ptr == NULL) return 0;

    long during = get_allocated_bytes();
    if (during <= before) {
        mlp_free(ptr);
        return 0;
    }

    mlp_free(ptr);
    long after = get_allocated_bytes();

    return after < during;
}

// ==================== Realloc Tests ====================

int test_realloc_grow() {
    void* ptr = mlp_malloc(100);
    if (ptr == NULL) return 0;

    void* new_ptr = mlp_realloc(ptr, 200);
    if (new_ptr == NULL) {
        mlp_free(ptr);
        return 0;
    }

    long allocated = get_allocated_bytes();
    if (allocated < 200) {
        mlp_free(new_ptr);
        return 0;
    }

    mlp_free(new_ptr);
    return 1;
}

int test_realloc_shrink() {
    void* ptr = mlp_malloc(200);
    if (ptr == NULL) return 0;

    void* new_ptr = mlp_realloc(ptr, 100);
    if (new_ptr == NULL) {
        mlp_free(ptr);
        return 0;
    }

    mlp_free(new_ptr);
    return 1;
}

int test_realloc_null() {
    // realloc(NULL, size) should behave like malloc(size)
    void* ptr = mlp_realloc(NULL, 100);
    if (ptr == NULL) return 0;

    long allocated = get_allocated_bytes();
    if (allocated < 100) {
        mlp_free(ptr);
        return 0;
    }

    mlp_free(ptr);
    return 1;
}

int test_realloc_zero_size() {
    void* ptr = mlp_malloc(100);
    if (ptr == NULL) return 0;

    // realloc(ptr, 0) should behave like free(ptr)
    void* new_ptr = mlp_realloc(ptr, 0);
    
    // After realloc to 0, pointer should be NULL (freed)
    return new_ptr == NULL;
}

int test_realloc_same_size() {
    void* ptr = mlp_malloc(100);
    if (ptr == NULL) return 0;

    void* new_ptr = mlp_realloc(ptr, 100);
    if (new_ptr == NULL) {
        mlp_free(ptr);
        return 0;
    }

    mlp_free(new_ptr);
    return 1;
}

int test_realloc_multiple_times() {
    void* ptr = mlp_malloc(50);
    if (ptr == NULL) return 0;

    // Grow multiple times
    ptr = mlp_realloc(ptr, 100);
    if (ptr == NULL) return 0;

    ptr = mlp_realloc(ptr, 200);
    if (ptr == NULL) return 0;

    ptr = mlp_realloc(ptr, 400);
    if (ptr == NULL) return 0;

    // Shrink multiple times
    ptr = mlp_realloc(ptr, 200);
    if (ptr == NULL) return 0;

    ptr = mlp_realloc(ptr, 100);
    if (ptr == NULL) return 0;

    mlp_free(ptr);
    return 1;
}

// ==================== Allocation Tracking Tests ====================

int test_get_allocated_bytes_initial() {
    // At start of test, should have some baseline
    long allocated = get_allocated_bytes();
    // Just verify it doesn't crash and returns a number
    return allocated >= 0;
}

int test_get_allocated_bytes_after_malloc() {
    long before = get_allocated_bytes();

    void* ptr = mlp_malloc(256);
    if (ptr == NULL) return 0;

    long after = get_allocated_bytes();
    mlp_free(ptr);

    return after > before;
}

int test_get_allocated_bytes_after_free() {
    void* ptr = mlp_malloc(256);
    if (ptr == NULL) return 0;

    long with_alloc = get_allocated_bytes();
    mlp_free(ptr);
    long after_free = get_allocated_bytes();

    return after_free < with_alloc;
}

int test_get_allocated_bytes_multiple() {
    long before = get_allocated_bytes();

    void* ptr1 = mlp_malloc(100);
    void* ptr2 = mlp_malloc(200);
    void* ptr3 = mlp_malloc(300);

    if (!ptr1 || !ptr2 || !ptr3) {
        mlp_free(ptr1);
        mlp_free(ptr2);
        mlp_free(ptr3);
        return 0;
    }

    long during = get_allocated_bytes();
    if (during <= before) {
        mlp_free(ptr1);
        mlp_free(ptr2);
        mlp_free(ptr3);
        return 0;
    }

    mlp_free(ptr1);
    long after_one = get_allocated_bytes();
    
    mlp_free(ptr2);
    long after_two = get_allocated_bytes();
    
    mlp_free(ptr3);
    long after_three = get_allocated_bytes();

    // Each free should reduce allocated bytes
    return (after_one < during) && 
           (after_two < after_one) && 
           (after_three < after_two);
}

int test_tracking_accuracy() {
    long before = get_allocated_bytes();

    void* ptr = mlp_malloc(1024);
    if (ptr == NULL) return 0;

    long after = get_allocated_bytes();
    long diff = after - before;

    mlp_free(ptr);

    // Difference should be at least the requested size
    // (may be more due to overhead)
    return diff >= 1024;
}

// ==================== Memory Leak Tests ====================

int test_check_memory_leaks_none() {
    // Allocate and free cleanly
    void* ptr1 = mlp_malloc(100);
    void* ptr2 = mlp_malloc(200);
    
    if (!ptr1 || !ptr2) {
        mlp_free(ptr1);
        mlp_free(ptr2);
        return 0;
    }

    mlp_free(ptr1);
    mlp_free(ptr2);

    // Redirect stderr to avoid output
    FILE* old_stderr = stderr;
    stderr = fopen("/dev/null", "w");

    long leaks = check_memory_leaks();

    fclose(stderr);
    stderr = old_stderr;

    // Should report 0 leaks (or low number due to test framework)
    return leaks == 0;
}

int test_check_memory_leaks_one() {
    void* ptr = mlp_malloc(100);
    if (ptr == NULL) return 0;

    // Intentionally don't free

    // Redirect stderr
    FILE* old_stderr = stderr;
    stderr = fopen("/dev/null", "w");

    long leaks = check_memory_leaks();

    fclose(stderr);
    stderr = old_stderr;

    // Clean up
    mlp_free(ptr);

    // Should report at least 1 leak
    return leaks >= 1;
}

int test_check_memory_leaks_multiple() {
    void* ptr1 = mlp_malloc(100);
    void* ptr2 = mlp_malloc(200);
    void* ptr3 = mlp_malloc(300);

    if (!ptr1 || !ptr2 || !ptr3) {
        mlp_free(ptr1);
        mlp_free(ptr2);
        mlp_free(ptr3);
        return 0;
    }

    // Intentionally don't free

    // Redirect stderr
    FILE* old_stderr = stderr;
    stderr = fopen("/dev/null", "w");

    long leaks = check_memory_leaks();

    fclose(stderr);
    stderr = old_stderr;

    // Clean up
    mlp_free(ptr1);
    mlp_free(ptr2);
    mlp_free(ptr3);

    // Should report at least 3 leaks
    return leaks >= 3;
}

int test_check_memory_leaks_partial() {
    void* ptr1 = mlp_malloc(100);
    void* ptr2 = mlp_malloc(200);
    void* ptr3 = mlp_malloc(300);

    if (!ptr1 || !ptr2 || !ptr3) {
        mlp_free(ptr1);
        mlp_free(ptr2);
        mlp_free(ptr3);
        return 0;
    }

    // Free only ptr2
    mlp_free(ptr2);

    // Redirect stderr
    FILE* old_stderr = stderr;
    stderr = fopen("/dev/null", "w");

    long leaks = check_memory_leaks();

    fclose(stderr);
    stderr = old_stderr;

    // Clean up
    mlp_free(ptr1);
    mlp_free(ptr3);

    // Should report 2 leaks
    return leaks >= 2;
}

// ==================== Stress Tests ====================

int test_many_small_allocations() {
    int success = 1;
    
    // Allocate 1000 small blocks
    void* ptrs[1000];
    for (int i = 0; i < 1000; i++) {
        ptrs[i] = mlp_malloc(16);
        if (ptrs[i] == NULL) {
            success = 0;
            break;
        }
    }

    // Free all
    for (int i = 0; i < 1000; i++) {
        mlp_free(ptrs[i]);
    }

    return success;
}

int test_alternating_alloc_free() {
    // Allocate and free in alternating pattern
    for (int i = 0; i < 100; i++) {
        void* ptr = mlp_malloc(100);
        if (ptr == NULL) return 0;
        mlp_free(ptr);
    }
    return 1;
}

// ==================== Main ====================

int main() {
    printf("==================================\n");
    printf("Phase 5.2 C Unit Tests - Memory Management\n");
    printf("==================================\n\n");

    printf("--- Malloc Tests ---\n");
    TEST(malloc_basic);
    TEST(malloc_zero);
    TEST(malloc_negative);
    TEST(malloc_large);
    TEST(malloc_very_large);
    TEST(malloc_small);
    printf("\n");

    printf("--- Free Tests ---\n");
    TEST(free_basic);
    TEST(free_null);
    TEST(free_multiple);
    TEST(free_in_reverse_order);
    printf("\n");

    printf("--- Calloc Tests ---\n");
    TEST(calloc_basic);
    TEST(calloc_zero_count);
    TEST(calloc_zero_size);
    TEST(calloc_large);
    TEST(calloc_tracking);
    printf("\n");

    printf("--- Realloc Tests ---\n");
    TEST(realloc_grow);
    TEST(realloc_shrink);
    TEST(realloc_null);
    TEST(realloc_zero_size);
    TEST(realloc_same_size);
    TEST(realloc_multiple_times);
    printf("\n");

    printf("--- Tracking Tests ---\n");
    TEST(get_allocated_bytes_initial);
    TEST(get_allocated_bytes_after_malloc);
    TEST(get_allocated_bytes_after_free);
    TEST(get_allocated_bytes_multiple);
    TEST(tracking_accuracy);
    printf("\n");

    printf("--- Leak Detection Tests ---\n");
    TEST(check_memory_leaks_none);
    TEST(check_memory_leaks_one);
    TEST(check_memory_leaks_multiple);
    TEST(check_memory_leaks_partial);
    printf("\n");

    printf("--- Stress Tests ---\n");
    TEST(many_small_allocations);
    TEST(alternating_alloc_free);
    printf("\n");

    printf("==================================\n");
    printf("Results: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("==================================\n");

    return tests_failed > 0 ? 1 : 0;
}
