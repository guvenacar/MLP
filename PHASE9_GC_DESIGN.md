# Phase 9: Garbage Collection - Design Document

**Start Date:** 23 Kasım 2025  
**Timeline:** 2-3 weeks  
**Status:** 🚀 IN PROGRESS

---

## Executive Summary

### Goals

Implement automatic memory management for MLP to solve the memory leak issues identified in Phase 8.10. The garbage collector will:

1. **Eliminate memory leaks** (384+ bytes per async operation)
2. **Simplify programming** (no manual `promise_free()` calls)
3. **Enable advanced features** (closures, cycles, long-running servers)
4. **Maintain performance** (<5ms GC pause for typical workloads)

### Current Problem

**Memory Leak Report (Phase 8.10):**
- Every `async_sleep()`, `promise_race()`, `async_timeout()` leaks 48 bytes
- No automatic cleanup after `promise_await_safe()`
- Long-running applications accumulate megabytes of leaked memory
- Example: 1M async operations = ~48MB leaked

### Proposed Solution

**Mark-and-Sweep Garbage Collection**

**Why Mark-and-Sweep:**
- ✅ Simple to implement (1-2 weeks)
- ✅ Handles cycles (circular references)
- ✅ Works with C interop (conservative scanning)
- ✅ Proven algorithm (used by many languages)

**Alternatives Considered:**
- ❌ Reference counting - Doesn't handle cycles, complex
- ❌ Generational GC - Over-engineered for current needs
- ❌ Manual memory pools - Doesn't solve general case

---

## Architecture Overview

### High-Level Design

```
┌─────────────────────────────────────────────────────────────┐
│                    MLP Runtime System                        │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐  │
│  │   Allocator  │───▶│  GC Heap     │◀───│  GC Engine   │  │
│  │              │    │              │    │              │  │
│  │ gc_malloc()  │    │ Object Graph │    │ Mark & Sweep │  │
│  │ gc_free()    │    │ Root Set     │    │ Collection   │  │
│  └──────────────┘    └──────────────┘    └──────────────┘  │
│         ▲                    ▲                    ▲          │
│         │                    │                    │          │
│  ┌──────┴────────────────────┴────────────────────┴──────┐  │
│  │            Stack Scanner & Root Tracker               │  │
│  │  - Stack frames  - Global variables  - Registers     │  │
│  └────────────────────────────────────────────────────────┘  │
│                                                               │
├─────────────────────────────────────────────────────────────┤
│                    MLP Application Code                       │
│   - Promises   - Strings   - Lists   - Hash Maps            │
└─────────────────────────────────────────────────────────────┘
```

### Key Components

1. **GC Heap** - Managed memory region for all allocations
2. **Object Metadata** - Header with type, size, mark bit, next pointer
3. **Root Set** - Stack, globals, registers (GC starting points)
4. **Mark Phase** - Traverse reachable objects, set mark bit
5. **Sweep Phase** - Free unmarked objects, clear mark bits

---

## Detailed Design

### 1. Object Header Structure

Every GC-managed object will have a header:

```c
typedef enum {
    GC_TYPE_PROMISE = 1,
    GC_TYPE_STRING = 2,
    GC_TYPE_LIST = 3,
    GC_TYPE_HASHMAP = 4,
    GC_TYPE_GENERIC = 5,
    GC_TYPE_ASYNC_CONTEXT = 6,
} GCObjectType;

typedef struct GCObjectHeader {
    uint8_t type;           // GCObjectType
    uint8_t marked;         // Mark bit for GC
    uint16_t padding;       // Alignment
    size_t size;            // Object size (including header)
    struct GCObjectHeader* next;  // Next object in heap list
} GCObjectHeader;

// Total header size: 24 bytes (64-bit system)
// Overhead: 24 bytes per object (acceptable)
```

**Object Layout:**
```
┌─────────────────────────────┐
│     GCObjectHeader (24B)    │
├─────────────────────────────┤
│     Object Data (varies)    │
│   (Promise, String, etc.)   │
└─────────────────────────────┘
```

### 2. GC Heap Management

