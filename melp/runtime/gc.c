// gc.c - Garbage Collector Implementation
// Simple Mark & Sweep GC for MELP

#include "gc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// ============================================
// GLOBAL STATE
// ============================================

static uint8_t* heap_start = NULL;
static uint8_t* heap_end = NULL;
static uint8_t* heap_ptr = NULL;  // Next free position

static Object* free_list = NULL;  // Free objects list
static size_t heap_usage = 0;
static size_t object_count = 0;

// Root set (stack variables, globals)
#define MAX_ROOTS 1024
static void** roots[MAX_ROOTS];
static size_t root_count = 0;

// Statistics
static size_t total_collections = 0;
static size_t total_freed = 0;

// ============================================
// INITIALIZATION
// ============================================

void gc_init() {
    heap_start = (uint8_t*)malloc(GC_HEAP_SIZE);
    if (!heap_start) {
        fprintf(stderr, "GC: Failed to allocate heap\n");
        exit(1);
    }
    
    heap_end = heap_start + GC_HEAP_SIZE;
    heap_ptr = heap_start;
    heap_usage = 0;
    object_count = 0;
    free_list = NULL;
    root_count = 0;
    
    printf("GC: Initialized with %zu bytes heap\n", (size_t)GC_HEAP_SIZE);
}

// ============================================
// ALLOCATION
// ============================================

void* gc_alloc(size_t size, uint8_t type_id) {
    size_t total_size = OBJECT_HEADER_SIZE + size;
    
    // Check if GC needed
    if ((double)heap_usage / GC_HEAP_SIZE > GC_THRESHOLD) {
        gc_collect();
    }
    
    // Try free list first
    Object** prev = &free_list;
    Object* curr = free_list;
    while (curr) {
        if (curr->size >= total_size) {
            // Found suitable free block
            *prev = curr->next;
            curr->gc_mark = 0;
            curr->type_id = type_id;
            curr->size = total_size;
            curr->next = NULL;
            heap_usage += total_size;
            object_count++;
            return (void*)((uint8_t*)curr + OBJECT_HEADER_SIZE);
        }
        prev = &curr->next;
        curr = curr->next;
    }
    
    // Allocate from heap
    if (heap_ptr + total_size > heap_end) {
        // Out of memory, try GC
        gc_collect();
        
        // Retry free list
        prev = &free_list;
        curr = free_list;
        while (curr) {
            if (curr->size >= total_size) {
                *prev = curr->next;
                curr->gc_mark = 0;
                curr->type_id = type_id;
                curr->size = total_size;
                curr->next = NULL;
                heap_usage += total_size;
                object_count++;
                return (void*)((uint8_t*)curr + OBJECT_HEADER_SIZE);
            }
            prev = &curr->next;
            curr = curr->next;
        }
        
        // Still no memory
        fprintf(stderr, "GC: Out of memory (requested %zu bytes)\n", size);
        gc_print_stats();
        return NULL;
    }
    
    // Allocate new object
    Object* obj = (Object*)heap_ptr;
    obj->gc_mark = 0;
    obj->type_id = type_id;
    obj->size = total_size;
    obj->next = NULL;
    
    heap_ptr += total_size;
    heap_usage += total_size;
    object_count++;
    
    return (void*)((uint8_t*)obj + OBJECT_HEADER_SIZE);
}

// ============================================
// ROOT MANAGEMENT
// ============================================

void gc_add_root(void** root_ptr) {
    if (root_count >= MAX_ROOTS) {
        fprintf(stderr, "GC: Too many roots\n");
        return;
    }
    roots[root_count++] = root_ptr;
}

void gc_remove_root(void** root_ptr) {
    for (size_t i = 0; i < root_count; i++) {
        if (roots[i] == root_ptr) {
            // Shift remaining roots
            for (size_t j = i; j < root_count - 1; j++) {
                roots[j] = roots[j + 1];
            }
            root_count--;
            return;
        }
    }
}

// ============================================
// MARK PHASE
// ============================================

void gc_mark_object(Object* obj) {
    if (!obj || obj->gc_mark) {
        return;  // NULL or already marked
    }
    
    obj->gc_mark = 1;
    
    // For arrays/structs, mark children
    // (In full implementation, traverse object fields)
    // For now, simple types have no children
}

void gc_mark() {
    // Mark from roots
    for (size_t i = 0; i < root_count; i++) {
        void* ptr = *roots[i];
        if (ptr) {
            // Get object header
            Object* obj = (Object*)((uint8_t*)ptr - OBJECT_HEADER_SIZE);
            
            // Verify object is in heap
            if ((uint8_t*)obj >= heap_start && (uint8_t*)obj < heap_ptr) {
                gc_mark_object(obj);
            }
        }
    }
}

// ============================================
// SWEEP PHASE
// ============================================

void gc_sweep() {
    uint8_t* ptr = heap_start;
    size_t freed_count = 0;
    size_t freed_bytes = 0;
    
    while (ptr < heap_ptr) {
        Object* obj = (Object*)ptr;
        
        if (obj->gc_mark) {
            // Live object, unmark for next GC
            obj->gc_mark = 0;
        } else {
            // Dead object, add to free list
            obj->next = free_list;
            free_list = obj;
            
            freed_count++;
            freed_bytes += obj->size;
            heap_usage -= obj->size;
            object_count--;
        }
        
        ptr += obj->size;
    }
    
    total_freed += freed_count;
    
    if (freed_count > 0) {
        printf("GC: Freed %zu objects (%zu bytes)\n", freed_count, freed_bytes);
    }
}

// ============================================
// COLLECTION
// ============================================

void gc_collect() {
    total_collections++;
    
    printf("GC: Collection #%zu (usage: %.1f%%)\n", 
           total_collections, 
           100.0 * heap_usage / GC_HEAP_SIZE);
    
    gc_mark();
    gc_sweep();
}

// ============================================
// STATISTICS
// ============================================

size_t gc_heap_usage() {
    return heap_usage;
}

size_t gc_object_count() {
    return object_count;
}

void gc_print_stats() {
    printf("=== GC Statistics ===\n");
    printf("Heap size:       %zu bytes\n", (size_t)GC_HEAP_SIZE);
    printf("Heap usage:      %zu bytes (%.1f%%)\n", 
           heap_usage, 100.0 * heap_usage / GC_HEAP_SIZE);
    printf("Object count:    %zu\n", object_count);
    printf("Collections:     %zu\n", total_collections);
    printf("Total freed:     %zu objects\n", total_freed);
    printf("Root count:      %zu\n", root_count);
}

// ============================================
// CLEANUP
// ============================================

void gc_destroy() {
    if (heap_start) {
        free(heap_start);
        heap_start = NULL;
        heap_end = NULL;
        heap_ptr = NULL;
    }
    
    printf("GC: Destroyed\n");
}
