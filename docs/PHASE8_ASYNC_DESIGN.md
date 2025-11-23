# Phase 8: Async/Await Implementation Design

**Status:** 🔄 In Progress  
**Started:** November 23, 2025  
**Target:** Asynchronous programming support for MLP

---

## Overview

Phase 8 adds asynchronous programming capabilities to MLP, enabling non-blocking I/O operations and concurrent task execution. This is crucial for modern applications that need to handle multiple operations simultaneously without blocking.

**Key Features:**
- `async` function declarations
- `await` expressions for async operations
- Promise/Future mechanism
- Simple event loop
- Non-blocking I/O operations

---

## Design Goals

1. **Simple Mental Model**: Async/await should feel natural, like synchronous code
2. **No OS Threads**: Single-threaded event loop (like Node.js, Python asyncio)
3. **Minimal Runtime**: Keep the runtime simple and lightweight
4. **Compatibility**: Regular functions should work alongside async functions
5. **Progressive**: Can be added incrementally without breaking existing code

---

## Architecture Overview

```
┌─────────────────────────────────────────────┐
│         MLP Async/Await System              │
├─────────────────────────────────────────────┤
│                                             │
│  ┌──────────────┐      ┌─────────────┐     │
│  │   Async      │      │   Promise   │     │
│  │  Functions   │─────▶│   Objects   │     │
│  └──────────────┘      └─────────────┘     │
│         │                     │             │
│         ▼                     ▼             │
│  ┌──────────────────────────────────┐      │
│  │       Event Loop                 │      │
│  │  - Task Queue                    │      │
│  │  - Scheduler                     │      │
│  │  - Callback Executor             │      │
│  └──────────────────────────────────┘      │
│         │                                   │
│         ▼                                   │
│  ┌──────────────────────────────────┐      │
│  │   Runtime (C Implementation)     │      │
│  └──────────────────────────────────┘      │
└─────────────────────────────────────────────┘
```

---

## Phase 8.1: Syntax Design

### Async Function Declaration

```mlp
-- Simple async function
async function fetch_data()
    string data = await read_file_async("data.txt")
    return data
end

-- Async function with parameters
async function process_files(paths[])
    numeric i = 0
    while i < length(paths)
        string content = await read_file_async(paths[i])
        await write_file_async("output.txt", content)
        i = i + 1
    end
    return "Done"
end
```

### Await Expression

```mlp
-- Await must be inside async function
async function main()
    -- Await a promise/future
    numeric result = await slow_computation()
    
    -- Chain awaits
    string data = await fetch_data()
    numeric count = await process_data(data)
    
    print "Processed: " + str(count)
end
```

### Promise/Future Creation

```mlp
-- Create a promise (returns immediately)
async function delayed_value(seconds)
    await sleep_async(seconds)
    return 42
end

-- Call async function (returns Promise)
numeric promise = delayed_value(2)

-- Await the promise
async function consumer()
    numeric value = await promise
    print "Got value: " + str(value)
end
```

---

## Phase 8.2: Promise/Future Data Structure

### Promise States

```c
typedef enum {
    PROMISE_PENDING,    // Not yet resolved
    PROMISE_RESOLVED,   // Successfully completed
    PROMISE_REJECTED    // Failed with error
} PromiseState;

typedef struct Promise {
    PromiseState state;
    void* value;           // Resolved value
    char* error;           // Rejection reason
    void** callbacks;      // Continuation callbacks
    int callback_count;
    int callback_capacity;
} Promise;
```

### Promise API

```c
// Runtime functions
Promise* promise_create();
void promise_resolve(Promise* p, void* value);
void promise_reject(Promise* p, const char* error);
void promise_then(Promise* p, void (*callback)(void*));
int promise_is_resolved(Promise* p);
void* promise_get_value(Promise* p);
```

---

## Phase 8.3: Event Loop Design

### Simple Event Loop

The event loop runs tasks from a queue until empty:

```c
typedef struct Task {
    void (*function)(void*);  // Task function pointer
    void* argument;           // Task argument
    struct Task* next;        // Next task in queue
} Task;

typedef struct EventLoop {
    Task* head;               // Queue head
    Task* tail;               // Queue tail
    int is_running;           // Loop state
} EventLoop;

// Event loop API
EventLoop* event_loop_create();
void event_loop_push_task(EventLoop* loop, void (*fn)(void*), void* arg);
void event_loop_run(EventLoop* loop);
void event_loop_stop(EventLoop* loop);
```

### Execution Flow

```
1. Main program starts
2. Async functions register tasks in event loop
3. event_loop_run() processes tasks one by one
4. Each task can spawn new tasks (continuations)
5. Loop stops when queue is empty
```

---

## Phase 8.4: Code Generation Strategy

### Async Function Transform

**Original MLP Code:**
```mlp
async function example()
    numeric x = 10
    numeric y = await computation()
    return x + y
end
```

**Generated Assembly (State Machine):**
```asm
example:
    ; State 0: Initial
    mov [state], 0
    mov [x], 10
    
    ; Call computation() -> returns Promise
    call computation
    mov [promise], rax
    
    ; Check if resolved
    mov rdi, [promise]
    call promise_is_resolved
    test rax, rax
    jz .suspend
    
    ; State 1: After await
    mov rdi, [promise]
    call promise_get_value
    mov [y], rax
    
    ; Return x + y
    mov rax, [x]
    add rax, [y]
    ret

.suspend:
    ; Not ready - register continuation
    ; Save state and return to event loop
    mov rdi, [promise]
    lea rsi, [example_resume]
    call promise_then
    ret

example_resume:
    ; Resume from state 1
    jmp example + offset_state1
```

### Await Expression Transform

**Before:**
```mlp
numeric result = await some_async_function()
```

**After (pseudo-code):**
```
1. Call some_async_function() -> Promise
2. Check if promise is resolved
   - YES: Extract value, continue
   - NO: Register continuation, suspend current function
3. Return to event loop
4. When promise resolves, resume from continuation
```

---

## Phase 8.5: Async I/O Operations

### File I/O

```mlp
-- Async file operations
async function read_file_async(path)
    -- Simulated async read (uses event loop)
    return await mlp_async_read(path)
end

async function write_file_async(path, content)
    await mlp_async_write(path, content)
end
```

### Sleep/Delay

```mlp
async function sleep_async(seconds)
    await mlp_async_sleep(seconds)
end

async function delayed_print()
    await sleep_async(1)
    print "1 second passed"
    await sleep_async(1)
    print "2 seconds passed"
end
```

---

## Phase 8.6: Implementation Roadmap

### Step 1: Promise Runtime (Phase 8.2)
- [ ] Implement `Promise` struct in C
- [ ] Promise state management functions
- [ ] Callback registration and execution
- [ ] Test: Create and resolve promises

### Step 2: Event Loop (Phase 8.3)
- [ ] Task queue implementation
- [ ] Event loop scheduler
- [ ] Task execution engine
- [ ] Test: Schedule and run tasks

### Step 3: Async Syntax (Phase 8.4)
- [ ] Lexer: `TOKEN_ASYNC`, `TOKEN_AWAIT`
- [ ] Parser: `AST_ASYNC_FUNCTION`, `AST_AWAIT_EXPR`
- [ ] Basic code generation (no state machine yet)
- [ ] Test: Parse async functions

### Step 4: Simple Async Functions (Phase 8.4)
- [ ] Generate code for async functions
- [ ] Wrap return values in promises
- [ ] Call async functions from sync code
- [ ] Test: Call async function, get promise

### Step 5: Await Transform (Phase 8.4)
- [ ] State machine generation for await
- [ ] Continuation passing
- [ ] Resume after promise resolves
- [ ] Test: await inside async function

### Step 6: Async I/O (Phase 8.5)
- [ ] `mlp_async_read()` - non-blocking file read
- [ ] `mlp_async_write()` - non-blocking file write
- [ ] `mlp_async_sleep()` - async sleep
- [ ] Test: Real async I/O operations

---

## Example Usage

### Complete Async Example