```c
typedef struct {
    GCObjectHeader* head;        // Linked list of all objects
    size_t total_allocated;      // Total bytes allocated
    size_t collection_threshold; // Trigger GC when exceeded
    size_t num_objects;          // Count of allocated objects
    int enabled;                 // GC on/off flag
} GCHeap;

// Global GC heap
GCHeap gc_heap = {
    .head = NULL,
    .total_allocated = 0,
    .collection_threshold = 1024 * 1024,  // 1MB initial threshold
    .num_objects = 0,
    .enabled = 1
};
```

### 3. Allocation API

```c
// Replace malloc() with gc_malloc()
void* gc_malloc(size_t size, GCObjectType type) {
    // 1. Check if GC needed
    if (gc_heap.total_allocated > gc_heap.collection_threshold) {
        gc_collect();  // Trigger collection
    }
    
    // 2. Allocate with header
    size_t total_size = sizeof(GCObjectHeader) + size;
    GCObjectHeader* header = malloc(total_size);
    
    if (!header) {
        // Try GC and retry
        gc_collect();
        header = malloc(total_size);
        if (!header) return NULL;  // Out of memory
    }
    
    // 3. Initialize header
    header->type = type;
    header->marked = 0;
    header->size = total_size;
    header->next = gc_heap.head;
    
    // 4. Add to heap list
    gc_heap.head = header;
    gc_heap.total_allocated += total_size;
    gc_heap.num_objects++;
    
    // 5. Return pointer to data (skip header)
    return (void*)((char*)header + sizeof(GCObjectHeader));
}

// Helper: Get header from object pointer
GCObjectHeader* gc_get_header(void* ptr) {
    if (!ptr) return NULL;
    return (GCObjectHeader*)((char*)ptr - sizeof(GCObjectHeader));
}
```

### 4. Root Set Management

```c
typedef struct {
    void** roots;      // Array of root pointers
    size_t count;      // Number of roots
    size_t capacity;   // Array capacity
} GCRootSet;

GCRootSet gc_roots = {
    .roots = NULL,
    .count = 0,
    .capacity = 0
};

// Register a root (global variable, persistent pointer)
void gc_add_root(void** ptr) {
    if (gc_roots.count >= gc_roots.capacity) {
        size_t new_cap = gc_roots.capacity == 0 ? 16 : gc_roots.capacity * 2;
        gc_roots.roots = realloc(gc_roots.roots, new_cap * sizeof(void**));
        gc_roots.capacity = new_cap;
    }
    gc_roots.roots[gc_roots.count++] = ptr;
}

void gc_remove_root(void** ptr) {
    for (size_t i = 0; i < gc_roots.count; i++) {
        if (gc_roots.roots[i] == ptr) {
            gc_roots.roots[i] = gc_roots.roots[--gc_roots.count];
            return;
        }
    }
}
```

### 5. Mark Phase

```c
// Mark single object as reachable
void gc_mark_object(void* ptr) {
    if (!ptr) return;
    
    GCObjectHeader* header = gc_get_header(ptr);
    if (!header) return;
    if (header->marked) return;  // Already marked
    
    header->marked = 1;
    
    // Recursively mark referenced objects based on type
    switch (header->type) {
        case GC_TYPE_PROMISE: {
            Promise* p = (Promise*)ptr;
            // Mark promise value if it's a GC object
            if (p->value) gc_mark_object(p->value);
            // Mark error string
            if (p->error) gc_mark_object((void*)p->error);
            break;
        }
        case GC_TYPE_LIST: {
            // Mark all list elements
            List* list = (List*)ptr;
            for (size_t i = 0; i < list->count; i++) {
                gc_mark_object(list->items[i]);
            }
            break;
        }
        case GC_TYPE_HASHMAP: {
            // Mark all hashmap values
            HashMap* map = (HashMap*)ptr;
            for (size_t i = 0; i < map->capacity; i++) {
                if (map->buckets[i].key) {
                    gc_mark_object((void*)map->buckets[i].key);
                    gc_mark_object(map->buckets[i].value);
                }
            }
            break;
        }
        case GC_TYPE_STRING:
        case GC_TYPE_GENERIC:
            // No internal references
            break;
    }
}

// Mark all reachable objects from roots
void gc_mark() {
    // 1. Mark registered roots
    for (size_t i = 0; i < gc_roots.count; i++) {
        void* ptr = *gc_roots.roots[i];
        gc_mark_object(ptr);
    }
    
    // 2. Conservative stack scanning
    void* stack_top = __builtin_frame_address(0);
    void* stack_bottom = pthread_getattr_np(/* current thread */);
    
    // Scan stack for potential pointers
    for (void** ptr = stack_bottom; ptr < stack_top; ptr++) {
        void* potential_ptr = *ptr;
        // Check if this looks like a heap pointer
        if (gc_is_heap_pointer(potential_ptr)) {
            gc_mark_object(potential_ptr);
        }
    }
}
```

