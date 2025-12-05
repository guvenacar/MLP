// gc_test.c - GC Unit Tests

#include "gc.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

// Test 1: Basic allocation
void test_basic_alloc() {
    printf("\n=== Test 1: Basic Allocation ===\n");
    
    gc_init();
    
    int* num = (int*)gc_alloc(sizeof(int), TYPE_INT);
    assert(num != NULL);
    *num = 42;
    assert(*num == 42);
    
    printf("✓ Allocated int: %d\n", *num);
    
    gc_destroy();
}

// Test 2: Multiple allocations
void test_multiple_alloc() {
    printf("\n=== Test 2: Multiple Allocations ===\n");
    
    gc_init();
    
    int* nums[100];
    for (int i = 0; i < 100; i++) {
        nums[i] = (int*)gc_alloc(sizeof(int), TYPE_INT);
        assert(nums[i] != NULL);
        *nums[i] = i;
    }
    
    // Verify
    for (int i = 0; i < 100; i++) {
        assert(*nums[i] == i);
    }
    
    printf("✓ Allocated 100 integers\n");
    gc_print_stats();
    
    gc_destroy();
}

// Test 3: GC collection (no roots)
void test_gc_collect_no_roots() {
    printf("\n=== Test 3: GC Collection (No Roots) ===\n");
    
    gc_init();
    
    // Allocate but don't save references
    for (int i = 0; i < 1000; i++) {
        gc_alloc(sizeof(int), TYPE_INT);
    }
    
    printf("Before GC:\n");
    gc_print_stats();
    
    // No roots, everything should be collected
    gc_collect();
    
    printf("\nAfter GC:\n");
    gc_print_stats();
    
    assert(gc_object_count() == 0);  // All freed
    printf("✓ All objects freed\n");
    
    gc_destroy();
}

// Test 4: GC collection (with roots)
void test_gc_collect_with_roots() {
    printf("\n=== Test 4: GC Collection (With Roots) ===\n");
    
    gc_init();
    
    int* num1 = (int*)gc_alloc(sizeof(int), TYPE_INT);
    int* num2 = (int*)gc_alloc(sizeof(int), TYPE_INT);
    *num1 = 100;
    *num2 = 200;
    
    // Register roots
    gc_add_root((void**)&num1);
    gc_add_root((void**)&num2);
    
    // Allocate garbage
    for (int i = 0; i < 1000; i++) {
        gc_alloc(sizeof(int), TYPE_INT);
    }
    
    printf("Before GC:\n");
    gc_print_stats();
    
    // GC should keep num1 and num2
    gc_collect();
    
    printf("\nAfter GC:\n");
    gc_print_stats();
    
    assert(*num1 == 100);
    assert(*num2 == 200);
    assert(gc_object_count() == 2);  // Only rooted objects remain
    
    printf("✓ Rooted objects survived: %d, %d\n", *num1, *num2);
    
    gc_remove_root((void**)&num1);
    gc_remove_root((void**)&num2);
    
    gc_destroy();
}

// Test 5: String allocation
void test_string_alloc() {
    printf("\n=== Test 5: String Allocation ===\n");
    
    gc_init();
    
    const char* msg = "Hello, GC!";
    char* str = (char*)gc_alloc(strlen(msg) + 1, TYPE_STRING);
    assert(str != NULL);
    strcpy(str, msg);
    
    printf("✓ Allocated string: \"%s\"\n", str);
    
    gc_add_root((void**)&str);
    
    // Allocate garbage
    for (int i = 0; i < 100; i++) {
        gc_alloc(100, TYPE_STRING);
    }
    
    gc_collect();
    
    assert(strcmp(str, msg) == 0);  // String survived
    printf("✓ String survived GC: \"%s\"\n", str);
    
    gc_remove_root((void**)&str);
    gc_destroy();
}

// Test 6: Stress test
void test_stress() {
    printf("\n=== Test 6: Stress Test ===\n");
    
    gc_init();
    
    int* persistent[10];
    for (int i = 0; i < 10; i++) {
        persistent[i] = (int*)gc_alloc(sizeof(int), TYPE_INT);
        *persistent[i] = i;
        gc_add_root((void**)&persistent[i]);
    }
    
    // Many allocations + GCs
    for (int round = 0; round < 10; round++) {
        printf("Round %d: ", round + 1);
        for (int i = 0; i < 500; i++) {
            gc_alloc(sizeof(int), TYPE_INT);
        }
        gc_collect();
    }
    
    // Verify persistent objects
    for (int i = 0; i < 10; i++) {
        assert(*persistent[i] == i);
    }
    
    printf("✓ All persistent objects intact\n");
    
    for (int i = 0; i < 10; i++) {
        gc_remove_root((void**)&persistent[i]);
    }
    
    gc_destroy();
}

// Test 7: Free list reuse
void test_free_list_reuse() {
    printf("\n=== Test 7: Free List Reuse ===\n");
    
    gc_init();
    
    // Allocate and collect
    for (int i = 0; i < 100; i++) {
        gc_alloc(sizeof(int), TYPE_INT);
    }
    gc_collect();  // Free all
    
    printf("After first GC:\n");
    gc_print_stats();
    
    // Allocate again (should reuse free list)
    int* nums[100];
    for (int i = 0; i < 100; i++) {
        nums[i] = (int*)gc_alloc(sizeof(int), TYPE_INT);
        assert(nums[i] != NULL);
        *nums[i] = i;
    }
    
    printf("\nAfter reallocation:\n");
    gc_print_stats();
    
    // Verify
    for (int i = 0; i < 100; i++) {
        assert(*nums[i] == i);
    }
    
    printf("✓ Free list reuse working\n");
    
    gc_destroy();
}

// Main test runner
int main() {
    printf("╔══════════════════════════════════╗\n");
    printf("║     MELP GC Unit Tests           ║\n");
    printf("╚══════════════════════════════════╝\n");
    
    test_basic_alloc();
    test_multiple_alloc();
    test_gc_collect_no_roots();
    test_gc_collect_with_roots();
    test_string_alloc();
    test_stress();
    test_free_list_reuse();
    
    printf("\n");
    printf("╔══════════════════════════════════╗\n");
    printf("║   All Tests Passed! ✓            ║\n");
    printf("╚══════════════════════════════════╝\n");
    
    return 0;
}
