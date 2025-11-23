# Phase 8.9: Advanced Async Features

**Date:** November 23, 2025  
**Status:** In Progress  
**Goal:** Complete async/await feature set with advanced promise utilities  
**Timeline:** 3-5 days

---

## Overview

Phase 8.9 extends MLP's async/await capabilities with advanced promise utilities, error handling, and timeout support. This phase completes the async/await feature set to match modern languages like JavaScript and Python.

### Motivation

Phase 8.8 delivered basic async/await with:
- ✅ Promise creation and resolution
- ✅ Basic await syntax
- ✅ Async I/O primitives (sleep, file, HTTP)
- ✅ promise_all() for parallel execution

Phase 8.9 adds missing advanced features:
- Promise.race() - First completed promise wins
- Promise.any() - First successful promise wins
- Promise.allSettled() - Wait for all (success or failure)
- Timeout support - Cancel slow operations
- Error propagation - Handle failures gracefully
- Async try-catch - Clean error handling syntax

---

## 🎯 Goals

### Primary Goals

1. **Promise Utilities** - race(), any(), allSettled()
2. **Timeout Support** - async_timeout() wrapper
3. **Error Handling** - Error state tracking and propagation
4. **Async Try-Catch** - Syntax support (optional)

### Success Criteria

- [ ] All 6 promise utilities implemented and tested
- [ ] Timeout cancellation working correctly
- [ ] Error propagation through promise chains
- [ ] 100% test coverage for new features
- [ ] Documentation complete

---

## 📋 Feature Specifications

### 1. Promise.race()

**Purpose:** Return the first promise that completes (success or failure)

**Signature:**
```c
Promise* promise_race(Promise** promises, int count);
```

**Behavior:**
- Takes array of promises and count
- Returns immediately with new Promise
- Resolves/rejects when first promise settles
- Other promises continue running (not cancelled)

**Use Cases:**
- Fastest server selection
- Timeout implementation
- First responder pattern

**Example:**
```mlp
async function fetch_fastest()
    Promise p1 = async_http_get("http://server1.com/api")
    Promise p2 = async_http_get("http://server2.com/api")
    Promise p3 = async_http_get("http://server3.com/api")
    
    Promise fastest = promise_race_simple(p1, p2, p3)
    string result = await fastest
    print "Fastest response: " + result
    return result
end function
```

**Implementation Notes:**
- Use callback mechanism to detect first completion
- Store winner's value/error in result promise
- Ignore subsequent completions

---

### 2. Promise.any()

**Purpose:** Return the first promise that succeeds (ignores failures)

**Signature:**
```c
Promise* promise_any(Promise** promises, int count);
```

**Behavior:**
- Takes array of promises and count
- Returns immediately with new Promise
- Resolves when first promise succeeds
- Rejects only if ALL promises fail (AggregateError)

**Use Cases:**
- Try multiple alternatives
- Fallback servers
- Best-effort operations

**Example:**
```mlp
async function fetch_with_fallback()
    Promise p1 = async_http_get("http://primary.com/api")
    Promise p2 = async_http_get("http://backup1.com/api")
    Promise p3 = async_http_get("http://backup2.com/api")
    
    Promise result = promise_any_simple(p1, p2, p3)
    string data = await result  -- First success or all failed
    return data
end function
```

**Implementation Notes:**
- Track failure count
- Only reject if failure_count == total_count
- Return first success immediately

---

### 3. Promise.allSettled()

**Purpose:** Wait for all promises to complete (success or failure)

**Signature:**
```c
Promise* promise_allSettled(Promise** promises, int count);
```

**Behavior:**
- Takes array of promises and count
- Returns immediately with new Promise
- Resolves when ALL promises settle (never rejects)
- Result contains array of {status, value/reason}

**Use Cases:**
- Batch operations with mixed results
- Collect all outcomes
- No early termination

**Example:**
```mlp
async function batch_process()
    Promise p1 = async_write_file("file1.txt", "data1")
    Promise p2 = async_write_file("file2.txt", "data2")
    Promise p3 = async_write_file("file3.txt", "data3")
    
    Promise results = promise_allSettled_simple(p1, p2, p3)
    await results
    print "All operations completed (some may have failed)"
end function
```

