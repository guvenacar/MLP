/**
 * gc.c - Garbage Collector Implementation
 * 
 * Mark-and-Sweep GC with Conservative Stack Scanning
 * 
 * Phase: 9.1 - Core Infrastructure
 * Date: 23 Kasım 2025
 */

#include "gc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

// Forward declarations for promise type (will be defined in simple_runtime.h)
typedef struct Promise Promise;

// ============================================================================
// Global State
// ============================================================================

static GCHeap gc_heap = {
    .head = NULL,
    .total_allocated = 0,
    .collection_threshold = 1024 * 1024,  // 1MB initial threshold
    .num_objects = 0,
    .enabled = 1,
    .num_collections = 0,
    .total_freed = 0,
    .total_freed_objects = 0
};

static GCRootSet gc_roots = {
    .roots = NULL,
    .count = 0,
    .capacity = 0
};

static pthread_mutex_t gc_lock = PTHREAD_MUTEX_INITIALIZER;

// Debug/verbose logging flag
static int gc_verbose = 0;

// Type-specific destructors (optional cleanup)
static void (*gc_destructors[8])(void*) = {NULL};

// ============================================================================
// Initialization & Shutdown
// ============================================================================

void gc_init(void) {
    pthread_mutex_lock(&gc_lock);
    
    // Reset heap state
    gc_heap.head = NULL;
    gc_heap.total_allocated = 0;
    gc_heap.collection_threshold = 1024 * 1024;  // 1MB
    gc_heap.num_objects = 0;
    gc_heap.enabled = 1;
    gc_heap.num_collections = 0;
    gc_heap.total_freed = 0;
    gc_heap.total_freed_objects = 0;
    
    // Reset root set
    if (gc_roots.roots) {
        free(gc_roots.roots);
    }
    gc_roots.roots = NULL;
    gc_roots.count = 0;
    gc_roots.capacity = 0;
    
    if (gc_verbose) {
        printf("[GC] Initialized (threshold: %zu bytes)\n", 
               gc_heap.collection_threshold);
    }
    
    pthread_mutex_unlock(&gc_lock);
}

void gc_shutdown(void) {
    pthread_mutex_lock(&gc_lock);
    
    if (gc_verbose) {
        printf("[GC] Shutting down...\n");
        gc_print_stats();
    }
    
    // Free all remaining objects
    GCObjectHeader* current = gc_heap.head;
    while (current) {
        GCObjectHeader* next = current->next;
        
        // Call destructor if registered
        if (current->type < 8 && gc_destructors[current->type]) {
            void* data = (void*)((char*)current + sizeof(GCObjectHeader));
            gc_destructors[current->type](data);
        }
        
        free(current);
        current = next;
    }
    
    // Free root set
    if (gc_roots.roots) {
        free(gc_roots.roots);
        gc_roots.roots = NULL;
    }
    
    gc_heap.head = NULL;
    gc_heap.total_allocated = 0;
    gc_heap.num_objects = 0;
    
    pthread_mutex_unlock(&gc_lock);
}

// ============================================================================
// Allocation
// ============================================================================

void* gc_malloc(size_t size, GCObjectType type) {
    pthread_mutex_lock(&gc_lock);
    
    // Check if GC needed (before allocation)
    if (gc_heap.enabled && 
        gc_heap.total_allocated > gc_heap.collection_threshold) {
        pthread_mutex_unlock(&gc_lock);
        gc_collect();  // Will re-acquire lock
        pthread_mutex_lock(&gc_lock);
    }
    
    // Allocate with header
    size_t total_size = sizeof(GCObjectHeader) + size;
    GCObjectHeader* header = (GCObjectHeader*)malloc(total_size);
    
    if (!header) {
        // Try GC and retry once
        pthread_mutex_unlock(&gc_lock);
        gc_collect();
        pthread_mutex_lock(&gc_lock);
        
        header = (GCObjectHeader*)malloc(total_size);
        if (!header) {
            pthread_mutex_unlock(&gc_lock);
            fprintf(stderr, "[GC] Out of memory (requested %zu bytes)\n", size);
            return NULL;
        }
    }
    
    // Initialize header
    header->type = type;
    header->marked = 0;
    header->padding = 0;
    header->size = total_size;
    header->next = gc_heap.head;
    
    // Add to heap list
    gc_heap.head = header;
    gc_heap.total_allocated += total_size;
    gc_heap.num_objects++;
    
    // Return pointer to data (skip header)
    void* ptr = (void*)((char*)header + sizeof(GCObjectHeader));
    
    if (gc_verbose) {
        printf("[GC] Allocated %zu bytes (type=%d) at %p\n", 
               size, type, ptr);
    }
    
    pthread_mutex_unlock(&gc_lock);
    return ptr;
}

