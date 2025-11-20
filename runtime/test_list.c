// Test program for List runtime functions
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// List structure (from runtime.c)
typedef struct {
    void** data;
    int size;
    int capacity;
    int element_size;
} List;

// Forward declarations
List* list_create();
void list_destroy(List* list);
int list_add(List* list, void* element);
void* list_get(List* list, int index);
int list_size(List* list);
void list_clear(List* list);
int list_remove(List* list, int index);

int main() {
    printf("=== List Runtime Function Tests ===\n\n");

    // Test 1: Create and destroy
    printf("Test 1: Create and destroy\n");
    List* list = list_create();
    assert(list != NULL);
    assert(list->size == 0);
    assert(list->capacity == 8);
    printf("✅ Created list (size=0, capacity=8)\n");
    list_destroy(list);
    printf("✅ Destroyed list\n\n");

    // Test 2: Add elements
    printf("Test 2: Add elements\n");
    list = list_create();
    list_add(list, (void*)10);
    list_add(list, (void*)20);
    list_add(list, (void*)30);
    assert(list_size(list) == 3);
    printf("✅ Added 3 elements, size=%d\n", list_size(list));

    // Test 3: Get elements
    printf("\nTest 3: Get elements\n");
    assert((long)list_get(list, 0) == 10);
    assert((long)list_get(list, 1) == 20);
    assert((long)list_get(list, 2) == 30);
    printf("✅ list[0] = %ld\n", (long)list_get(list, 0));
    printf("✅ list[1] = %ld\n", (long)list_get(list, 1));
    printf("✅ list[2] = %ld\n", (long)list_get(list, 2));

    // Test 4: Dynamic growth (add more than initial capacity)
    printf("\nTest 4: Dynamic growth\n");
    for (int i = 3; i < 20; i++) {
        list_add(list, (void*)(long)(i * 10));
    }
    assert(list_size(list) == 20);
    assert(list->capacity >= 20);
    printf("✅ Added 20 elements total, capacity grew to %d\n", list->capacity);
    printf("✅ list[10] = %ld\n", (long)list_get(list, 10));
    printf("✅ list[19] = %ld\n", (long)list_get(list, 19));

    // Test 5: Clear
    printf("\nTest 5: Clear list\n");
    list_clear(list);
    assert(list_size(list) == 0);
    assert(list->capacity >= 20);  // Capacity unchanged
    printf("✅ Cleared list, size=0, capacity=%d (unchanged)\n", list->capacity);

    // Test 6: Remove element
    printf("\nTest 6: Remove element\n");
    list_add(list, (void*)100);
    list_add(list, (void*)200);
    list_add(list, (void*)300);
    assert(list_size(list) == 3);
    list_remove(list, 1);  // Remove middle element (200)
    assert(list_size(list) == 2);
    assert((long)list_get(list, 0) == 100);
    assert((long)list_get(list, 1) == 300);
    printf("✅ Removed element at index 1\n");
    printf("✅ list[0] = %ld, list[1] = %ld\n",
           (long)list_get(list, 0), (long)list_get(list, 1));

    // Test 7: Bounds checking
    printf("\nTest 7: Bounds checking\n");
    void* result = list_get(list, 100);  // Out of bounds
    assert(result == NULL);
    printf("✅ Out of bounds access correctly returned NULL\n");

    list_destroy(list);
    printf("\n=== All Tests Passed! ===\n");
    return 0;
}