```mlp
-- Async I/O operation
async function load_config()
    string config = await read_file_async("config.json")
    return config
end

-- Process data asynchronously
async function process_data(data)
    await sleep_async(1)  -- Simulate processing delay
    return length(data)
end

-- Main async function
async function main()
    print "Loading config..."
    string config = await load_config()
    print "Config loaded: " + config
    
    print "Processing..."
    numeric size = await process_data(config)
    print "Processed " + str(size) + " bytes"
    
    return "Done"
end

-- Start the program
numeric result_promise = main()
event_loop_run()  -- Run until all tasks complete
```

### Output
```
Loading config...
Config loaded: {...}
Processing...
Processed 123 bytes
```

---

## Technical Challenges

### Challenge 1: State Machine Complexity
**Problem:** Transforming async functions into state machines is complex.  
**Solution:** Start with simple cases (single await), then extend to multiple awaits and loops.

### Challenge 2: Stack Management
**Problem:** Suspending and resuming requires saving local variables.  
**Solution:** Use heap-allocated "async frame" to store local state.

### Challenge 3: Error Handling
**Problem:** How to handle errors in async code?  
**Solution:** Promise rejection mechanism (like JavaScript).

### Challenge 4: Debugging
**Problem:** Stack traces become confusing with continuations.  
**Solution:** Store debug info in promises (later phase).

---

## Comparison with Other Languages

### JavaScript (Node.js)
```javascript
async function example() {
    const data = await fetch('file.txt');
    return data;
}
```
- Single-threaded event loop
- Non-blocking I/O
- Promise-based

### Python (asyncio)
```python
async def example():
    data = await read_file('file.txt')
    return data
```
- Event loop with coroutines
- State machine transformation
- Similar to our design

### Rust
```rust
async fn example() -> String {
    let data = read_file("file.txt").await;
    data
}
```
- Zero-cost async/await
- Complex state machine
- Our simpler version

---

## Testing Strategy

### Unit Tests
1. Promise creation and resolution
2. Event loop task scheduling
3. Async function parsing
4. Code generation correctness

### Integration Tests
1. Simple async function call
2. Await inside async function
3. Multiple awaits in sequence
4. Async I/O operations
5. Error handling

### Example Test Cases

**Test 1: Simple Async**
```mlp
async function get_value()
    return 42
end

async function main()
    numeric x = await get_value()
    print x  -- Should print 42
end
```

**Test 2: Sequential Awaits**
```mlp
async function main()
    numeric a = await get_value(1)
    numeric b = await get_value(2)
    print a + b  -- Should print 3
end
```

**Test 3: Async I/O**
```mlp
async function main()
    string content = await read_file_async("test.txt")
    print content
end
```

---

## Performance Considerations

### Memory Overhead
- Each Promise: ~48 bytes
- Each Task: ~24 bytes
- Async frame: varies by function

### Execution Speed
- Event loop overhead: minimal (~10-20 instructions per task)
- State machine overhead: 1-2 extra jumps per await
- Overall: Similar to JavaScript async/await

---

## Future Enhancements (Post Phase 8)

1. **Async Iterators**: `async for` loops
2. **Parallel Execution**: Worker threads (optional)
3. **Async Generators**: `async generator` functions
4. **Promise Combinators**: `Promise.all()`, `Promise.race()`
5. **Better Error Messages**: Async stack traces

---

## Summary

Phase 8 adds async/await to MLP using:
- **Promise/Future** for representing async operations
- **Event Loop** for task scheduling
- **State Machine** transformation for await
- **Simple API** for async I/O

**Timeline:**
- Phase 8.1-8.2: Design + Promise (2-3 days)
- Phase 8.3: Event Loop (2-3 days)
- Phase 8.4: Async/Await Syntax (3-4 days)
- Phase 8.5: Async I/O (2-3 days)
- **Total: ~2 weeks** for basic async/await support

---

**Next Steps:**
1. Implement Promise struct in `simple_runtime.c`
2. Create event loop implementation
3. Add async/await tokens to lexer
4. Design AST nodes for async functions
5. Start with simple async function (no await first)

**Let's build async/await! 🚀**
