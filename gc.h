/**
 * gc.h - Garbage Collector for MLP Runtime
 * 
 * Mark-and-Sweep Garbage Collection with Conservative Stack Scanning
 * 
 * Features:
 * - Automatic memory management
 * - Type-aware object tracking
 * - Cycle detection
 * - Adaptive collection threshold
 * 
 * Phase: 9.1 - Core Infrastructure
 * Date: 23 Kasım 2025
 */

#ifndef MLP_GC_H
#define MLP_GC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Object Types
// ============================================================================

/**
 * GC-managed object types for type-aware marking
 */
typedef enum {
    GC_TYPE_PROMISE = 1,        // Promise object
    GC_TYPE_STRING = 2,         // C string (char*)
    GC_TYPE_LIST = 3,           // Dynamic array/list
    GC_TYPE_HASHMAP = 4,        // Hash table
    GC_TYPE_GENERIC = 5,        // Generic allocation (no internal pointers)
    GC_TYPE_ASYNC_CONTEXT = 6,  // Async function context
    GC_TYPE_CLOSURE = 7,        // Closure/lambda context
} GCObjectType;

// ============================================================================
// Object Header
// ============================================================================

/**
 * Header prepended to every GC-managed allocation
 * 
 * Memory layout:
 * ┌─────────────────────────────┐
 * │  GCObjectHeader (24 bytes)  │
 * ├─────────────────────────────┤
 * │  Object Data (variable)     │
 * └─────────────────────────────┘
 */
typedef struct GCObjectHeader {
    uint8_t type;               // GCObjectType
    uint8_t marked;             // Mark bit (0 = unreachable, 1 = reachable)
    uint16_t padding;           // Alignment padding
    size_t size;                // Total size including header
    struct GCObjectHeader* next; // Next object in heap list
} GCObjectHeader;

// ============================================================================
// GC Heap
// ============================================================================

/**
 * Global GC heap state
 */
typedef struct {
    GCObjectHeader* head;        // Linked list of all objects
    size_t total_allocated;      // Total bytes allocated
    size_t collection_threshold; // Trigger GC when exceeded
    size_t num_objects;          // Count of allocated objects
    int enabled;                 // GC on/off flag (1 = enabled)
    
    // Statistics
    size_t num_collections;      // Total GC collections
    size_t total_freed;          // Total bytes freed
    size_t total_freed_objects;  // Total objects freed
} GCHeap;

// ============================================================================
// Root Set Management
// ============================================================================

/**
 * GC root set - pointers that are always reachable
 */
typedef struct {
    void*** roots;      // Array of pointer-to-pointer (for tracking updates)
    size_t count;       // Number of roots
    size_t capacity;    // Array capacity
} GCRootSet;

// ============================================================================
// Core API
// ============================================================================

/**
 * Initialize the garbage collector
 * Must be called before any gc_malloc() calls
 */
void gc_init(void);

/**
 * Shutdown the garbage collector
 * Frees all remaining objects and cleans up GC state
 */
void gc_shutdown(void);

/**
 * Allocate memory managed by the GC
 * 
 * @param size - Bytes to allocate (excluding header)
 * @param type - Object type for type-aware marking
 * @return Pointer to allocated memory, or NULL on failure
 * 
 * Note: May trigger gc_collect() if threshold exceeded
 */
void* gc_malloc(size_t size, GCObjectType type);

/**
 * Allocate and zero-initialize memory
 */
void* gc_calloc(size_t count, size_t size, GCObjectType type);

/**
 * Duplicate a string with GC management
 * Equivalent to strdup() but GC-managed
 */
char* gc_strdup(const char* str);

/**
 * Get the object header from a GC-managed pointer
 * 
 * @param ptr - Pointer returned by gc_malloc()
 * @return Pointer to the object header, or NULL if ptr is NULL
 */
GCObjectHeader* gc_get_header(void* ptr);

// ============================================================================
// Root Set API
// ============================================================================

/**
 * Register a pointer as a GC root
 * The pointer will be scanned during GC mark phase
 * 
 * @param ptr - Pointer-to-pointer to register (e.g., &global_var)
 * 
 * Example:
 *   Promise* global_promise = NULL;
 *   gc_add_root((void**)&global_promise);
 */
void gc_add_root(void** ptr);

/**
 * Unregister a GC root
 * 
 * @param ptr - Pointer-to-pointer to unregister
 */