void* gc_calloc(size_t count, size_t size, GCObjectType type) {
    size_t total = count * size;
    void* ptr = gc_malloc(total, type);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

void* gc_realloc(void* ptr, size_t new_size, GCObjectType type) {
    if (!ptr) {
        // NULL pointer - just allocate
        return gc_malloc(new_size, type);
    }
    
    // Get old size
    size_t old_size = gc_get_size(ptr);
    if (old_size == 0) {
        // Not a GC pointer - error
        fprintf(stderr, "[GC] gc_realloc called on non-GC pointer\n");
        return NULL;
    }
    
    // Allocate new memory
    void* new_ptr = gc_malloc(new_size, type);
    if (!new_ptr) {
        return NULL;
    }
    
    // Copy old data
    size_t copy_size = old_size < new_size ? old_size : new_size;
    memcpy(new_ptr, ptr, copy_size);
    
    // Old memory will be collected by GC (no manual free)
    return new_ptr;
}

char* gc_strdup(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str) + 1;
    char* result = (char*)gc_malloc(len, GC_TYPE_STRING);
    if (result) {
        memcpy(result, str, len);
    }
    return result;
}

GCObjectHeader* gc_get_header(void* ptr) {
    if (!ptr) return NULL;
    return (GCObjectHeader*)((char*)ptr - sizeof(GCObjectHeader));
}

// ============================================================================
// Root Set Management
// ============================================================================

void gc_add_root(void** ptr) {
    pthread_mutex_lock(&gc_lock);
    
    // Grow array if needed
    if (gc_roots.count >= gc_roots.capacity) {
        size_t new_cap = gc_roots.capacity == 0 ? 16 : gc_roots.capacity * 2;
        void*** new_roots = (void***)realloc(gc_roots.roots, 
                                             new_cap * sizeof(void**));
        if (!new_roots) {
            pthread_mutex_unlock(&gc_lock);
            fprintf(stderr, "[GC] Failed to grow root set\n");
            return;
        }
        gc_roots.roots = new_roots;
        gc_roots.capacity = new_cap;
    }
    
    gc_roots.roots[gc_roots.count++] = ptr;
    
    if (gc_verbose) {
        printf("[GC] Added root %p (count: %zu)\n", ptr, gc_roots.count);
    }
    
    pthread_mutex_unlock(&gc_lock);
}

void gc_remove_root(void** ptr) {
    pthread_mutex_lock(&gc_lock);
    
    for (size_t i = 0; i < gc_roots.count; i++) {
        if (gc_roots.roots[i] == ptr) {
            // Swap with last element and decrease count
            gc_roots.roots[i] = gc_roots.roots[--gc_roots.count];
            
            if (gc_verbose) {
                printf("[GC] Removed root %p (count: %zu)\n", 
                       ptr, gc_roots.count);
            }
            
            pthread_mutex_unlock(&gc_lock);
            return;
        }
    }
    
    pthread_mutex_unlock(&gc_lock);
}

// ============================================================================
// Marking
// ============================================================================