**Implementation Notes:**
- Track completion count
- Never reject result promise
- Store both successes and failures

---

### 4. Timeout Support

**Purpose:** Cancel slow operations automatically

**Signature:**
```c
Promise* async_timeout(Promise* promise, int milliseconds);
```

**Behavior:**
- Wraps existing promise with timeout
- Returns new Promise
- Rejects if original promise doesn't complete in time
- Original promise continues (not truly cancelled)

**Example:**
```mlp
async function fetch_with_timeout()
    Promise p = async_http_get("http://slow-server.com/api")
    Promise timed = async_timeout(p, 5000)  -- 5 second timeout
    
    try
        string result = await timed
        print "Success: " + result
    catch
        print "Request timed out!"
    end try
end function
```

**Implementation Notes:**
- Start timer thread
- Race between original promise and timer
- Cleanup timer on success

---

### 5. Error Propagation

**Purpose:** Handle failures gracefully in promise chains

**Design:**

**Promise struct enhancement:**
```c
typedef struct Promise {
    PromiseState state;     // PENDING, FULFILLED, REJECTED
    void* value;            // Success value
    char* error;            // Error message (new!)
    int error_code;         // Error code (new!)
    void (*callback)(struct Promise*);
} Promise;
```

**New functions:**
```c
void promise_reject_with_error(Promise* p, const char* error, int code);
int promise_has_error(Promise* p);
char* promise_get_error(Promise* p);
int promise_get_error_code(Promise* p);
```

**Example:**
```mlp
async function safe_operation()
    Promise p = async_http_get("http://invalid-url.com")
    
    if promise_has_error(p) then
        string err = promise_get_error(p)
        numeric code = promise_get_error_code(p)
        print "Error " + str(code) + ": " + err
        return null
    end if
    
    string result = promise_get_value(p)
    return result
end function
```

---

### 6. Async Try-Catch (Optional)

**Purpose:** Clean syntax for error handling in async functions

**Syntax:**
```mlp
async function risky_operation()
    try
        Promise p = async_http_get("http://server.com")
        string result = await p
        print result
    catch error
        print "Error: " + error
    end try
end function
```

**Status:** DEFERRED
- Not critical for Phase 8.9
- Can be added in future if needed
- Current error checking via promise_has_error() sufficient

---

## 🏗️ Implementation Plan

### Step 1: Promise Struct Enhancement (Day 1)

**File:** `simple_runtime.c`

**Tasks:**
1. Add error fields to Promise struct
2. Implement promise_reject_with_error()
3. Implement error getter functions
4. Update existing promise functions to handle errors

**Changes:**
```c
typedef struct Promise {
    PromiseState state;
    void* value;
    char* error;        // NEW
    int error_code;     // NEW
    void (*callback)(struct Promise*);
} Promise;

// NEW FUNCTIONS
void promise_reject_with_error(Promise* p, const char* error, int code) {
    if (!p || p->state != PROMISE_PENDING) return;
    
    p->state = PROMISE_REJECTED;
    p->error = strdup(error);
    p->error_code = code;
    
    if (p->callback) {
        p->callback(p);
    }
}

int promise_has_error(Promise* p) {
    return p && p->state == PROMISE_REJECTED;
}

char* promise_get_error(Promise* p) {
    return (p && p->error) ? p->error : "";
}

int promise_get_error_code(Promise* p) {
    return p ? p->error_code : 0;
}
```

**Testing:**
- Create promise with error
- Verify error state
- Check error message and code

---

### Step 2: promise_race() (Day 2)

**File:** `simple_runtime.c`