### 6. Sweep Phase

```c
void gc_sweep() {
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
            
            // Call type-specific destructor if needed
            gc_destroy_object(header);
            
            free(header);
        } else {
            // Reachable object - clear mark bit for next cycle
            header->marked = 0;
            current = &header->next;
        }
    }
    
    gc_heap.total_allocated -= freed_bytes;
    gc_heap.num_objects -= freed_objects;
    
    // Log collection stats
    printf("[GC] Collected %zu objects, freed %zu bytes\n", 
           freed_objects, freed_bytes);
}
```

### 7. Collection Trigger

```c
void gc_collect() {
    if (!gc_heap.enabled) return;
    
    clock_t start = clock();
    
    // 1. Mark phase
    gc_mark();
    
    // 2. Sweep phase
    gc_sweep();
    
    // 3. Adjust threshold
    // If we freed less than 25%, increase threshold
    size_t old_threshold = gc_heap.collection_threshold;
    if (gc_heap.total_allocated > old_threshold * 0.75) {
        gc_heap.collection_threshold = old_threshold * 2;
    }
    
    clock_t end = clock();
    double elapsed_ms = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    
    printf("[GC] Collection completed in %.2f ms\n", elapsed_ms);
}
```

---

## Integration with Existing Code

### Phase 9.1: Promise Integration

**Replace manual allocation:**

```c
// BEFORE (simple_runtime.c line 85):
Promise* promise_create() {
    Promise* p = malloc(sizeof(Promise));
    // ...
}

// AFTER:
Promise* promise_create() {
    Promise* p = gc_malloc(sizeof(Promise), GC_TYPE_PROMISE);
    // ...
}
```

**Remove manual frees:**

```c
// BEFORE (throughout codebase):
promise_await_safe(p);
promise_free(p);  // Manual cleanup

// AFTER:
promise_await_safe(p);
// No manual free - GC handles it!
```

### Phase 9.2: String Integration

```c
// String duplication with GC
char* gc_strdup(const char* str) {
    size_t len = strlen(str) + 1;
    char* result = gc_malloc(len, GC_TYPE_STRING);
    if (result) memcpy(result, str, len);
    return result;
}

// Replace strdup() calls:
// BEFORE: p->error = strdup("Timeout expired");
// AFTER:  p->error = gc_strdup("Timeout expired");
```

### Phase 9.3: List & HashMap Integration

```c
// BEFORE (list creation):
List* list = malloc(sizeof(List));
list->items = malloc(capacity * sizeof(void*));

// AFTER:
List* list = gc_malloc(sizeof(List), GC_TYPE_LIST);
list->items = gc_malloc(capacity * sizeof(void*), GC_TYPE_GENERIC);
```

### Phase 9.4: Compiler Integration

**Generate GC root registration:**

```nasm
; For each global variable or persistent local:
; BEFORE (no GC):
section .data
    global_promise: dq 0

; AFTER (with GC):
section .data
    global_promise: dq 0

section .text
_start:
    ; Register as GC root
    lea rdi, [global_promise]
    call gc_add_root
```

---

## Performance Considerations

### GC Overhead

