#ifndef SMART_POINTERS_H
#define SMART_POINTERS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

// ============================================================================
// MLP SMART POINTERS SYSTEM - MODULE #51
// ============================================================================
// Rust-style smart pointers: Rc<T>, Arc<T>, Box<T>, Weak<T>
// Reference counting, atomic operations, heap allocation
// MLP syntax: dim ptr as Rc<Node>, dim arc as Arc<Data>

// ============================================================================
// SMART POINTER TYPES
// ============================================================================

typedef enum {
    SP_RC,      // Reference counted (single-threaded)
    SP_ARC,     // Atomic reference counted (thread-safe)
    SP_BOX,     // Heap allocation (single owner)
    SP_WEAK     // Weak reference (non-owning)
} SmartPointerKind;

// Reference counted pointer (Rc<T>)
typedef struct RcInner {
    void* data;                  // Actual data
    size_t strong_count;         // Number of Rc references
    size_t weak_count;           // Number of Weak references
    size_t data_size;            // Size of data in bytes
    char* type_name;             // Type name (e.g., "Node")
    void (*drop_fn)(void*);      // Custom drop function
} RcInner;

typedef struct Rc {
    RcInner* inner;              // Inner reference counted data
    char* mangled_name;          // Mangled type name (e.g., "Rc_Node")
} Rc;

// Atomic reference counted pointer (Arc<T>)
typedef struct ArcInner {
    void* data;                  // Actual data
    _Atomic size_t strong_count; // Atomic strong count
    _Atomic size_t weak_count;   // Atomic weak count
    size_t data_size;            // Size of data in bytes
    char* type_name;             // Type name
    void (*drop_fn)(void*);      // Custom drop function
} ArcInner;

typedef struct Arc {
    ArcInner* inner;             // Inner atomic reference counted data
    char* mangled_name;          // Mangled type name (e.g., "Arc_Data")
} Arc;

// Heap allocated pointer (Box<T>)
typedef struct Box {
    void* data;                  // Heap allocated data
    size_t data_size;            // Size of data in bytes
    char* type_name;             // Type name
    char* mangled_name;          // Mangled type name (e.g., "Box_Node")
    void (*drop_fn)(void*);      // Custom drop function
    bool moved;                  // Move semantics flag
} Box;

// Weak reference (Weak<T>)
typedef struct Weak {
    RcInner* rc_inner;           // Points to Rc inner (or NULL)
    ArcInner* arc_inner;         // Points to Arc inner (or NULL)
    char* mangled_name;          // Mangled type name (e.g., "Weak_Node")
    SmartPointerKind parent_kind; // SP_RC or SP_ARC
} Weak;

// ============================================================================
// SMART POINTER CONTEXT
// ============================================================================

typedef struct SmartPointerContext {
    Rc** rcs;                    // Array of all Rc instances
    size_t rc_count;             // Number of Rc instances
    size_t rc_capacity;          // Capacity of rcs array
    
    Arc** arcs;                  // Array of all Arc instances
    size_t arc_count;            // Number of Arc instances
    size_t arc_capacity;         // Capacity of arcs array
    
    Box** boxes;                 // Array of all Box instances
    size_t box_count;            // Number of Box instances
    size_t box_capacity;         // Capacity of boxes array
    
    Weak** weaks;                // Array of all Weak instances
    size_t weak_count;           // Number of Weak instances
    size_t weak_capacity;        // Capacity of weaks array
    
    size_t total_allocations;    // Total heap allocations
    size_t total_deallocations;  // Total heap deallocations
    size_t current_memory;       // Current memory usage (bytes)
    size_t peak_memory;          // Peak memory usage (bytes)
} SmartPointerContext;

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

SmartPointerContext* smart_pointer_context_create(void);
void smart_pointer_context_destroy(SmartPointerContext* ctx);
void smart_pointer_context_reset(SmartPointerContext* ctx);

// ============================================================================
// Rc<T> OPERATIONS (Single-threaded Reference Counting)
// ============================================================================