void gc_mark_object(void* ptr) {
    if (!ptr) return;
    
    // Validate pointer before trying to access header
    // Skip if pointer looks like a small integer (< 4096)
    if ((unsigned long)ptr < 4096) return;
    
    GCObjectHeader* header = gc_get_header(ptr);
    if (!header) return;
    
    // Verify header is in heap
    if (!gc_is_heap_pointer(header)) return;
    
    if (header->marked) return;  // Already marked
    
    header->marked = 1;
    
    if (gc_verbose) {
        printf("[GC] Marked object at %p (type=%d, size=%zu)\n", 
               ptr, header->type, header->size);
    }
    
    // Type-aware recursive marking
    switch (header->type) {
        case GC_TYPE_PROMISE: {
            // Promise structure from simple_runtime.c:
            // typedef struct Promise {
            //     PromiseState state;  // 4 bytes (enum)
            //     void* value;         // 8 bytes (offset +8 due to padding)
            //     char* error;         // 8 bytes (offset +16)
            //     ...
            // } Promise;
            
            // Mark value field (if it's a GC pointer)
            void** value_ptr = (void**)((char*)ptr + 8);
            if (*value_ptr && gc_is_heap_pointer(*value_ptr)) {
                gc_mark_object(*value_ptr);
            }
            
            // Mark error string (if exists)
            char** error_ptr = (char**)((char*)ptr + 16);
            if (*error_ptr && gc_is_heap_pointer(*error_ptr)) {
                gc_mark_object(*error_ptr);
            }
            break;
        }
        
        case GC_TYPE_LIST: {
            // List structure: void** items, size_t count, size_t capacity
            void*** items_ptr = (void***)ptr;
            size_t* count_ptr = (size_t*)((char*)ptr + sizeof(void**));
            
            if (*items_ptr) {
                // Mark the items array itself
                gc_mark_object(*items_ptr);
                
                // Mark each item in the list
                for (size_t i = 0; i < *count_ptr; i++) {
                    if ((*items_ptr)[i]) {
                        gc_mark_object((*items_ptr)[i]);
                    }
                }
            }
            break;
        }
        
        case GC_TYPE_HASHMAP: {
            // HashMap structure (simplified)
            // Will be properly integrated in Phase 9.2
            // For now, just mark it as reachable
            break;
        }
        
        case GC_TYPE_STRING:
        case GC_TYPE_GENERIC:
        case GC_TYPE_ASYNC_CONTEXT:
        case GC_TYPE_CLOSURE:
            // No internal pointers to mark
            break;
    }
}

void gc_mark(void) {
    if (gc_verbose) {
        printf("[GC] === Mark Phase Started ===\n");
    }
    
    // 1. Mark from registered roots
    for (size_t i = 0; i < gc_roots.count; i++) {
        void* ptr = *gc_roots.roots[i];
        if (ptr) {
            gc_mark_object(ptr);
        }
    }
    
    // 2. Conservative stack scanning
    // For now, we'll use a simplified version
    // In Phase 9.2, we'll implement proper stack scanning
    gc_scan_stack();
    
    if (gc_verbose) {
        printf("[GC] === Mark Phase Complete ===\n");
    }
}

void gc_scan_stack(void) {
    // Conservative stack scanning
    // Scan the stack for potential heap pointers
    
    // Get approximate stack bounds
    // This is a simplified version - proper implementation in Phase 9.2
    void* stack_ptr;
    stack_ptr = __builtin_frame_address(0);
    
    if (gc_verbose) {
        printf("[GC] Stack scanning at %p\n", stack_ptr);
    }
    
    // TODO: Implement proper stack scanning in Phase 9.2
    // For now, this is a placeholder
}

int gc_is_heap_pointer(void* ptr) {
    if (!ptr) return 0;
    
    // Check if pointer falls within any allocated object
    GCObjectHeader* current = gc_heap.head;
    while (current) {
        void* obj_start = (void*)current;
        void* obj_end = (void*)((char*)current + current->size);
        
        if (ptr >= obj_start && ptr < obj_end) {
            return 1;
        }
        
        current = current->next;
    }
    
    return 0;
}

// ============================================================================
// Sweeping
// ============================================================================

void gc_sweep(void) {
    if (gc_verbose) {
        printf("[GC] === Sweep Phase Started ===\n");
    }
    
    GCObjectHeader** current = &gc_heap.head;
    size_t freed_bytes = 0;
    size_t freed_objects = 0;
    
    while (*current) {
        GCObjectHeader* header = *current;
        
        if (!header->marked) {
            // Unreachable object - free it
            *current = header->next;
            
            freed_bytes += header->size;
            freed_objects++;
            
            if (gc_verbose) {
                void* data = (void*)((char*)header + sizeof(GCObjectHeader));
                printf("[GC] Freeing object at %p (type=%d, size=%zu)\n", 
                       data, header->type, header->size);
            }
            
            // Call destructor if registered
            if (header->type < 8 && gc_destructors[header->type]) {
                void* data = (void*)((char*)header + sizeof(GCObjectHeader));
                gc_destructors[header->type](data);
            }
            
            free(header);
        } else {
            // Reachable object - clear mark bit for next cycle
            header->marked = 0;
            current = &header->next;
        }
    }
    
    gc_heap.total_allocated -= freed_bytes;
    gc_heap.num_objects -= freed_objects;
    gc_heap.total_freed += freed_bytes;
    gc_heap.total_freed_objects += freed_objects;
    
    if (gc_verbose) {
        printf("[GC] Freed %zu objects (%zu bytes)\n", 
               freed_objects, freed_bytes);
        printf("[GC] === Sweep Phase Complete ===\n");
    }
}

