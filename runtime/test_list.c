/**
 * MLP Dynamic List - Comprehensive Test Suite
 * Tests all list operations with edge cases and stress tests
 * 
 * Compile: gcc -o test_list test_list.c runtime.c -I.
 * Run: ./test_list
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Forward declarations from runtime.c
typedef struct {
    void** data;
    int size;
    int capacity;
    size_t element_size;
    int is_pointer_type;
} DynamicList;

DynamicList* list_create(size_t element_size, int is_pointer_type);
void list_add(DynamicList* list, void* element);
void* list_get(DynamicList* list, int index);
int list_size(DynamicList* list);
void list_remove(DynamicList* list, int index);
void list_clear(DynamicList* list);
void list_destroy(DynamicList* list);

// Test result tracking
int tests_passed = 0;
int tests_failed = 0;

#define TEST_START(name) printf("\n🧪 Test %d: %s\n", tests_passed + tests_failed + 1, name)
#define ASSERT_EQ(a, b, msg) if ((a) != (b)) { printf("   ❌ FAIL: %s (expected %d, got %d)\n", msg, (int)(b), (int)(a)); tests_failed++; return; } else { printf("   ✅ %s\n", msg); }
#define ASSERT_TRUE(cond, msg) if (!(cond)) { printf("   ❌ FAIL: %s\n", msg); tests_failed++; return; } else { printf("   ✅ %s\n", msg); }
#define TEST_END() tests_passed++; printf("   ✅ Test passed\n")

/**
 * Test 1: Basic list creation
 * Verify list initializes with correct state
 */
void test_list_creation() {
    TEST_START("Basic list creation");
    
    DynamicList* list = list_create(sizeof(int), 0);
    ASSERT_TRUE(list != NULL, "List created");
    ASSERT_EQ(list->size, 0, "Size is 0");
    ASSERT_EQ(list->capacity, 8, "Initial capacity is 8");
    ASSERT_TRUE(list->data != NULL, "Data array allocated");
    
    list_destroy(list);
    TEST_END();
}

/**
 * Test 2: Add single element
 * Verify adding one element updates size correctly
 */
void test_add_single() {
    TEST_START("Add single element");
    
    DynamicList* list = list_create(sizeof(int), 0);
    int value = 42;
    
    list_add(list, &value);
    ASSERT_EQ(list->size, 1, "Size is 1 after add");
    
    int* retrieved = (int*)list_get(list, 0);
    ASSERT_TRUE(retrieved != NULL, "Element retrieved");
    ASSERT_EQ(*retrieved, 42, "Value is correct");
    
    list_destroy(list);
    TEST_END();
}

/**
 * Test 3: Add multiple elements
 * Verify multiple additions work correctly
 */
void test_add_multiple() {
    TEST_START("Add multiple elements");
    
    DynamicList* list = list_create(sizeof(int), 0);
    
    for (int i = 0; i < 5; i++) {
        list_add(list, &i);
    }
    
    ASSERT_EQ(list->size, 5, "Size is 5");
    
    for (int i = 0; i < 5; i++) {
        int* val = (int*)list_get(list, i);
        ASSERT_EQ(*val, i, "Element at index matches");
    }
    
    list_destroy(list);
    TEST_END();
}

/**
 * Test 4: Capacity expansion
 * Verify list expands when exceeding initial capacity (8 → 16)
 */
void test_capacity_expansion() {
    TEST_START("Capacity expansion");
    
    DynamicList* list = list_create(sizeof(int), 0);
    ASSERT_EQ(list->capacity, 8, "Initial capacity 8");
    
    // Add 9 elements to trigger expansion
    for (int i = 0; i < 9; i++) {
        list_add(list, &i);
    }
    
    ASSERT_EQ(list->size, 9, "Size is 9");
    ASSERT_EQ(list->capacity, 16, "Capacity expanded to 16");
    
    // Verify all elements still accessible
    for (int i = 0; i < 9; i++) {
        int* val = (int*)list_get(list, i);
        ASSERT_EQ(*val, i, "Element preserved after expansion");
    }
    
    list_destroy(list);
    TEST_END();
}

