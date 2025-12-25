# Memory Leak Detection Report - Phase 8.10

## Executive Summary

**Date:** 23 Kasım 2025  
**Tool:** Valgrind 3.19.0  
**Status:** 🔴 **CRITICAL LEAKS DETECTED**

### Quick Stats

| Test Suite | Definitely Lost | Indirectly Lost | Possibly Lost | Status |
|------------|-----------------|-----------------|---------------|--------|
| Edge Cases | 96 bytes (2 blocks) | 13 bytes (1 block) | 320 bytes (1 block) | 🔴 FAIL |
| Integration | 384 bytes (8 blocks) | 0 bytes | 1,280 bytes (4 blocks) | 🔴 FAIL |

---

## Test 1: Edge Case Tests (`test_edge_simple`)

### Leak Summary
```
LEAK SUMMARY:
   definitely lost: 96 bytes in 2 blocks
   indirectly lost: 13 bytes in 1 blocks
     possibly lost: 320 bytes in 1 blocks
   still reachable: 64 bytes in 2 blocks
```

### Critical Leaks (Definitely Lost)

#### Leak 1: Promise from `promise_new()` (Test 2 - Double Resolve)
**Location:** `simple_runtime.c:1293` (promise_new)  
**Size:** 48 bytes  
**Stack Trace:**
```
at malloc (vg_replace_malloc.c:381)
by promise_create (simple_runtime.c:85)
by promise_new (simple_runtime.c:1293)
by ??? (test_edge_simple:0x40141A)
```

**Analysis:** Promise created via `promise_new()` is never freed after use.

**Impact:** Every call to `promise_new()` leaks 48 bytes.

---

#### Leak 2: Promise from `async_timeout()` (Test 3 - Zero Timeout)
**Location:** `simple_runtime.c:1255` (async_timeout)  
**Size:** 48 bytes (direct) + 13 bytes (indirect error message)  
**Stack Trace:**
```
at malloc (vg_replace_malloc.c:381)
by promise_create (simple_runtime.c:85)
by async_timeout (simple_runtime.c:1255)
by ??? (test_edge_simple:0x40150A)
```

**Analysis:** 
- `async_timeout(0ms)` creates immediate rejection promise
- Promise + error message string leaked (61 total bytes)
- Error message: "Timeout: 0ms" (13 bytes via strdup)

**Impact:** Zero-timeout promises leak both promise struct and error string.

---

### Non-Critical Leaks

#### Thread-Related (Possibly Lost)
**Size:** 320 bytes (pthread TLS)  
**Source:** `async_sleep()` thread creation  
**Status:** ⚠️ EXPECTED - pthread internal structures

#### Still Reachable
**Size:** 64 bytes (2 blocks)  
**Source:** `async_sleep()` thread data structures  
**Status:** ✅ ACCEPTABLE - background thread cleanup

---

## Test 2: Integration Tests (`test_int_short`)

### Leak Summary
```
LEAK SUMMARY:
   definitely lost: 384 bytes in 8 blocks
   indirectly lost: 0 bytes in 0 blocks
     possibly lost: 1,280 bytes in 4 blocks
   still reachable: 260 bytes in 7 blocks
```

### Critical Leaks (Definitely Lost) - 8 Promises

All leaks follow same pattern: **Promise created but never freed after await**

#### Leak Pattern Breakdown

| Source Function | Count | Size Each | Total | Line |
|----------------|-------|-----------|-------|------|
| `async_sleep()` | 4 | 48 bytes | 192 bytes | simple_runtime.c:628 |
| `promise_race()` | 2 | 48 bytes | 96 bytes | simple_runtime.c:977 |
| `async_timeout()` | 1 | 48 bytes | 48 bytes | simple_runtime.c:1260 |
| `promise_new()` | 1 | 48 bytes | 48 bytes | simple_runtime.c:1293 |
| **TOTAL** | **8** | - | **384 bytes** | - |

#### Detailed Leak Locations