**Expected Overhead:**
- Header: 24 bytes per object (33% for 48-byte promise)
- Mark time: O(reachable objects) - ~0.1μs per object
- Sweep time: O(all objects) - ~0.05μs per object
- Total pause: 1-5ms for 10,000 objects

**Optimization Strategies:**

1. **Lazy Collection**
   - Only collect when threshold exceeded
   - Adaptive threshold (grow if high survival rate)

2. **Write Barriers** (Future)
   - Track pointer updates
   - Incremental marking

3. **Parallel Marking** (Future)
   - Use multiple threads for mark phase
   - Reduce pause time

### Memory Overhead

**Current (No GC):**
- Promise: 48 bytes
- Memory leak: 48 bytes per uncleaned promise

**With GC:**
- Promise: 72 bytes (48 + 24 header)
- Memory leak: 0 bytes (automatic cleanup)

**Trade-off:** +50% memory per object, but -100% leaks

---

## Implementation Plan

### Week 1: Core GC (Days 1-7)

**Day 1-2: Infrastructure**
- ✅ Create `gc.c` and `gc.h`
- ✅ Implement `GCObjectHeader`, `GCHeap`
- ✅ Implement `gc_malloc()`, `gc_get_header()`
- ✅ Implement `gc_add_root()`, `gc_remove_root()`
- ✅ Unit tests for allocator

**Day 3-4: Mark Phase**
- ✅ Implement `gc_mark_object()` (type-aware)
- ✅ Implement `gc_mark()` (root set traversal)
- ✅ Conservative stack scanning
- ✅ Unit tests for marking

**Day 5-6: Sweep Phase**
- ✅ Implement `gc_sweep()` (free unmarked)
- ✅ Implement `gc_collect()` (full cycle)
- ✅ Adaptive threshold logic
- ✅ Unit tests for collection

**Day 7: Integration Testing**
- ✅ Test with promise leaks (Phase 8.10 cases)
- ✅ Verify memory cleanup
- ✅ Measure GC pause times
- ✅ Benchmark overhead

### Week 2: Integration (Days 8-14)

**Day 8-9: Promise System**
- Replace `malloc()` with `gc_malloc()` in promise code
- Remove all `promise_free()` calls
- Update promise resolution to preserve GC metadata
- Test async operations (sleep, timeout, race)

**Day 10-11: String & Data Structures**
- Implement `gc_strdup()`
- Update list allocation
- Update hashmap allocation
- Test collections with GC

**Day 12-13: Compiler Support**
- Generate GC root registrations for globals
- Handle local variable GC roots (scope tracking)
- Update stack frame layout for GC scanning
- Test compiled programs with GC

**Day 14: Testing & Validation**
- Run all Phase 8 tests with GC enabled
- Verify no memory leaks (valgrind)
- Performance regression testing
- Edge case testing

### Week 3: Polish & Optimization (Days 15-21)

**Day 15-16: Performance Tuning**
- Profile GC overhead
- Optimize mark algorithm (reduce recursion)
- Tune collection threshold
- Optimize object header size

**Day 17-18: Advanced Features**
- Finalization support (destructors)
- Weak references (optional)
- GC statistics API
- Manual collection trigger

**Day 19-20: Documentation**
- GC design documentation
- API reference updates
- Migration guide (for users)
- Performance guidelines

**Day 21: Final Testing**
- Stress testing (long-running programs)
- Memory pressure testing
- Integration with all Phase 8 features
- Release preparation

---

## Testing Strategy

### Unit Tests

1. **Allocation Tests**
   - Allocate single object
   - Allocate many objects
   - Trigger GC during allocation
   - Out-of-memory handling

2. **Mark Tests**
   - Mark simple object
   - Mark object graph (nested references)
   - Mark cycles
   - Mark from roots

3. **Sweep Tests**
   - Free unmarked objects
   - Preserve marked objects
   - Update heap statistics
   - Clear mark bits

4. **Collection Tests**
   - Full GC cycle
   - Adaptive threshold
   - Performance measurement
   - Memory reclamation

### Integration Tests