/**
 * Test 5: Remove element
 * Verify element removal and list shifting
 */
void test_remove() {
    TEST_START("Remove element");
    
    DynamicList* list = list_create(sizeof(int), 0);
    
    // Add elements: 10, 20, 30, 40, 50
    for (int i = 1; i <= 5; i++) {
        int val = i * 10;
        list_add(list, &val);
    }
    
    ASSERT_EQ(list->size, 5, "Initial size 5");
    
    // Remove element at index 2 (30)
    list_remove(list, 2);
    
    ASSERT_EQ(list->size, 4, "Size reduced to 4");
    
    // Verify remaining elements: 10, 20, 40, 50
    int expected[] = {10, 20, 40, 50};
    for (int i = 0; i < 4; i++) {
        int* val = (int*)list_get(list, i);
        ASSERT_EQ(*val, expected[i], "Elements shifted correctly");
    }
    
    list_destroy(list);
    TEST_END();
}

/**
 * Test 6: Clear list
 * Verify clear removes all elements but preserves list
 */
void test_clear() {
    TEST_START("Clear list");
    
    DynamicList* list = list_create(sizeof(int), 0);
    
    // Add 5 elements
    for (int i = 0; i < 5; i++) {
        list_add(list, &i);
    }
    
    ASSERT_EQ(list->size, 5, "Size is 5 before clear");
    
    list_clear(list);
    
    ASSERT_EQ(list->size, 0, "Size is 0 after clear");
    ASSERT_TRUE(list->data != NULL, "Data array still allocated");
    
    // Add element after clear
    int val = 99;
    list_add(list, &val);
    ASSERT_EQ(list->size, 1, "Can add after clear");
    
    int* retrieved = (int*)list_get(list, 0);
    ASSERT_EQ(*retrieved, 99, "New element correct");
    
    list_destroy(list);
    TEST_END();
}

/**
 * Test 7: String list
 * Verify list works with pointer types (strings)
 */
void test_string_list() {
    TEST_START("String list");
    
    DynamicList* list = list_create(sizeof(char*), 1);  // is_pointer_type = 1
    
    char* str1 = strdup("Alice");
    char* str2 = strdup("Bob");
    char* str3 = strdup("Charlie");
    
    list_add(list, str1);
    list_add(list, str2);
    list_add(list, str3);
    
    ASSERT_EQ(list->size, 3, "Size is 3");
    
    char* r1 = (char*)list_get(list, 0);
    char* r2 = (char*)list_get(list, 1);
    char* r3 = (char*)list_get(list, 2);
    
    ASSERT_TRUE(strcmp(r1, "Alice") == 0, "First string correct");
    ASSERT_TRUE(strcmp(r2, "Bob") == 0, "Second string correct");
    ASSERT_TRUE(strcmp(r3, "Charlie") == 0, "Third string correct");
    
    free(str1);
    free(str2);
    free(str3);
    list_destroy(list);
    TEST_END();
}

/**
 * Test 8: Empty list operations
 * Verify operations on empty list handle gracefully
 */
void test_empty_list() {
    TEST_START("Empty list operations");
    
    DynamicList* list = list_create(sizeof(int), 0);
    
    ASSERT_EQ(list_size(list), 0, "Empty list size is 0");
    
    // Get on empty list (should return NULL)
    void* result = list_get(list, 0);
    ASSERT_TRUE(result == NULL, "Get on empty returns NULL");
    
    // Clear on empty list (should not crash)
    list_clear(list);
    ASSERT_EQ(list->size, 0, "Clear on empty works");
    
    // Remove on empty list (should not crash)
    list_remove(list, 0);
    ASSERT_EQ(list->size, 0, "Remove on empty works");
    
    list_destroy(list);
    TEST_END();
}