// ============================================================================
// Collection
// ============================================================================

void gc_collect(void) {
    if (!gc_heap.enabled) return;
    
    pthread_mutex_lock(&gc_lock);
    
    clock_t start = clock();
    
    size_t before_allocated = gc_heap.total_allocated;
    size_t before_objects = gc_heap.num_objects;
    
    if (gc_verbose) {
        printf("[GC] ========================================\n");
        printf("[GC] Collection #%zu Starting\n", gc_heap.num_collections + 1);
        printf("[GC] Before: %zu objects, %zu bytes\n", 
               before_objects, before_allocated);
    }
    
    // Mark phase
    gc_mark();
    
    // Sweep phase
    gc_sweep();
    
    gc_heap.num_collections++;
    
    // Adjust threshold (adaptive)
    // If we freed less than 25%, double the threshold
    size_t after_allocated = gc_heap.total_allocated;
    size_t freed = before_allocated - after_allocated;
    double freed_ratio = (double)freed / before_allocated;
    
    if (freed_ratio < 0.25) {
        gc_heap.collection_threshold *= 2;
        
        if (gc_verbose) {
            printf("[GC] Low reclaim rate (%.1f%%), increasing threshold to %zu\n",
                   freed_ratio * 100, gc_heap.collection_threshold);
        }
    }
    
    clock_t end = clock();
    double elapsed_ms = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
    
    printf("[GC] Collection #%zu completed in %.2f ms\n", 
           gc_heap.num_collections, elapsed_ms);
    printf("[GC] After: %zu objects, %zu bytes (freed %zu bytes)\n",
           gc_heap.num_objects, gc_heap.total_allocated, freed);
    
    pthread_mutex_unlock(&gc_lock);
}

void gc_set_enabled(int enabled) {
    pthread_mutex_lock(&gc_lock);
    gc_heap.enabled = enabled;
    
    if (gc_verbose) {
        printf("[GC] %s\n", enabled ? "Enabled" : "Disabled");
    }
    
    pthread_mutex_unlock(&gc_lock);
}

int gc_is_enabled(void) {
    return gc_heap.enabled;
}

void gc_set_threshold(size_t threshold) {
    pthread_mutex_lock(&gc_lock);
    gc_heap.collection_threshold = threshold;
    
    if (gc_verbose) {
        printf("[GC] Threshold set to %zu bytes\n", threshold);
    }
    
    pthread_mutex_unlock(&gc_lock);
}

size_t gc_get_threshold(void) {
    return gc_heap.collection_threshold;
}

// ============================================================================
// Statistics
// ============================================================================

void gc_get_stats(GCStats* stats) {
    pthread_mutex_lock(&gc_lock);
    
    stats->total_allocated = gc_heap.total_allocated;
    stats->num_objects = gc_heap.num_objects;
    stats->collection_threshold = gc_heap.collection_threshold;
    stats->num_collections = gc_heap.num_collections;
    stats->total_freed = gc_heap.total_freed;
    stats->total_freed_objects = gc_heap.total_freed_objects;
    stats->num_roots = gc_roots.count;
    
    pthread_mutex_unlock(&gc_lock);
}

