/**
 * MLP Hash Map - Comprehensive Test Suite
 * Tests all hash map operations with collision handling
 * 
 * Compile: gcc -o test_hashmap test_hashmap.c hashmap.c -I.
 * Run: ./test_hashmap
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hashmap.h"

// Test result tracking
int tests_passed = 0;
int tests_failed = 0;

#define TEST_START(name) printf("\n🧪 Test %d: %s\n", tests_passed + tests_failed + 1, name)
#define ASSERT_EQ(a, b, msg) if ((a) != (b)) { printf("   ❌ FAIL: %s (expected %d, got %d)\n", msg, (int)(b), (int)(a)); tests_failed++; return; } else { printf("   ✅ %s\n", msg); }
#define ASSERT_TRUE(cond, msg) if (!(cond)) { printf("   ❌ FAIL: %s\n", msg); tests_failed++; return; } else { printf("   ✅ %s\n", msg); }
#define ASSERT_STR_EQ(a, b, msg) if (strcmp((a), (b)) != 0) { printf("   ❌ FAIL: %s (expected '%s', got '%s')\n", msg, b, a); tests_failed++; return; } else { printf("   ✅ %s\n", msg); }
#define TEST_END() tests_passed++; printf("   ✅ Test passed\n")

/**
 * Test 1: Basic hash map creation
 */
void test_hashmap_creation() {
    TEST_START("Basic hash map creation");
    
    HashMap* map = hashmap_create(16);
    ASSERT_TRUE(map != NULL, "Map created");
    ASSERT_EQ(map->size, 0, "Initial size is 0");
    ASSERT_EQ(map->capacity, 16, "Capacity is 16");
    ASSERT_TRUE(map->entries != NULL, "Entries allocated");
    
    hashmap_destroy(map);
    TEST_END();
}

/**
 * Test 2: Put and get single entry
 */
void test_put_get_single() {
    TEST_START("Put and get single entry");
    
    HashMap* map = hashmap_create(16);
    
    int value = 42;
    int result = hashmap_put(map, "answer", &value);
    ASSERT_EQ(result, 1, "Put successful");
    ASSERT_EQ(map->size, 1, "Size is 1");
    
    int* retrieved = (int*)hashmap_get(map, "answer");
    ASSERT_TRUE(retrieved != NULL, "Value retrieved");
    ASSERT_EQ(*retrieved, 42, "Value correct");
    
    hashmap_destroy(map);
    TEST_END();
}

/**
 * Test 3: Put multiple entries
 */
void test_put_multiple() {
    TEST_START("Put multiple entries");
    
    HashMap* map = hashmap_create(16);
    
    int val1 = 10, val2 = 20, val3 = 30;
    hashmap_put(map, "one", &val1);
    hashmap_put(map, "two", &val2);
    hashmap_put(map, "three", &val3);
    
    ASSERT_EQ(map->size, 3, "Size is 3");
    
    int* r1 = (int*)hashmap_get(map, "one");
    int* r2 = (int*)hashmap_get(map, "two");
    int* r3 = (int*)hashmap_get(map, "three");
    
    ASSERT_EQ(*r1, 10, "First value correct");
    ASSERT_EQ(*r2, 20, "Second value correct");
    ASSERT_EQ(*r3, 30, "Third value correct");
    
    hashmap_destroy(map);
    TEST_END();
}

/**
 * Test 4: Update existing key
 */
void test_update_existing() {
    TEST_START("Update existing key");
    
    HashMap* map = hashmap_create(16);
    
    int val1 = 100;
    hashmap_put(map, "counter", &val1);
    ASSERT_EQ(map->size, 1, "Size is 1");
    
    int val2 = 200;
    hashmap_put(map, "counter", &val2);
    ASSERT_EQ(map->size, 1, "Size still 1 (updated)");
    
    int* retrieved = (int*)hashmap_get(map, "counter");
    ASSERT_EQ(*retrieved, 200, "Value updated");
    
    hashmap_destroy(map);
    TEST_END();
}

/**
 * Test 5: Remove entry
 */
void test_remove() {
    TEST_START("Remove entry");
    
    HashMap* map = hashmap_create(16);
    
    int val1 = 10, val2 = 20, val3 = 30;
    hashmap_put(map, "a", &val1);
    hashmap_put(map, "b", &val2);
    hashmap_put(map, "c", &val3);
    
    ASSERT_EQ(map->size, 3, "Initial size 3");
    
    int removed = hashmap_remove(map, "b");
    ASSERT_EQ(removed, 1, "Remove successful");
    ASSERT_EQ(map->size, 2, "Size reduced to 2");
    
    void* result = hashmap_get(map, "b");
    ASSERT_TRUE(result == NULL, "Removed key returns NULL");
    
    // Other keys still accessible
    int* r1 = (int*)hashmap_get(map, "a");
    int* r3 = (int*)hashmap_get(map, "c");
    ASSERT_EQ(*r1, 10, "Key 'a' still accessible");
    ASSERT_EQ(*r3, 30, "Key 'c' still accessible");
    
    hashmap_destroy(map);
    TEST_END();
}