**1-3. Three `async_sleep()` promises (Test 1 - Race)**
```c
// test_int_short.mlp - Line ~15-20
p1 = async_sleep(30)  // LEAK
p2 = async_sleep(60)  // LEAK  
p3 = async_sleep(90)  // LEAK
result = promise_race_simple(p1, p2, p3)
status = promise_await_safe(result)
// p1, p2, p3 never freed!
```

**4. `async_sleep()` promise (Test 2 - Timeout Success)**
```c
task = async_sleep(50)  // LEAK
wrapped = async_timeout(task, 200)
status = promise_await_safe(wrapped)
// task never freed!
```

**5. Race result promise (Test 1)**
```c
result = promise_race_simple(...)  // LEAK - result promise
status = promise_await_safe(result)
// result never freed!
```

**6. Timeout wrapper promise (Test 3 - Timeout Failure)**
```c
slow = async_sleep(300)
wrapped = async_timeout(slow, 100)  // LEAK - wrapper promise
status = promise_await_safe(wrapped)
// wrapped never freed!
```

**7. Manual promise (Test 4 - Immediate)**
```c
immediate = promise_new()  // LEAK
promise_resolve_numeric(immediate, 777)
// immediate never freed!
```

**8. Race result promise (Test 4)**
```c
result = promise_race_simple(...)  // LEAK - result promise
// result never freed!
```

### Additional Issues

#### Uninitialized Value Warning
```
Use of uninitialised value at promise_race_poll_thread (simple_runtime.c:1006, 1008)
Created by stack allocation in test_int_short
```

**Analysis:** Race condition - poll thread accessing uninitialized promise array.

---

## Root Cause Analysis

### The Core Problem

**MLP has no automatic garbage collection yet!** (Phase 9 planned)

Current behavior:
1. ✅ Promises created dynamically (`malloc()`)
2. ✅ Promises used and awaited
3. ❌ **Promises NEVER freed** - no `promise_free()` calls in generated code
4. ❌ No reference counting
5. ❌ No automatic cleanup

### Why This Matters

**Production Impact:**
- Long-running async applications will leak memory continuously
- Every `async_sleep()`, `promise_race()`, `async_timeout()` call leaks 48+ bytes
- 1000 async operations = ~48KB leaked
- 1M async operations = ~48MB leaked

**Example Real-World Scenario:**
```mlp
# Web server handling 10,000 requests/day
function handle_request()
    data = async_http_get("https://api.example.com")  # LEAK: 48 bytes
    timeout_data = async_timeout(data, 5000)          # LEAK: 48 bytes
    result = promise_await_safe(timeout_data)
    # Total: 96 bytes leaked per request
end

# Daily leak: 10,000 × 96 = 960KB
# Monthly leak: 960KB × 30 = ~28MB
```

---

## Solutions

### Solution 1: Add Explicit `promise_free()` Function ⚠️ MANUAL

**Implementation:**
```c
// simple_runtime.c
void promise_free(Promise* promise) {
    if (!promise) return;
    if (promise->state == PROMISE_REJECTED && promise->error) {
        free((void*)promise->error);
    }
    free(promise);
}
```

**Usage in MLP:**
```mlp
task = async_sleep(100)
result = promise_await_safe(task)
promise_free(task)  # Manual cleanup
```

**Pros:** 
- ✅ Immediate solution
- ✅ Full control

**Cons:**
- ❌ Manual - error-prone
- ❌ Breaks async/await ergonomics
- ❌ Easy to forget

---

### Solution 2: Automatic Cleanup in Compiler 🎯 RECOMMENDED (Short-term)

**Approach:** Insert `promise_free()` after `promise_await_safe()` in generated assembly

**Implementation in `mlpc`:**
```c
// When compiling promise_await_safe(p):
1. Generate: call promise_await_safe
2. Generate: mov [saved_result], rax
3. ADD: mov rdi, [promise_ptr]
4. ADD: call promise_free
5. Generate: mov rax, [saved_result]
```