void gc_print_stats(void) {
    GCStats stats;
    gc_get_stats(&stats);
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║              Garbage Collector Statistics                  ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║ Current State:                                             ║\n");
    printf("║   Objects:         %10zu                               ║\n", stats.num_objects);
    printf("║   Allocated:       %10zu bytes (%.2f MB)              ║\n", 
           stats.total_allocated, stats.total_allocated / (1024.0 * 1024.0));
    printf("║   Threshold:       %10zu bytes (%.2f MB)              ║\n", 
           stats.collection_threshold, stats.collection_threshold / (1024.0 * 1024.0));
    printf("║   Roots:           %10zu                               ║\n", stats.num_roots);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║ Lifetime Statistics:                                       ║\n");
    printf("║   Collections:     %10zu                               ║\n", stats.num_collections);
    printf("║   Freed:           %10zu bytes (%.2f MB)              ║\n", 
           stats.total_freed, stats.total_freed / (1024.0 * 1024.0));
    printf("║   Freed Objects:   %10zu                               ║\n", stats.total_freed_objects);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

// ============================================================================
// Advanced API
// ============================================================================

int gc_is_managed(void* ptr) {
    if (!ptr) return 0;
    
    GCObjectHeader* header = gc_get_header(ptr);
    
    // Check if header looks valid
    pthread_mutex_lock(&gc_lock);
    int found = gc_is_heap_pointer(header);
    pthread_mutex_unlock(&gc_lock);
    
    return found;
}

size_t gc_get_size(void* ptr) {
    if (!ptr) return 0;
    
    GCObjectHeader* header = gc_get_header(ptr);
    if (!header) return 0;
    
    return header->size - sizeof(GCObjectHeader);
}

GCObjectType gc_get_type(void* ptr) {
    if (!ptr) return 0;
    
    GCObjectHeader* header = gc_get_header(ptr);
    if (!header) return 0;
    
    return (GCObjectType)header->type;
}

void gc_register_destructor(GCObjectType type, void (*destructor)(void*)) {
    if (type < 8) {
        gc_destructors[type] = destructor;
        
        if (gc_verbose) {
            printf("[GC] Registered destructor for type %d\n", type);
        }
    }
}

// ============================================================================
// Debug API
// ============================================================================

#ifdef GC_DEBUG

void gc_dump_objects(void) {
    pthread_mutex_lock(&gc_lock);
    
    printf("\n[GC] ========== Object Dump ==========\n");
    printf("[GC] Total: %zu objects, %zu bytes\n\n", 
           gc_heap.num_objects, gc_heap.total_allocated);
    
    GCObjectHeader* current = gc_heap.head;
    size_t index = 0;
    
    while (current) {
        void* data = (void*)((char*)current + sizeof(GCObjectHeader));
        
        printf("[%zu] %p: type=%d, size=%zu, marked=%d\n",
               index++, data, current->type, 
               current->size - sizeof(GCObjectHeader), current->marked);
        
        current = current->next;
    }
    
    printf("\n");
    
    pthread_mutex_unlock(&gc_lock);
}

int gc_verify_heap(void) {
    pthread_mutex_lock(&gc_lock);
    
    int errors = 0;
    GCObjectHeader* current = gc_heap.head;
    size_t counted_objects = 0;
    size_t counted_bytes = 0;
    
    while (current) {
        counted_objects++;
        counted_bytes += current->size;
        
        // Check for corruption
        if (current->type > GC_TYPE_CLOSURE) {
            printf("[GC] ERROR: Invalid type %d at %p\n", 
                   current->type, current);
            errors++;
        }
        
        if (current->size < sizeof(GCObjectHeader)) {
            printf("[GC] ERROR: Invalid size %zu at %p\n", 
                   current->size, current);
            errors++;
        }
        
        current = current->next;
    }
    
    if (counted_objects != gc_heap.num_objects) {
        printf("[GC] ERROR: Object count mismatch (counted=%zu, recorded=%zu)\n",
               counted_objects, gc_heap.num_objects);
        errors++;
    }
    
    if (counted_bytes != gc_heap.total_allocated) {
        printf("[GC] ERROR: Byte count mismatch (counted=%zu, recorded=%zu)\n",
               counted_bytes, gc_heap.total_allocated);
        errors++;
    }
    
    pthread_mutex_unlock(&gc_lock);
    
    if (errors == 0) {
        printf("[GC] Heap verification passed\n");
        return 1;
    } else {
        printf("[GC] Heap verification failed with %d errors\n", errors);
        return 0;
    }
}

void gc_set_verbose(int verbose) {
    gc_verbose = verbose;
    printf("[GC] Verbose logging %s\n", verbose ? "enabled" : "disabled");
}

#endif // GC_DEBUG