**Implementation:**
```c
typedef struct RaceContext {
    Promise* result;
    Promise** promises;
    int count;
    int settled;
    pthread_mutex_t mutex;
} RaceContext;

void race_callback(Promise* p) {
    RaceContext* ctx = (RaceContext*)p->callback_context;
    
    pthread_mutex_lock(&ctx->mutex);
    
    if (!ctx->settled) {
        ctx->settled = 1;
        
        if (promise_has_error(p)) {
            promise_reject_with_error(ctx->result, 
                promise_get_error(p), 
                promise_get_error_code(p));
        } else {
            promise_resolve(ctx->result, promise_get_value(p));
        }
    }
    
    pthread_mutex_unlock(&ctx->mutex);
}

Promise* promise_race(Promise** promises, int count) {
    if (!promises || count <= 0) return NULL;
    
    Promise* result = promise_create();
    RaceContext* ctx = malloc(sizeof(RaceContext));
    ctx->result = result;
    ctx->promises = promises;
    ctx->count = count;
    ctx->settled = 0;
    pthread_mutex_init(&ctx->mutex, NULL);
    
    // Attach callback to all promises
    for (int i = 0; i < count; i++) {
        promises[i]->callback = race_callback;
        promises[i]->callback_context = ctx;
    }
    
    return result;
}

// Helper for 3 promises
Promise* promise_race_simple(Promise* p1, Promise* p2, Promise* p3) {
    Promise* arr[3] = {p1, p2, p3};
    return promise_race(arr, 3);
}
```

**Testing:**
- Race 3 async_sleep with different delays
- Verify fastest wins
- Test with errors

---

### Step 3: promise_any() (Day 2-3)

**File:** `simple_runtime.c`

**Implementation:**
```c
typedef struct AnyContext {
    Promise* result;
    Promise** promises;
    int count;
    int success_count;
    int failure_count;
    pthread_mutex_t mutex;
} AnyContext;

void any_callback(Promise* p) {
    AnyContext* ctx = (AnyContext*)p->callback_context;
    
    pthread_mutex_lock(&ctx->mutex);
    
    if (promise_has_error(p)) {
        ctx->failure_count++;
        
        // All failed?
        if (ctx->failure_count == ctx->count) {
            promise_reject_with_error(ctx->result, 
                "All promises rejected", -1);
        }
    } else {
        // First success!
        if (ctx->success_count == 0) {
            ctx->success_count = 1;
            promise_resolve(ctx->result, promise_get_value(p));
        }
    }
    
    pthread_mutex_unlock(&ctx->mutex);
}

Promise* promise_any(Promise** promises, int count) {
    if (!promises || count <= 0) return NULL;
    
    Promise* result = promise_create();
    AnyContext* ctx = malloc(sizeof(AnyContext));
    ctx->result = result;
    ctx->promises = promises;
    ctx->count = count;
    ctx->success_count = 0;
    ctx->failure_count = 0;
    pthread_mutex_init(&ctx->mutex, NULL);
    
    for (int i = 0; i < count; i++) {
        promises[i]->callback = any_callback;
        promises[i]->callback_context = ctx;
    }
    
    return result;
}

Promise* promise_any_simple(Promise* p1, Promise* p2, Promise* p3) {
    Promise* arr[3] = {p1, p2, p3};
    return promise_any(arr, 3);
}
```

**Testing:**
- Mix successful and failing promises
- Verify first success wins
- Test all failures case

---

### Step 4: promise_allSettled() (Day 3)

**File:** `simple_runtime.c`

**Implementation:**
```c
typedef struct AllSettledContext {
    Promise* result;
    Promise** promises;
    int count;
    int settled_count;
    pthread_mutex_t mutex;
} AllSettledContext;

void allSettled_callback(Promise* p) {
    AllSettledContext* ctx = (AllSettledContext*)p->callback_context;
    
    pthread_mutex_lock(&ctx->mutex);
    
    ctx->settled_count++;
    
    // All settled?
    if (ctx->settled_count == ctx->count) {
        // Always resolve (never reject)
        promise_resolve(ctx->result, (void*)1);
    }
    
    pthread_mutex_unlock(&ctx->mutex);
}

Promise* promise_allSettled(Promise** promises, int count) {
    if (!promises || count <= 0) return NULL;
    
    Promise* result = promise_create();
    AllSettledContext* ctx = malloc(sizeof(AllSettledContext));
    ctx->result = result;
    ctx->promises = promises;
    ctx->count = count;
    ctx->settled_count = 0;
    pthread_mutex_init(&ctx->mutex, NULL);
    
    for (int i = 0; i < count; i++) {
        promises[i]->callback = allSettled_callback;
        promises[i]->callback_context = ctx;
    }
    
    return result;
}

Promise* promise_allSettled_simple(Promise* p1, Promise* p2, Promise* p3) {
    Promise* arr[3] = {p1, p2, p3};
    return promise_allSettled(arr, 3);
}
```