/**
 * Test 6: Contains check
 */
void test_contains() {
    TEST_START("Contains check");
    
    HashMap* map = hashmap_create(16);
    
    int val = 42;
    hashmap_put(map, "exists", &val);
    
    int exists = hashmap_contains(map, "exists");
    ASSERT_EQ(exists, 1, "Key exists");
    
    int not_exists = hashmap_contains(map, "not_there");
    ASSERT_EQ(not_exists, 0, "Key doesn't exist");
    
    hashmap_destroy(map);
    TEST_END();
}

/**
 * Test 7: Clear hash map
 */
void test_clear() {
    TEST_START("Clear hash map");
    
    HashMap* map = hashmap_create(16);
    
    int val1 = 1, val2 = 2, val3 = 3;
    hashmap_put(map, "x", &val1);
    hashmap_put(map, "y", &val2);
    hashmap_put(map, "z", &val3);
    
    ASSERT_EQ(map->size, 3, "Size is 3 before clear");
    
    hashmap_clear(map);
    ASSERT_EQ(map->size, 0, "Size is 0 after clear");
    
    void* result = hashmap_get(map, "x");
    ASSERT_TRUE(result == NULL, "Key not found after clear");
    
    // Can add after clear
    int val4 = 99;
    hashmap_put(map, "new", &val4);
    ASSERT_EQ(map->size, 1, "Can add after clear");
    
    hashmap_destroy(map);
    TEST_END();
}

/**
 * Test 8: Auto-resize (load factor)
 */
void test_auto_resize() {
    TEST_START("Auto-resize on load factor");
    
    HashMap* map = hashmap_create(8); // Small capacity
    ASSERT_EQ(map->capacity, 8, "Initial capacity 8");
    
    // Add 6 elements (6/8 = 0.75 > 0.7, triggers resize)
    int values[10];
    for (int i = 0; i < 6; i++) {
        values[i] = i * 10;
        char key[10];
        sprintf(key, "key%d", i);
        hashmap_put(map, key, &values[i]);
    }
    
    ASSERT_EQ(map->size, 6, "Size is 6");
    ASSERT_EQ(map->capacity, 16, "Capacity expanded to 16");
    
    // Verify all keys still accessible
    for (int i = 0; i < 6; i++) {
        char key[10];
        sprintf(key, "key%d", i);
        int* val = (int*)hashmap_get(map, key);
        ASSERT_TRUE(val != NULL, "Key still accessible after resize");
        ASSERT_EQ(*val, i * 10, "Value preserved after resize");
    }
    
    hashmap_destroy(map);
    TEST_END();
}

/**
 * Test 9: String values
 */
void test_string_values() {
    TEST_START("String values");
    
    HashMap* map = hashmap_create(16);
    
    char* str1 = strdup("Hello");
    char* str2 = strdup("World");
    
    hashmap_put(map, "greeting", str1);
    hashmap_put(map, "target", str2);
    
    char* r1 = (char*)hashmap_get(map, "greeting");
    char* r2 = (char*)hashmap_get(map, "target");
    
    ASSERT_STR_EQ(r1, "Hello", "First string correct");
    ASSERT_STR_EQ(r2, "World", "Second string correct");
    
    free(str1);
    free(str2);
    hashmap_destroy(map);
    TEST_END();
}

/**
 * Test 10: Collision handling
 */
void test_collision_handling() {
    TEST_START("Collision handling");
    
    HashMap* map = hashmap_create(4); // Very small to force collisions
    
    int val1 = 1, val2 = 2, val3 = 3;
    hashmap_put(map, "a", &val1);
    hashmap_put(map, "b", &val2);
    hashmap_put(map, "c", &val3);
    
    // All should be accessible despite collisions
    int* r1 = (int*)hashmap_get(map, "a");
    int* r2 = (int*)hashmap_get(map, "b");
    int* r3 = (int*)hashmap_get(map, "c");
    
    ASSERT_TRUE(r1 != NULL && *r1 == 1, "First key accessible");
    ASSERT_TRUE(r2 != NULL && *r2 == 2, "Second key accessible");
    ASSERT_TRUE(r3 != NULL && *r3 == 3, "Third key accessible");
    
    hashmap_destroy(map);
    TEST_END();
}

/**
 * Main test runner
 */
int main() {
    printf("═══════════════════════════════════════════════════════\n");
    printf("  MLP Hash Map - Comprehensive Test Suite\n");
    printf("═══════════════════════════════════════════════════════\n");
    
    test_hashmap_creation();
    test_put_get_single();
    test_put_multiple();
    test_update_existing();
    test_remove();
    test_contains();
    test_clear();
    test_auto_resize();
    test_string_values();
    test_collision_handling();
    
    printf("\n═══════════════════════════════════════════════════════\n");
    printf("  Test Results\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("  ✅ Passed: %d\n", tests_passed);
    printf("  ❌ Failed: %d\n", tests_failed);
    printf("  📊 Total:  %d\n", tests_passed + tests_failed);
    printf("═══════════════════════════════════════════════════════\n");
    
    return tests_failed > 0 ? 1 : 0;
}
