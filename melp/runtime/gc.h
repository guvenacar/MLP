// gc.h - Garbage Collector Header
// Simple Mark & Sweep GC for MELP

#ifndef MELP_GC_H
#define MELP_GC_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// ============================================
// CONFIGURATION
// ============================================

#define GC_HEAP_SIZE (1024 * 1024)  // 1 MB heap
#define GC_THRESHOLD 0.75            // Trigger GC at 75% usage

// ============================================
// OBJECT HEADER
// ============================================

typedef struct Object {
    uint8_t gc_mark;      // GC mark bit (0 = white, 1 = black)
    uint8_t type_id;      // Type identifier
    uint32_t size;        // Object size in bytes
    struct Object* next;  // Next object (for free list)
} Object;

#define OBJECT_HEADER_SIZE sizeof(Object)

// ============================================
// TYPE IDs
// ============================================

#define TYPE_INT        1
#define TYPE_FLOAT      2
#define TYPE_STRING     3
#define TYPE_ARRAY      4
#define TYPE_STRUCT     5
#define TYPE_CLOSURE    6

// ============================================
// GC API
// ============================================

// Initialize GC
void gc_init();

// Allocate object (triggers GC if needed)
void* gc_alloc(size_t size, uint8_t type_id);

// Force garbage collection
void gc_collect();

// Register root (stack variable, global)
void gc_add_root(void** root_ptr);
void gc_remove_root(void** root_ptr);

// Statistics
size_t gc_heap_usage();
size_t gc_object_count();
void gc_print_stats();

// Cleanup
void gc_destroy();

// ============================================
// INTERNAL FUNCTIONS
// ============================================

// Mark phase: Mark all reachable objects
void gc_mark();

// Sweep phase: Free unmarked objects
void gc_sweep();

// Mark a single object and its children
void gc_mark_object(Object* obj);

#endif // MELP_GC_H