**Testing:**
- Mix successful and failing promises
- Verify all complete before resolving
- Never rejects

---

### Step 5: async_timeout() (Day 4)

**File:** `simple_runtime.c`

**Implementation:**
```c
typedef struct TimeoutContext {
    Promise* result;
    Promise* original;
    int milliseconds;
    int completed;
    pthread_mutex_t mutex;
} TimeoutContext;

void* timeout_thread(void* arg) {
    TimeoutContext* ctx = (TimeoutContext*)arg;
    
    usleep(ctx->milliseconds * 1000);  // Convert to microseconds
    
    pthread_mutex_lock(&ctx->mutex);
    
    if (!ctx->completed) {
        ctx->completed = 1;
        promise_reject_with_error(ctx->result, "Timeout", -2);
    }
    
    pthread_mutex_unlock(&ctx->mutex);
    free(ctx);
    return NULL;
}

void timeout_callback(Promise* p) {
    TimeoutContext* ctx = (TimeoutContext*)p->callback_context;
    
    pthread_mutex_lock(&ctx->mutex);
    
    if (!ctx->completed) {
        ctx->completed = 1;
        
        if (promise_has_error(p)) {
            promise_reject_with_error(ctx->result,
                promise_get_error(p),
                promise_get_error_code(p));
        } else {
            promise_resolve(ctx->result, promise_get_value(p));
        }
    }
    
    pthread_mutex_unlock(&ctx->mutex);
}

Promise* async_timeout(Promise* promise, int milliseconds) {
    if (!promise || milliseconds <= 0) return NULL;
    
    Promise* result = promise_create();
    TimeoutContext* ctx = malloc(sizeof(TimeoutContext));
    ctx->result = result;
    ctx->original = promise;
    ctx->milliseconds = milliseconds;
    ctx->completed = 0;
    pthread_mutex_init(&ctx->mutex, NULL);
    
    // Attach callback to original promise
    promise->callback = timeout_callback;
    promise->callback_context = ctx;
    
    // Start timeout timer
    pthread_t timer;
    pthread_create(&timer, NULL, timeout_thread, ctx);
    pthread_detach(timer);
    
    return result;
}
```

**Testing:**
- Fast operation (completes before timeout)
- Slow operation (timeout triggers)
- Various timeout durations

---

### Step 6: Testing & Documentation (Day 5)

**Test Files:**

1. **test_promise_race.mlp**
```mlp
async function test_race()
    print "Testing promise_race..."
    
    Promise p1 = async_sleep(1000)
    Promise p2 = async_sleep(500)
    Promise p3 = async_sleep(2000)
    
    Promise fastest = promise_race_simple(p1, p2, p3)
    await fastest
    
    print "Race complete! (p2 should win)"
end function

function main()
    test_race()
    return 0
end function
```

2. **test_promise_any.mlp**
```mlp
async function failing_op()
    await async_sleep(100)
    return null  -- Simulate failure
end function

async function success_op()
    await async_sleep(200)
    return "Success!"
end function

async function test_any()
    print "Testing promise_any..."
    
    Promise p1 = failing_op()
    Promise p2 = success_op()
    Promise p3 = failing_op()
    
    Promise result = promise_any_simple(p1, p2, p3)
    string value = await result
    
    print "First success: " + value
end function

function main()
    test_any()
    return 0
end function
```