/**
 * Test 9: Large list (stress test)
 * Verify list handles 1000+ elements
 */
void test_large_list() {
    TEST_START("Large list (1000 elements)");
    
    DynamicList* list = list_create(sizeof(int), 0);
    
    // Add 1000 elements
    for (int i = 0; i < 1000; i++) {
        list_add(list, &i);
    }
    
    ASSERT_EQ(list->size, 1000, "Size is 1000");
    
    // Verify capacity grew: 8→16→32→64→128→256→512→1024
    ASSERT_TRUE(list->capacity >= 1024, "Capacity at least 1024");
    
    // Verify random elements
    int* val0 = (int*)list_get(list, 0);
    int* val500 = (int*)list_get(list, 500);
    int* val999 = (int*)list_get(list, 999);
    
    ASSERT_EQ(*val0, 0, "First element correct");
    ASSERT_EQ(*val500, 500, "Middle element correct");
    ASSERT_EQ(*val999, 999, "Last element correct");
    
    list_destroy(list);
    TEST_END();
}

/**
 * Test 10: Mixed operations
 * Verify complex sequence of adds and removes
 */
void test_mixed_operations() {
    TEST_START("Mixed add/remove operations");
    
    DynamicList* list = list_create(sizeof(int), 0);
    
    // Add 10 elements (0-9)
    for (int i = 0; i < 10; i++) {
        list_add(list, &i);
    }
    ASSERT_EQ(list->size, 10, "Added 10 elements");
    
    // Remove 5 elements (indices 1, 3, 5, 7, 9)
    list_remove(list, 9);
    list_remove(list, 7);
    list_remove(list, 5);
    list_remove(list, 3);
    list_remove(list, 1);
    ASSERT_EQ(list->size, 5, "Size is 5 after removals");
    
    // Remaining: 0, 2, 4, 6, 8
    int expected_after_remove[] = {0, 2, 4, 6, 8};
    for (int i = 0; i < 5; i++) {
        int* val = (int*)list_get(list, i);
        ASSERT_EQ(*val, expected_after_remove[i], "Element correct after removes");
    }
    
    // Add 5 more (10-14)
    for (int i = 10; i < 15; i++) {
        list_add(list, &i);
    }
    ASSERT_EQ(list->size, 10, "Size is 10 after adding more");
    
    // Remove 3 from middle (indices 2, 3, 4)
    list_remove(list, 4);
    list_remove(list, 3);
    list_remove(list, 2);
    ASSERT_EQ(list->size, 7, "Final size is 7");
    
    // Final list: 0, 2, 10, 11, 12, 13, 14
    int expected_final[] = {0, 2, 10, 11, 12, 13, 14};
    for (int i = 0; i < 7; i++) {
        int* val = (int*)list_get(list, i);
        ASSERT_EQ(*val, expected_final[i], "Final element correct");
    }
    
    list_destroy(list);
    TEST_END();
}

/**
 * Main test runner
 */
int main() {
    printf("═══════════════════════════════════════════════════════\n");
    printf("  MLP Dynamic List - Comprehensive Test Suite\n");
    printf("═══════════════════════════════════════════════════════\n");
    
    test_list_creation();
    test_add_single();
    test_add_multiple();
    test_capacity_expansion();
    test_remove();
    test_clear();
    test_string_list();
    test_empty_list();
    test_large_list();
    test_mixed_operations();
    
    printf("\n═══════════════════════════════════════════════════════\n");
    printf("  Test Results\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("  ✅ Passed: %d\n", tests_passed);
    printf("  ❌ Failed: %d\n", tests_failed);
    printf("  📊 Total:  %d\n", tests_passed + tests_failed);
    printf("═══════════════════════════════════════════════════════\n");
    
    return tests_failed > 0 ? 1 : 0;
}