1. **Promise Leak Fix**
   - Run Phase 8.10 edge case tests
   - Run Phase 8.10 integration tests
   - Verify 0 leaked bytes (valgrind)

2. **Long-Running Server**
   - Create 1M async operations
   - Measure memory usage over time
   - Verify stable memory (no growth)

3. **Data Structure Tests**
   - Create/destroy 10K lists
   - Create/destroy 10K hashmaps
   - Verify memory cleanup

### Performance Tests

1. **GC Overhead**
   - Measure pause time (target: <5ms)
   - Measure throughput impact (target: <5%)
   - Compare with manual management

2. **Scalability**
   - Test with 10, 100, 1K, 10K, 100K objects
   - Verify linear scaling

---

## Success Criteria

### Must Have (P0)

- ✅ Zero memory leaks in Phase 8.10 tests (valgrind)
- ✅ <10% performance regression vs manual memory
- ✅ Handles cycles (circular references)
- ✅ Transparent to MLP code (automatic)

### Should Have (P1)

- ✅ <5ms GC pause for typical workloads
- ✅ Adaptive threshold (good performance)
- ✅ Conservative stack scanning works
- ✅ Type-aware marking (correctness)

### Nice to Have (P2)

- Finalization support (destructors)
- Weak references
- GC statistics/profiling API
- Incremental/concurrent collection

---

## Risks & Mitigations

### Risk 1: Conservative Scanning False Positives
**Impact:** May keep objects alive longer than needed  
**Mitigation:** 
- Acceptable for first version
- Future: Precise stack maps from compiler

### Risk 2: GC Pause Too Long
**Impact:** Unacceptable latency for real-time apps  
**Mitigation:**
- Profile and optimize mark/sweep
- Future: Incremental/concurrent GC

### Risk 3: Integration Bugs
**Impact:** Crashes, memory corruption  
**Mitigation:**
- Extensive testing (unit + integration)
- Gradual rollout (feature flag)
- Valgrind on all tests

### Risk 4: Performance Regression
**Impact:** Slower than manual memory management  
**Mitigation:**
- Benchmark before/after
- Profile and optimize hot paths
- Adaptive threshold tuning

---

## Alternative Designs Considered

### 1. Reference Counting

**Pros:**
- Deterministic cleanup (immediate)
- No GC pauses
- Simple to understand

**Cons:**
- ❌ Doesn't handle cycles (circular refs)
- ❌ Overhead on every pointer assignment
- ❌ Complex to implement correctly

**Decision:** REJECTED - Cycles are important for closures, data structures

### 2. Generational GC

**Pros:**
- Better performance (focus on young objects)
- Industry standard (Java, .NET)

**Cons:**
- ❌ Much more complex (3-4 weeks)
- ❌ Write barriers add overhead
- ❌ Over-engineered for current needs

**Decision:** DEFERRED - Consider for Phase 10+

### 3. Arena/Region-Based

**Pros:**
- Very fast allocation/deallocation
- Good cache locality

**Cons:**
- ❌ Requires lifetime annotations
- ❌ Not transparent to programmer
- ❌ Doesn't solve general case

**Decision:** REJECTED - Violates "automatic" requirement

---

## Future Enhancements (Phase 10+)

### Incremental Collection
- Split mark/sweep into smaller chunks
- Reduce max pause time
- Better for real-time applications

### Concurrent Collection
- Mark/sweep in background thread
- Minimal pause (stop-the-world only for root scan)
- Requires write barriers

### Generational GC
- Young generation (frequent, fast collection)
- Old generation (rare, thorough collection)
- Better performance for long-lived objects

### Compacting GC
- Defragment heap (move objects together)
- Better cache locality
- Reduces memory fragmentation

---

## Conclusion

Mark-and-sweep GC is the right choice for Phase 9:
- ✅ Solves memory leak problem completely
- ✅ Reasonable implementation time (2-3 weeks)
- ✅ Good performance characteristics
- ✅ Proven algorithm (battle-tested)
- ✅ Enables future features (closures, cycles)

**Next Step:** Begin implementation - Day 1 (GC Infrastructure)

---

**Status:** Ready to implement 🚀