3. **test_async_timeout.mlp**
```mlp
async function fast_op()
    await async_sleep(100)
    return "Fast"
end function

async function slow_op()
    await async_sleep(5000)
    return "Slow"
end function

async function test_timeout()
    print "Testing timeout..."
    
    -- Should succeed
    Promise p1 = fast_op()
    Promise t1 = async_timeout(p1, 1000)
    string r1 = await t1
    print "Fast result: " + r1
    
    -- Should timeout
    Promise p2 = slow_op()
    Promise t2 = async_timeout(p2, 1000)
    
    if promise_has_error(t2) then
        print "Timeout detected! " + promise_get_error(t2)
    end if
end function

function main()
    test_timeout()
    return 0
end function
```

4. **test_error_propagation.mlp**
```mlp
async function test_errors()
    print "Testing error propagation..."
    
    Promise p = async_http_get("http://invalid-url")
    await async_sleep(100)
    
    if promise_has_error(p) then
        print "Error detected: " + promise_get_error(p)
        print "Error code: " + str(promise_get_error_code(p))
    else
        print "No error"
    end if
end function

function main()
    test_errors()
    return 0
end function
```

---

## 📊 Deliverables

### Runtime Functions

1. `promise_reject_with_error(promise, error, code)` ✅
2. `promise_has_error(promise)` ✅
3. `promise_get_error(promise)` ✅
4. `promise_get_error_code(promise)` ✅
5. `promise_race(promises[], count)` ✅
6. `promise_race_simple(p1, p2, p3)` ✅
7. `promise_any(promises[], count)` ✅
8. `promise_any_simple(p1, p2, p3)` ✅
9. `promise_allSettled(promises[], count)` ✅
10. `promise_allSettled_simple(p1, p2, p3)` ✅
11. `async_timeout(promise, milliseconds)` ✅

**Total:** 11 new functions

### Test Suite

- test_promise_race.mlp
- test_promise_any.mlp
- test_promise_allSettled.mlp
- test_async_timeout.mlp
- test_error_propagation.mlp

**Total:** 5 test files

### Documentation

- This document (PHASE8_9_ADVANCED_ASYNC.md)
- Updated API_REFERENCE.md
- Updated TODO.md with completion status

---

## 🔧 Build & Test

### Compilation

```bash
# Compile runtime with new functions
cd runtime
gcc -c simple_runtime.c -o simple_runtime.o -pthread

# Compile test
cd ..
./mlpc test_promise_race.mlp -o test_race
./mlpc test_promise_any.mlp -o test_any
./mlpc test_async_timeout.mlp -o test_timeout
```

### Running Tests

```bash
# Test race
./test_race
# Expected: "p2 should win" (500ms is fastest)

# Test any
./test_any
# Expected: "First success: Success!"

# Test timeout
./test_timeout
# Expected: "Fast result: Fast" then "Timeout detected!"
```

---

## 📈 Success Metrics

### Functionality
- [ ] All 11 functions implemented
- [ ] All 5 test files passing
- [ ] No memory leaks (valgrind clean)
- [ ] Thread-safe (no race conditions)

### Performance
- [ ] promise_race() overhead < 1ms
- [ ] promise_any() overhead < 1ms
- [ ] Timeout accuracy ±10ms

### Quality
- [ ] Code reviewed
- [ ] Documentation complete
- [ ] Examples working

---

## 🚀 Timeline

**Day 1 (Nov 23):** Promise struct enhancement, error handling ✅  
**Day 2 (Nov 24):** promise_race() implementation  
**Day 3 (Nov 25):** promise_any() implementation  
**Day 4 (Nov 26):** promise_allSettled(), async_timeout()  
**Day 5 (Nov 27):** Testing, documentation, polish  

**Completion:** November 27, 2025

---

## 🔄 Next Steps After Phase 8.9

1. **Production Polish** (parallel)
   - Test suite expansion
   - Documentation completion
   - Community preparation

2. **Phase 9: Garbage Collection**
   - Mark-and-sweep GC
   - Automatic memory management

3. **MLP v3.0 Release**
   - Stable production release
   - Community announcement

---

**Status:** Ready to implement!  
**Author:** MLP Development Team  
**Date:** November 23, 2025