// Create new Rc<T>
Rc* rc_new(SmartPointerContext* ctx, const char* type_name, void* data, size_t data_size);

// Clone Rc<T> (increment reference count)
Rc* rc_clone(SmartPointerContext* ctx, Rc* rc);

// Drop Rc<T> (decrement reference count, free if zero)
void rc_drop(SmartPointerContext* ctx, Rc* rc);

// Get strong count
size_t rc_strong_count(Rc* rc);

// Get weak count
size_t rc_weak_count(Rc* rc);

// Get data pointer
void* rc_get(Rc* rc);

// Downgrade to Weak<T>
Weak* rc_downgrade(SmartPointerContext* ctx, Rc* rc);

// ============================================================================
// Arc<T> OPERATIONS (Thread-safe Atomic Reference Counting)
// ============================================================================

// Create new Arc<T>
Arc* arc_new(SmartPointerContext* ctx, const char* type_name, void* data, size_t data_size);

// Clone Arc<T> (atomic increment reference count)
Arc* arc_clone(SmartPointerContext* ctx, Arc* arc);

// Drop Arc<T> (atomic decrement reference count, free if zero)
void arc_drop(SmartPointerContext* ctx, Arc* arc);

// Get strong count (atomic load)
size_t arc_strong_count(Arc* arc);

// Get weak count (atomic load)
size_t arc_weak_count(Arc* arc);

// Get data pointer
void* arc_get(Arc* arc);

// Downgrade to Weak<T>
Weak* arc_downgrade(SmartPointerContext* ctx, Arc* arc);

// ============================================================================
// Box<T> OPERATIONS (Heap Allocation)
// ============================================================================

// Create new Box<T>
Box* box_new(SmartPointerContext* ctx, const char* type_name, void* data, size_t data_size);

// Drop Box<T> (free heap memory)
void box_drop(SmartPointerContext* ctx, Box* box);

// Get data pointer
void* box_get(Box* box);

// Move Box<T> (transfer ownership)
Box* box_move(Box* box);

// ============================================================================
// Weak<T> OPERATIONS (Weak References)
// ============================================================================

// Create new Weak<T> from Rc<T>
Weak* weak_from_rc(SmartPointerContext* ctx, Rc* rc);

// Create new Weak<T> from Arc<T>
Weak* weak_from_arc(SmartPointerContext* ctx, Arc* arc);

// Upgrade Weak<T> to Rc<T> (returns NULL if dropped)
Rc* weak_upgrade_rc(SmartPointerContext* ctx, Weak* weak);

// Upgrade Weak<T> to Arc<T> (returns NULL if dropped)
Arc* weak_upgrade_arc(SmartPointerContext* ctx, Weak* weak);

// Drop Weak<T>
void weak_drop(SmartPointerContext* ctx, Weak* weak);

// Check if Weak<T> is still valid
bool weak_is_valid(Weak* weak);

// ============================================================================
// MEMORY TRACKING
// ============================================================================

// Get current memory usage
size_t smart_pointer_memory_usage(SmartPointerContext* ctx);

// Get peak memory usage
size_t smart_pointer_peak_memory(SmartPointerContext* ctx);

// Get allocation statistics
void smart_pointer_stats(SmartPointerContext* ctx, 
                         size_t* total_allocs, 
                         size_t* total_deallocs,
                         size_t* current_memory);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

// Mangle type name (e.g., "Node" -> "Rc_Node")
char* smart_pointer_mangle_name(SmartPointerKind kind, const char* type_name);

// Register Rc in context
void smart_pointer_register_rc(SmartPointerContext* ctx, Rc* rc);

// Register Arc in context
void smart_pointer_register_arc(SmartPointerContext* ctx, Arc* arc);

// Register Box in context
void smart_pointer_register_box(SmartPointerContext* ctx, Box* box);

// Register Weak in context
void smart_pointer_register_weak(SmartPointerContext* ctx, Weak* weak);

#endif // SMART_POINTERS_H