void gc_remove_root(void** ptr);

// ============================================================================
// Collection API
// ============================================================================

/**
 * Trigger a full garbage collection cycle
 * 
 * Process:
 * 1. Mark phase - Traverse reachable objects from roots
 * 2. Sweep phase - Free unmarked objects
 * 3. Update statistics and threshold
 * 
 * Called automatically by gc_malloc() when threshold exceeded,
 * but can also be called manually for testing or memory pressure.
 */
void gc_collect(void);

/**
 * Enable or disable garbage collection
 * 
 * @param enabled - 1 to enable, 0 to disable
 * 
 * Disabling GC is useful for:
 * - Performance-critical sections
 * - Testing/debugging
 * - Manual memory management
 */
void gc_set_enabled(int enabled);

/**
 * Check if GC is enabled
 * 
 * @return 1 if enabled, 0 if disabled
 */
int gc_is_enabled(void);

/**
 * Set the collection threshold
 * GC will trigger when total_allocated exceeds this value
 * 
 * @param threshold - Bytes allocated before triggering GC
 */
void gc_set_threshold(size_t threshold);

/**
 * Get current collection threshold
 */
size_t gc_get_threshold(void);

// ============================================================================
// Statistics API
// ============================================================================

/**
 * GC statistics structure
 */
typedef struct {
    size_t total_allocated;      // Current allocated bytes
    size_t num_objects;          // Current object count
    size_t collection_threshold; // Current threshold
    size_t num_collections;      // Total collections performed
    size_t total_freed;          // Lifetime freed bytes
    size_t total_freed_objects;  // Lifetime freed objects
    size_t num_roots;            // Current root count
} GCStats;

/**
 * Get current GC statistics
 * 
 * @param stats - Pointer to GCStats structure to fill
 */
void gc_get_stats(GCStats* stats);

/**
 * Print GC statistics to stdout
 * Useful for debugging and profiling
 */
void gc_print_stats(void);

// ============================================================================
// Advanced API (Optional)
// ============================================================================

/**
 * Mark a single object as reachable
 * Used internally by gc_collect(), but exposed for custom marking
 * 
 * @param ptr - Pointer to object to mark
 */
void gc_mark_object(void* ptr);

/**
 * Check if a pointer is GC-managed
 * 
 * @param ptr - Pointer to check
 * @return 1 if GC-managed, 0 otherwise
 */
int gc_is_managed(void* ptr);

/**
 * Get the size of an allocated object (excluding header)
 * 
 * @param ptr - GC-managed pointer
 * @return Size in bytes, or 0 if not GC-managed
 */
size_t gc_get_size(void* ptr);

/**
 * Get the type of an allocated object
 * 
 * @param ptr - GC-managed pointer
 * @return GCObjectType, or 0 if not GC-managed
 */
GCObjectType gc_get_type(void* ptr);

// ============================================================================
// Internal Functions (exposed for testing)
// ============================================================================

/**
 * Mark phase - traverse reachable objects from roots
 */
void gc_mark(void);

/**
 * Sweep phase - free unmarked objects
 */
void gc_sweep(void);

/**
 * Conservative stack scanning
 * Scans the stack for potential heap pointers and marks them
 */
void gc_scan_stack(void);

/**
 * Check if a pointer points into the GC heap
 * Used by conservative stack scanning
 */
int gc_is_heap_pointer(void* ptr);

// ============================================================================
// Type-Specific Destructors (optional cleanup)
// ============================================================================

/**
 * Register a destructor for a specific object type
 * Destructor is called when object is freed during sweep
 * 
 * @param type - GCObjectType
 * @param destructor - Function pointer: void (*)(void* ptr)
 */
void gc_register_destructor(GCObjectType type, void (*destructor)(void*));

// ============================================================================
// Debug API
// ============================================================================

#ifdef GC_DEBUG

/**
 * Dump all GC objects to stdout
 * Shows type, size, marked status for each object
 */
void gc_dump_objects(void);

/**
 * Verify GC heap integrity
 * Checks for corruption, double-frees, etc.
 * Returns 1 if OK, 0 if errors detected
 */
int gc_verify_heap(void);

/**
 * Enable verbose GC logging
 */
void gc_set_verbose(int verbose);

#endif // GC_DEBUG

#ifdef __cplusplus
}
#endif

#endif // MLP_GC_H
