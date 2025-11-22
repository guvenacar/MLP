// Test for MLP List implementation
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Copy relevant parts from runtime.c for testing
typedef struct {
    void* data;
    long long length;
    long long capacity;
    long long element_size;
} MLP_List;

// Memory tracking stubs
static long long allocated_bytes = 0;

void* mlp_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr) allocated_bytes += size;
    return ptr;
}

void mlp_free(void* ptr) {
    if (ptr) free(ptr);
}

void* mlp_realloc(void* ptr, size_t size) {
    return realloc(ptr, size);
}

// List implementation
MLP_List* mlp_list_create(long long element_size);
void mlp_list_add(MLP_List* list, void* element);
void* mlp_list_get(MLP_List* list, long long index);
void mlp_list_set(MLP_List* list, long long index, void* element);
void mlp_list_remove(MLP_List* list, long long index);
void mlp_list_insert(MLP_List* list, long long index, void* element);
void mlp_list_clear(MLP_List* list);
long long mlp_list_length(MLP_List* list);
long long mlp_list_capacity(MLP_List* list);
long long mlp_list_is_empty(MLP_List* list);
void mlp_list_free(MLP_List* list);

static int mlp_list_resize(MLP_List* list, long long new_capacity) {
    if (!list || new_capacity < list->length) return 0;
    void* new_data = mlp_realloc(list->data, new_capacity * list->element_size);
    if (!new_data) return 0;
    list->data = new_data;
    list->capacity = new_capacity;
    return 1;
}

MLP_List* mlp_list_create(long long element_size) {
    if (element_size <= 0) return NULL;
    MLP_List* list = (MLP_List*)mlp_malloc(sizeof(MLP_List));
    if (!list) return NULL;
    list->element_size = element_size;
    list->length = 0;
    list->capacity = 8;
    list->data = mlp_malloc(list->capacity * element_size);
    if (!list->data) {
        mlp_free(list);
        return NULL;
    }
    return list;
}

void mlp_list_add(MLP_List* list, void* element) {
    if (!list || !element) return;
    if (list->length >= list->capacity) {
        if (!mlp_list_resize(list, list->capacity * 2)) return;
    }
    char* dest = (char*)list->data + (list->length * list->element_size);
    memcpy(dest, element, list->element_size);
    list->length++;
}

void* mlp_list_get(MLP_List* list, long long index) {
    if (!list || index < 0 || index >= list->length) return NULL;
    return (char*)list->data + (index * list->element_size);
}

void mlp_list_set(MLP_List* list, long long index, void* element) {
    if (!list || !element || index < 0 || index >= list->length) return;
    char* dest = (char*)list->data + (index * list->element_size);
    memcpy(dest, element, list->element_size);
}

void mlp_list_remove(MLP_List* list, long long index) {
    if (!list || index < 0 || index >= list->length) return;
    if (index < list->length - 1) {
        char* dest = (char*)list->data + (index * list->element_size);
        char* src = dest + list->element_size;
        memmove(dest, src, (list->length - index - 1) * list->element_size);
    }
    list->length--;
}

void mlp_list_insert(MLP_List* list, long long index, void* element) {
    if (!list || !element || index < 0 || index > list->length) return;
    if (list->length >= list->capacity) {
        if (!mlp_list_resize(list, list->capacity * 2)) return;
    }
    if (index < list->length) {
        char* dest = (char*)list->data + ((index + 1) * list->element_size);
        char* src = (char*)list->data + (index * list->element_size);
        memmove(dest, src, (list->length - index) * list->element_size);
    }
    char* insert_pos = (char*)list->data + (index * list->element_size);
    memcpy(insert_pos, element, list->element_size);
    list->length++;
}

void mlp_list_clear(MLP_List* list) {
    if (!list) return;
    list->length = 0;
}

long long mlp_list_length(MLP_List* list) {
    return list ? list->length : 0;
}

long long mlp_list_capacity(MLP_List* list) {
    return list ? list->capacity : 0;
}

long long mlp_list_is_empty(MLP_List* list) {
    return (list && list->length == 0) ? 1 : 0;
}

void mlp_list_free(MLP_List* list) {
    if (!list) return;
    if (list->data) mlp_free(list->data);
    mlp_free(list);
}

// Test functions
void test_int_list() {
    printf("=== Testing Integer List ===\n");
    
    MLP_List* list = mlp_list_create(sizeof(long long));
    assert(list != NULL);
    assert(mlp_list_is_empty(list) == 1);
    assert(mlp_list_length(list) == 0);
    
    // Add elements
    for (long long i = 0; i < 20; i++) {
        mlp_list_add(list, &i);
    }
    assert(mlp_list_length(list) == 20);
    
    // Get elements
    for (long long i = 0; i < 20; i++) {
        long long* val = (long long*)mlp_list_get(list, i);
        assert(val != NULL);
        assert(*val == i);
    }
    
    // Set element
    long long new_val = 999;
    mlp_list_set(list, 5, &new_val);
    long long* check = (long long*)mlp_list_get(list, 5);
    assert(*check == 999);
    
    // Remove element
    mlp_list_remove(list, 10);
    assert(mlp_list_length(list) == 19);
    
    // Insert element
    long long insert_val = 777;
    mlp_list_insert(list, 0, &insert_val);
    assert(mlp_list_length(list) == 20);
    long long* first = (long long*)mlp_list_get(list, 0);
    assert(*first == 777);
    
    // Clear
    mlp_list_clear(list);
    assert(mlp_list_is_empty(list) == 1);
    assert(mlp_list_length(list) == 0);
    
    mlp_list_free(list);
    printf("✓ Integer list tests passed\n\n");
}

void test_string_list() {
    printf("=== Testing String List ===\n");
    
    MLP_List* list = mlp_list_create(sizeof(char*));
    assert(list != NULL);
    
    // Add strings
    char* s1 = strdup("Hello");
    char* s2 = strdup("World");
    char* s3 = strdup("MLP");
    
    mlp_list_add(list, &s1);
    mlp_list_add(list, &s2);
    mlp_list_add(list, &s3);
    
    assert(mlp_list_length(list) == 3);
    
    // Get strings
    char** str = (char**)mlp_list_get(list, 0);
    assert(strcmp(*str, "Hello") == 0);
    str = (char**)mlp_list_get(list, 1);
    assert(strcmp(*str, "World") == 0);
    str = (char**)mlp_list_get(list, 2);
    assert(strcmp(*str, "MLP") == 0);
    
    // Clean up
    free(s1);
    free(s2);
    free(s3);
    mlp_list_free(list);
    printf("✓ String list tests passed\n\n");
}

typedef struct {
    int x;
    int y;
} Point;

void test_struct_list() {
    printf("=== Testing Struct List ===\n");
    
    MLP_List* list = mlp_list_create(sizeof(Point));
    assert(list != NULL);
    
    // Add points
    for (int i = 0; i < 10; i++) {
        Point p = {i, i * 2};
        mlp_list_add(list, &p);
    }
    
    assert(mlp_list_length(list) == 10);
    
    // Get points
    for (int i = 0; i < 10; i++) {
        Point* p = (Point*)mlp_list_get(list, i);
        assert(p->x == i);
        assert(p->y == i * 2);
    }
    
    mlp_list_free(list);
    printf("✓ Struct list tests passed\n\n");
}

int main() {
    printf("╔════════════════════════════════════════╗\n");
    printf("║  MLP List Implementation Tests        ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    test_int_list();
    test_string_list();
    test_struct_list();
    
    printf("╔════════════════════════════════════════╗\n");
    printf("║  All tests passed! ✓                  ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    return 0;
}