**Example Generated Code:**
```nasm
; Before (current - LEAKING):
mov rdi, [p1]
call promise_await_safe
mov [result], rax

; After (with cleanup - FIXED):
mov rdi, [p1]
call promise_await_safe
mov [temp_result], rax    ; Save result
mov rdi, [p1]             ; Reload promise pointer
call promise_free         ; Free promise
mov rax, [temp_result]    ; Restore result
mov [result], rax
```

**Pros:**
- ✅ Automatic - no manual cleanup
- ✅ Transparent to user
- ✅ No syntax changes
- ✅ Can implement immediately

**Cons:**
- ⚠️ Doesn't handle all cases (e.g., promises not awaited)
- ⚠️ Still needs Phase 9 GC for complete solution

---

### Solution 3: Reference Counting 🔄 MEDIUM-TERM

**Approach:** Track references to each promise

**Implementation:**
```c
typedef struct {
    int refcount;           // NEW: Reference counter
    PromiseState state;
    void* value;
    const char* error;
} Promise;

Promise* promise_retain(Promise* p) {
    if (p) p->refcount++;
    return p;
}

void promise_release(Promise* p) {
    if (!p) return;
    if (--p->refcount == 0) {
        if (p->error) free((void*)p->error);
        free(p);
    }
}
```

**Pros:**
- ✅ Handles shared promises
- ✅ Automatic when refcount reaches 0

**Cons:**
- ⚠️ Requires compiler changes (insert retain/release)
- ⚠️ Still manual tracking

---

### Solution 4: Full Garbage Collection 🚀 LONG-TERM (Phase 9)

**Approach:** Mark-and-sweep GC as planned in Phase 9

**Timeline:** 2-3 weeks implementation

**Features:**
- ✅ Fully automatic
- ✅ Handles cycles
- ✅ No manual cleanup
- ✅ Best user experience

**Status:** Planned for Phase 9 (after Phase 8.10 completion)

---

## Recommendations

### Immediate Actions (Phase 8.10)

1. ✅ **Document the issue** (this report)
2. 🔄 **Implement Solution 2** (automatic cleanup in compiler)
   - Add `promise_free()` function to runtime
   - Modify `mlpc` to insert cleanup after await
   - Test with edge cases and integration tests
3. 📝 **Add to API documentation** - Note about memory management

### Short-term (Phase 8.10 completion)

4. ⚠️ **Add compiler warning** - Warn if promise created but never awaited
5. 📊 **Create memory benchmark** - Track leak fixes

### Long-term (Phase 9)

6. 🚀 **Implement full GC** - Replace manual cleanup with automatic GC

---

## Testing Validation

After implementing Solution 2, re-run valgrind:

**Expected Results:**
```
LEAK SUMMARY:
   definitely lost: 0 bytes in 0 blocks        ← GOAL
   indirectly lost: 0 bytes in 0 blocks
     possibly lost: <320 bytes (pthread only)
   still reachable: <100 bytes (background threads)
```

**Commands:**
```bash
# Edge cases
valgrind --leak-check=full ./test_edge_simple

# Integration
valgrind --leak-check=full ./test_int_short

# Both should show 0 definitely lost
```

---

## Appendix: Full Valgrind Logs

### Edge Case Test Log
Saved to: `valgrind_edge.log`

### Integration Test Log
Saved to: `valgrind_integration.log`

---

## Conclusion

**Current Status:** 🔴 **Critical memory leaks detected**

**Impact:** All async operations leak memory in production

**Solution:** Implement automatic `promise_free()` cleanup in compiler (Solution 2)

**Timeline:** 
- Solution 2 implementation: ~2-3 hours
- Validation testing: ~1 hour
- **Total:** ~4 hours to fix

**Next Steps:**
1. Implement `promise_free()` function
2. Modify `mlpc` compiler to insert cleanup
3. Re-test with valgrind
4. Move to Phase 9 (full GC) after Phase 8.10 complete
