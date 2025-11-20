# Phase 5.2 - Documentation Brief (Claude #2)

**Session**: Phase 5.2 - Error Handling & Memory Management
**Your Role**: Documentation & Design Specifications
**Branch**: `claude/phase5-2-docs-[SESSION_ID]`
**Timeline**: ~2 weeks

---

## 🎯 Your Mission

Create comprehensive documentation for Phase 5.2 features:
1. Error handling design document (~600 lines)
2. Memory management design document (~700 lines)
3. Example programs demonstrating new features (~450 lines)
4. API reference updates (~100 lines)
5. Roadmap updates (~20 lines)

**Total Expected**: ~1,870 lines of documentation

---

## 📋 Part 1: Error Handling Design Document

### 1.1 Create docs/ERROR_HANDLING_DESIGN.md

**Goal**: Comprehensive specification for error handling in MLP.

**Structure** (~600 lines):

```markdown
# Error Handling - Design Specification

**Status**: Phase 5.2 - In Progress
**Version**: 1.0
**Last Updated**: [DATE]

---

## Overview

This document specifies error handling mechanisms for MLP, enabling programs to handle errors gracefully, report failures, and ensure clean program termination.

### Goals
- Provide standard error handling functions
- Support custom error codes
- Enable assertion-based debugging
- Maintain clean error reporting to stderr

### Non-Goals
- Exception handling (try/catch) - future work
- Stack unwinding - future work
- Signal handling - future work

---

## 1. Error Handling Functions

### 1.1 exit_with_code()

**Purpose**: Exit program with specific return code

**Signature**:
```mlp
function exit_with_code(code) then
    -- Exits program with given code
end
```

**Parameters**:
- `code` (int): Exit code (0 = success, non-zero = error)

**Behavior**:
- Immediately terminates program
- Returns `code` to operating system
- No cleanup is performed (call before exiting if needed)

**Example**:
```mlp
function main() then
    string file = read_file("config.txt")
    if string_length(file) == 0 then
        print "Error: config.txt not found"
        exit_with_code(1)
    end
    print "Config loaded successfully"
    exit_with_code(0)
end
```

**Common Exit Codes**:
- `0` - Success
- `1` - General error
- `2` - Misuse of command
- `126` - Command cannot execute
- `127` - Command not found
- `128+n` - Fatal error signal n

---

### 1.2 panic()

**Purpose**: Print error message and exit with code 1

**Signature**:
```mlp
function panic(message) then
    -- Prints "PANIC: message" to stderr and exits
end
```

**Parameters**:
- `message` (string): Error message to display

**Behavior**:
- Prints "PANIC: " + message to stderr
- Exits with code 1
- Used for unrecoverable errors

**Example**:
```mlp
function divide(a, b) then
    if b == 0 then
        panic("Division by zero")
    end
    return a / b
end
```

**When to Use**:
- Unrecoverable errors
- Programmer errors (not user errors)
- Critical failures that prevent continuation

**Output Format**:
```
PANIC: <message>
```

---

### 1.3 assert()

**Purpose**: Runtime assertion with error message

**Signature**:
```mlp
function assert(condition, message) then
    -- If condition is false, prints message and exits
end
```

**Parameters**:
- `condition` (int/bool): Condition to check (0 = false)
- `message` (string): Error message if assertion fails

**Behavior**:
- If condition is true (non-zero): no action
- If condition is false (zero):
  - Prints "ASSERTION FAILED: " + message to stderr
  - Exits with code 1

**Example**:
```mlp
function array_get(array, size, index) then
    assert(index >= 0, "Index must be non-negative")
    assert(index < size, "Index out of bounds")
    return array[index]
end
```

**Best Practices**:
- Use for preconditions and postconditions
- Use for invariants that should never fail
- Don't use for expected errors (use if/else instead)

**Output Format**:
```
ASSERTION FAILED: <message>
```

---

### 1.4 get_error_code() / set_error_code()

**Purpose**: Get/set global error code

**Signatures**:
```mlp
function get_error_code() then
    -- Returns last error code set
end

function set_error_code(code) then
    -- Sets error code
end
```

**Parameters**:
- `code` (int): Error code to set

**Return Value**:
- `get_error_code()` returns int

**Behavior**:
- Global error code storage
- Initially 0
- Useful for error propagation

**Example**:
```mlp
function file_operation(filename) then
    string content = read_file(filename)
    if string_length(content) == 0 then
        set_error_code(404)  -- File not found
        return 0
    end
    set_error_code(0)  -- Success
    return 1
end

function main() then
    int success = file_operation("test.txt")
    int error = get_error_code()
    if error != 0 then
        print "Operation failed with error:"
        print error
    end
end
```

---

## 2. Error Handling Patterns

### 2.1 Error Code Pattern

```mlp
function risky_operation() then
    set_error_code(0)

    -- Try operation
    if operation_failed then
        set_error_code(100)
        return 0
    end

    return 1  -- Success
end

function caller() then
    int result = risky_operation()
    if result == 0 then
        int error = get_error_code()
        print "Error code:"
        print error
    end
end
```

### 2.2 Panic on Fatal Error Pattern

```mlp
function initialize_system() then
    int result = setup_critical_resource()
    if result == 0 then
        panic("Failed to initialize critical resource")
    end
end
```

### 2.3 Assert Preconditions Pattern

```mlp
function process_array(array, size) then
    assert(size > 0, "Array size must be positive")
    assert(array != 0, "Array must not be NULL")

    -- Process array safely
end
```

---

## 3. Error Handling in Self-Hosting Compiler

### 3.1 Lexer Errors

```mlp
function lex_error(message, line) then
    print "Lexer error at line "
    print line
    print ": "
    print message
    exit_with_code(1)
end
```

### 3.2 Parser Errors

```mlp
function parse_error(message, token) then
    print "Parser error: "
    print message
    print " at token '"
    print token
    print "'"
    exit_with_code(1)
end
```

### 3.3 Semantic Errors

```mlp
function semantic_error(message) then
    print "Semantic error: "
    print message
    exit_with_code(1)
end
```

---

## 4. Implementation Details

### 4.1 Runtime Implementation

- Single global variable: `mlp_last_error_code`
- All functions use `fprintf(stderr, ...)` for errors
- All panic/assert functions call `exit(1)`

### 4.2 Compiler Integration

- 5 new lexer tokens
- Parser recognizes as built-in functions
- Code generator emits calls to runtime functions

---

## 5. Testing

### 5.1 Unit Tests

- Test each function individually in C
- Test error propagation
- Test output format

### 5.2 Integration Tests

- Test in MLP programs
- Verify exit codes
- Verify stderr output

---

## 6. Examples

See:
- `examples/error_handling_demo.mlp` - Comprehensive examples
- `test/test_error_handling.mlp` - Test suite

---

## 7. Future Enhancements

- Exception handling (try/catch)
- Stack traces
- Error contexts
- Custom error types

---

**Status**: Complete
**Next**: Implement in Phase 5.2
```

**Deliverable**: `docs/ERROR_HANDLING_DESIGN.md` (~600 lines)

---

## 📋 Part 2: Memory Management Design Document

### 2.1 Create docs/MEMORY_MANAGEMENT_DESIGN.md

**Goal**: Comprehensive specification for memory management in MLP.

**Structure** (~700 lines):

```markdown
# Memory Management - Design Specification

**Status**: Phase 5.2 - In Progress
**Version**: 1.0
**Last Updated**: [DATE]

---

## Overview

This document specifies memory management functions for MLP, providing dynamic allocation with tracking and leak detection.

### Goals
- Provide malloc/free wrappers with tracking
- Enable memory leak detection
- Support realloc and calloc
- Track total memory usage

### Non-Goals
- Garbage collection - future work
- Reference counting - future work
- Memory pools - future work

---

## 1. Memory Management Functions

### 1.1 mlp_malloc()

**Purpose**: Allocate memory with tracking

**Signature**:
```mlp
function mlp_malloc(size) then
    -- Returns pointer to allocated memory
end
```

**Parameters**:
- `size` (int): Number of bytes to allocate

**Return Value**:
- Pointer to allocated memory (int/pointer)
- 0 (NULL) on failure

**Behavior**:
- Allocates `size` bytes
- Tracks allocation in internal list
- Updates total_allocated counter

**Example**:
```mlp
function create_buffer() then
    int buffer = mlp_malloc(1024)
    if buffer == 0 then
        panic("Failed to allocate buffer")
    end
    return buffer
end
```

---

### 1.2 mlp_free()

**Purpose**: Free allocated memory

**Signature**:
```mlp
function mlp_free(ptr) then
    -- Frees memory at ptr
end
```

**Parameters**:
- `ptr` (int/pointer): Pointer to memory to free

**Behavior**:
- Removes allocation from tracking list
- Updates total_freed counter
- Calls system free()
- Safe to call with NULL (does nothing)

**Example**:
```mlp
function cleanup_buffer(buffer) then
    mlp_free(buffer)
end
```

---

### 1.3 mlp_realloc()

**Purpose**: Resize allocated memory

**Signature**:
```mlp
function mlp_realloc(ptr, size) then
    -- Returns pointer to resized memory
end
```

**Parameters**:
- `ptr` (int/pointer): Existing memory pointer (or 0 for new)
- `size` (int): New size in bytes

**Return Value**:
- Pointer to resized memory
- 0 (NULL) on failure (original pointer remains valid)

**Behavior**:
- If ptr is NULL: equivalent to mlp_malloc(size)
- If size is 0: equivalent to mlp_free(ptr)
- Otherwise: resizes allocation, may move memory

**Example**:
```mlp
function grow_buffer(buffer, old_size, new_size) then
    int new_buffer = mlp_realloc(buffer, new_size)
    if new_buffer == 0 then
        panic("Failed to grow buffer")
    end
    return new_buffer
end
```

---

### 1.4 mlp_calloc()

**Purpose**: Allocate zeroed memory

**Signature**:
```mlp
function mlp_calloc(count, size) then
    -- Returns pointer to zeroed memory
end
```

**Parameters**:
- `count` (int): Number of elements
- `size` (int): Size of each element

**Return Value**:
- Pointer to allocated zeroed memory
- 0 (NULL) on failure

**Behavior**:
- Allocates count * size bytes
- Zeros all bytes
- Tracks allocation

**Example**:
```mlp
function create_array(size) then
    -- Allocate array of integers (8 bytes each)
    int array = mlp_calloc(size, 8)
    assert(array != 0, "Failed to allocate array")
    return array
end
```

---

### 1.5 get_allocated_bytes()

**Purpose**: Get total allocated memory

**Signature**:
```mlp
function get_allocated_bytes() then
    -- Returns total currently allocated bytes
end
```

**Return Value**:
- Total bytes allocated minus freed (int)

**Example**:
```mlp
function show_memory_usage() then
    int bytes = get_allocated_bytes()
    print "Memory in use: "
    print bytes
    print " bytes"
end
```

---

### 1.6 check_memory_leaks()

**Purpose**: Detect and report memory leaks

**Signature**:
```mlp
function check_memory_leaks() then
    -- Returns number of leaked blocks
end
```

**Return Value**:
- Number of leaked memory blocks (int)
- 0 if no leaks

**Behavior**:
- Iterates through tracking list
- Prints each leaked block to stderr
- Returns count of leaks

**Output Format**:
```
MEMORY LEAK: 256 bytes at 0x7fff12345678
MEMORY LEAK: 128 bytes at 0x7fff12345ABC
TOTAL LEAKS: 2 blocks, 384 bytes
```

**Example**:
```mlp
function main() then
    -- Program logic

    -- Check leaks before exit
    int leaks = check_memory_leaks()
    if leaks > 0 then
        print "WARNING: Memory leaks detected!"
        exit_with_code(1)
    end
    exit_with_code(0)
end
```

---

## 2. Memory Tracking System

### 2.1 Internal Structure

```c
typedef struct MemoryBlock {
    void* ptr;
    size_t size;
    struct MemoryBlock* next;
} MemoryBlock;
```

### 2.2 Global State

```c
static MemoryBlock* memory_list_head = NULL;
static size_t total_allocated = 0;
static size_t total_freed = 0;
```

### 2.3 Tracking Operations

- **track_allocation(ptr, size)**: Add to list
- **untrack_allocation(ptr)**: Remove from list
- **find_allocation(ptr)**: Find block by pointer

---

## 3. Memory Management Patterns

### 3.1 RAII-like Pattern

```mlp
function process_file(filename) then
    int buffer = mlp_malloc(4096)
    assert(buffer != 0, "Failed to allocate buffer")

    -- Use buffer
    string content = read_file(filename)

    -- Clean up before return
    mlp_free(buffer)
    return 1
end
```

### 3.2 Growing Buffer Pattern

```mlp
function build_large_string() then
    int capacity = 256
    int buffer = mlp_malloc(capacity)
    int used = 0

    -- Add data
    while has_more_data then
        if used + 100 > capacity then
            capacity = capacity * 2
            buffer = mlp_realloc(buffer, capacity)
            assert(buffer != 0, "Failed to grow buffer")
        end
        -- Add data to buffer
        used = used + 100
    end

    return buffer
end
```

### 3.3 Array Allocation Pattern

```mlp
function create_token_array(count) then
    -- Each token is 32 bytes
    int tokens = mlp_calloc(count, 32)
    assert(tokens != 0, "Failed to allocate tokens")
    return tokens
end
```

---

## 4. Memory Management in Self-Hosting Compiler

### 4.1 AST Node Allocation

```mlp
function create_ast_node(type) then
    int node = mlp_malloc(64)  -- AST node size
    assert(node != 0, "Failed to allocate AST node")
    -- Initialize node fields
    return node
end
```

### 4.2 Symbol Table Allocation

```mlp
function create_symbol_table() then
    int table = mlp_calloc(256, 8)  -- 256 symbol pointers
    assert(table != 0, "Failed to allocate symbol table")
    return table
end
```

### 4.3 String Pooling

```mlp
function intern_string(str) then
    int len = string_length(str)
    int copy = mlp_malloc(len + 1)
    assert(copy != 0, "Failed to intern string")
    -- Copy string to copy
    return copy
end
```

---

## 5. Memory Leak Detection

### 5.1 How It Works

1. Each allocation is tracked in linked list
2. Each free removes from list
3. `check_memory_leaks()` reports remaining allocations

### 5.2 Best Practices

- Call `check_memory_leaks()` before program exit
- In debug builds, always enable tracking
- Use valgrind/sanitizers for additional verification

### 5.3 Common Leak Sources

- Forgetting to free temporary buffers
- Early returns without cleanup
- Exceptions/panics bypassing cleanup
- Lost pointers (overwriting without freeing)

---

## 6. Performance Considerations

### 6.1 Tracking Overhead

- Each allocation adds 24 bytes tracking overhead
- Linked list traversal on free is O(n)
- For production: consider compile-time flag to disable tracking

### 6.2 Optimization Strategies

- Memory pools for fixed-size allocations
- Arena allocator for compiler phases
- Bump allocator for temporary allocations

---

## 7. Implementation Details

### 7.1 Platform Considerations

- Uses standard malloc/free/realloc/calloc
- Thread-safe if underlying system allocator is thread-safe
- NULL pointer handling matches C standard

### 7.2 Error Handling

- Returns NULL on allocation failure
- No automatic panic on OOM (caller decides)
- Tracking failures don't crash program

---

## 8. Testing

### 8.1 Unit Tests

- Test each function individually
- Test leak detection
- Test memory tracking accuracy

### 8.2 Stress Tests

- Large allocations
- Many small allocations
- Realloc chains
- Leak scenarios

---

## 9. Examples

See:
- `examples/memory_management_demo.mlp` - Comprehensive examples
- `test/test_memory_management.mlp` - Test suite

---

## 10. Future Enhancements

- Garbage collection
- Reference counting
- Memory pools
- Arena allocators
- Smart pointers
- RAII wrappers

---

**Status**: Complete
**Next**: Implement in Phase 5.2
```

**Deliverable**: `docs/MEMORY_MANAGEMENT_DESIGN.md` (~700 lines)

---

## 📋 Part 3: Example Programs

### 3.1 Create examples/error_handling_demo.mlp

**Goal**: Demonstrate all error handling features

**Content** (~200 lines):

```mlp
-- Error Handling Demo for MLP
-- Demonstrates: exit_with_code, panic, assert, error codes

print "===================================="
print "MLP Error Handling Demo"
print "===================================="
print ""

-- ==================== Section 1: Error Codes ====================

function test_error_codes() then
    print "=== Error Code Management ==="

    set_error_code(0)
    print "Set error code to 0"

    int code = get_error_code()
    print "Retrieved error code: "
    print code

    set_error_code(42)
    code = get_error_code()
    print "Set error code to 42, retrieved: "
    print code

    set_error_code(0)
    print ""
    return 0
end

-- ==================== Section 2: Assertions ====================

function test_assertions() then
    print "=== Assertion Tests ==="

    -- This assertion should pass
    assert(1, "This assertion should pass")
    print "Assertion 1 passed"

    int x = 10
    assert(x > 0, "x must be positive")
    print "Assertion 2 passed: x > 0"

    int array_size = 5
    int index = 3
    assert(index >= 0, "Index must be non-negative")
    assert(index < array_size, "Index must be within bounds")
    print "Assertion 3 passed: index in bounds"

    print ""
    return 0
end

-- ==================== Section 3: Safe Division ====================

function safe_divide(a, b) then
    if b == 0 then
        set_error_code(100)  -- Division by zero error
        return 0
    end
    set_error_code(0)
    return a / b
end

function test_safe_divide() then
    print "=== Safe Division Test ==="

    int result = safe_divide(10, 2)
    int error = get_error_code()
    if error == 0 then
        print "10 / 2 = "
        print result
    end

    result = safe_divide(10, 0)
    error = get_error_code()
    if error != 0 then
        print "Division by zero detected, error code: "
        print error
    end

    print ""
    return 0
end

-- ==================== Section 4: File Processing ====================

function process_file(filename) then
    print "Processing file: "
    print filename

    -- Simulate file check
    if string_length(filename) == 0 then
        set_error_code(404)  -- File not found
        return 0
    end

    -- Simulate processing
    print "File processed successfully"
    set_error_code(0)
    return 1
end

function test_file_processing() then
    print "=== File Processing Test ==="

    int success = process_file("data.txt")
    int error = get_error_code()
    if error == 0 then
        print "File operation succeeded"
    end

    success = process_file("")
    error = get_error_code()
    if error != 0 then
        print "File operation failed, error code: "
        print error
    end

    print ""
    return 0
end

-- ==================== Section 5: Precondition Checks ====================

function array_get(index, max_index) then
    assert(index >= 0, "Index must be non-negative")
    assert(index < max_index, "Index out of bounds")

    -- Simulate array access
    print "Accessing array at index "
    print index
    return index * 10  -- Dummy value
end

function test_array_access() then
    print "=== Array Access Test ==="

    int value = array_get(2, 10)
    print "Retrieved value: "
    print value

    value = array_get(5, 10)
    print "Retrieved value: "
    print value

    print ""
    return 0
end

-- ==================== Section 6: Error Propagation ====================

function level3_function() then
    set_error_code(303)
    return 0
end

function level2_function() then
    int result = level3_function()
    if result == 0 then
        int error = get_error_code()
        print "Level 2 detected error: "
        print error
        return 0
    end
    return 1
end

function level1_function() then
    int result = level2_function()
    if result == 0 then
        int error = get_error_code()
        print "Level 1 detected error: "
        print error
        return 0
    end
    return 1
end

function test_error_propagation() then
    print "=== Error Propagation Test ==="

    int result = level1_function()
    if result == 0 then
        print "Error successfully propagated through call stack"
    end

    print ""
    return 0
end

-- ==================== Main Tests ====================

test_error_codes()
test_assertions()
test_safe_divide()
test_file_processing()
test_array_access()
test_error_propagation()

print "===================================="
print "All Error Handling Tests Completed!"
print "===================================="

-- Note: panic() and exit_with_code() examples in separate files
-- as they terminate the program
```

**Deliverable**: `examples/error_handling_demo.mlp` (~200 lines)

---

### 3.2 Create examples/memory_management_demo.mlp

**Goal**: Demonstrate all memory management features

**Content** (~250 lines):

```mlp
-- Memory Management Demo for MLP
-- Demonstrates: mlp_malloc, mlp_free, mlp_realloc, mlp_calloc,
--               get_allocated_bytes, check_memory_leaks

print "===================================="
print "MLP Memory Management Demo"
print "===================================="
print ""

-- ==================== Section 1: Basic Allocation ====================

function test_basic_allocation() then
    print "=== Basic Allocation Test ==="

    print "Allocating 256 bytes..."
    int buffer = mlp_malloc(256)
    assert(buffer != 0, "Failed to allocate buffer")
    print "Buffer allocated successfully"

    int allocated = get_allocated_bytes()
    print "Bytes allocated: "
    print allocated

    print "Freeing buffer..."
    mlp_free(buffer)

    allocated = get_allocated_bytes()
    print "Bytes allocated after free: "
    print allocated

    print ""
    return 0
end

-- ==================== Section 2: Multiple Allocations ====================

function test_multiple_allocations() then
    print "=== Multiple Allocations Test ==="

    print "Allocating 3 buffers..."
    int buf1 = mlp_malloc(100)
    int buf2 = mlp_malloc(200)
    int buf3 = mlp_malloc(300)

    assert(buf1 != 0, "Failed to allocate buf1")
    assert(buf2 != 0, "Failed to allocate buf2")
    assert(buf3 != 0, "Failed to allocate buf3")

    int allocated = get_allocated_bytes()
    print "Total bytes allocated: "
    print allocated

    print "Freeing middle buffer..."
    mlp_free(buf2)

    allocated = get_allocated_bytes()
    print "Bytes after freeing buf2: "
    print allocated

    print "Freeing remaining buffers..."
    mlp_free(buf1)
    mlp_free(buf3)

    allocated = get_allocated_bytes()
    print "Bytes after freeing all: "
    print allocated

    print ""
    return 0
end

-- ==================== Section 3: calloc (Zeroed Memory) ====================

function test_calloc() then
    print "=== Calloc Test ==="

    print "Allocating array of 10 elements (8 bytes each)..."
    int array = mlp_calloc(10, 8)
    assert(array != 0, "Failed to allocate array")
    print "Array allocated successfully (zeroed)"

    int allocated = get_allocated_bytes()
    print "Bytes allocated: "
    print allocated

    mlp_free(array)
    print "Array freed"

    print ""
    return 0
end

-- ==================== Section 4: realloc (Growing Buffer) ====================

function test_realloc() then
    print "=== Realloc Test ==="

    print "Allocating 128 bytes..."
    int buffer = mlp_malloc(128)
    assert(buffer != 0, "Failed to allocate buffer")

    int allocated = get_allocated_bytes()
    print "Initial allocation: "
    print allocated

    print "Growing to 256 bytes..."
    buffer = mlp_realloc(buffer, 256)
    assert(buffer != 0, "Failed to realloc buffer")

    allocated = get_allocated_bytes()
    print "After realloc: "
    print allocated

    print "Growing to 512 bytes..."
    buffer = mlp_realloc(buffer, 512)
    assert(buffer != 0, "Failed to realloc buffer")

    allocated = get_allocated_bytes()
    print "After second realloc: "
    print allocated

    mlp_free(buffer)
    print "Buffer freed"

    print ""
    return 0
end

-- ==================== Section 5: Dynamic String Builder ====================

function test_string_builder() then
    print "=== Dynamic String Builder Test ==="

    int capacity = 100
    int buffer = mlp_malloc(capacity)
    assert(buffer != 0, "Failed to allocate string buffer")
    print "Allocated initial buffer: 100 bytes"

    -- Simulate adding data
    int used = 50
    print "Used 50 bytes of buffer"

    -- Need more space
    if used + 60 > capacity then
        print "Buffer too small, growing..."
        capacity = capacity * 2
        buffer = mlp_realloc(buffer, capacity)
        assert(buffer != 0, "Failed to grow buffer")
        print "Grown to: "
        print capacity
        print " bytes"
    end

    mlp_free(buffer)
    print "Buffer freed"

    print ""
    return 0
end

-- ==================== Section 6: Memory Leak Detection ====================

function test_memory_leaks_clean() then
    print "=== Memory Leak Detection (Clean) ==="

    int buf1 = mlp_malloc(100)
    int buf2 = mlp_malloc(200)

    mlp_free(buf1)
    mlp_free(buf2)

    int leaks = check_memory_leaks()
    if leaks == 0 then
        print "No memory leaks detected!"
    end

    print ""
    return 0
end

function test_memory_leaks_with_leak() then
    print "=== Memory Leak Detection (With Leak) ==="

    int buf1 = mlp_malloc(100)
    int buf2 = mlp_malloc(200)

    -- Intentionally forget to free buf2
    mlp_free(buf1)

    print "Checking for leaks (buf2 not freed)..."
    int leaks = check_memory_leaks()
    print "Leak count: "
    print leaks

    -- Clean up for rest of demo
    mlp_free(buf2)

    print ""
    return 0
end

-- ==================== Section 7: Compiler-like Usage ====================

function simulate_ast_allocation() then
    print "=== Simulating Compiler AST Allocation ==="

    -- Allocate nodes
    print "Allocating AST nodes..."
    int node1 = mlp_malloc(64)  -- Binary expression
    int node2 = mlp_malloc(64)  -- Function call
    int node3 = mlp_malloc(64)  -- If statement

    assert(node1 != 0, "Failed to allocate node1")
    assert(node2 != 0, "Failed to allocate node2")
    assert(node3 != 0, "Failed to allocate node3")

    int allocated = get_allocated_bytes()
    print "AST nodes allocated, total bytes: "
    print allocated

    -- Simulate processing
    print "Processing AST..."

    -- Free nodes after processing
    print "Freeing AST nodes..."
    mlp_free(node1)
    mlp_free(node2)
    mlp_free(node3)

    allocated = get_allocated_bytes()
    print "After freeing, bytes: "
    print allocated

    print ""
    return 0
end

-- ==================== Main Tests ====================

test_basic_allocation()
test_multiple_allocations()
test_calloc()
test_realloc()
test_string_builder()
test_memory_leaks_clean()
test_memory_leaks_with_leak()
simulate_ast_allocation()

-- Final leak check
print "===================================="
print "Final Memory Leak Check"
print "===================================="
int final_leaks = check_memory_leaks()
if final_leaks == 0 then
    print "SUCCESS: No memory leaks!"
else
    print "WARNING: Memory leaks detected!"
end

print ""
print "===================================="
print "All Memory Management Tests Completed!"
print "===================================="
```

**Deliverable**: `examples/memory_management_demo.mlp` (~250 lines)

---

## 📋 Part 4: API Reference Updates

### 4.1 Update API_REFERENCE.md

Add Phase 5.2 functions to API reference (~100 lines total additions):

**Section to add**: Phase 5.2 Functions

```markdown
## Phase 5.2: Error Handling & Memory Management

### Error Handling (5 functions)

#### exit_with_code(code)
Exit program with specific return code.
- **Parameters**: `code` (int) - Exit code
- **Returns**: Never returns (program exits)
- **Example**: `exit_with_code(1)`

#### panic(message)
Print error message to stderr and exit with code 1.
- **Parameters**: `message` (string) - Error message
- **Returns**: Never returns (program exits)
- **Example**: `panic("Critical error occurred")`

#### assert(condition, message)
Check condition, panic if false.
- **Parameters**:
  - `condition` (int) - Condition to check
  - `message` (string) - Error message if fails
- **Returns**: void
- **Example**: `assert(x > 0, "x must be positive")`

#### get_error_code()
Get last error code.
- **Returns**: int - Last error code
- **Example**: `int code = get_error_code()`

#### set_error_code(code)
Set error code.
- **Parameters**: `code` (int) - Error code to set
- **Returns**: void
- **Example**: `set_error_code(404)`

### Memory Management (6 functions)

#### mlp_malloc(size)
Allocate memory with tracking.
- **Parameters**: `size` (int) - Bytes to allocate
- **Returns**: pointer - Allocated memory (0 on failure)
- **Example**: `int buf = mlp_malloc(256)`

#### mlp_free(ptr)
Free allocated memory.
- **Parameters**: `ptr` (pointer) - Memory to free
- **Returns**: void
- **Example**: `mlp_free(buf)`

#### mlp_realloc(ptr, size)
Resize allocated memory.
- **Parameters**:
  - `ptr` (pointer) - Existing memory
  - `size` (int) - New size
- **Returns**: pointer - Resized memory
- **Example**: `buf = mlp_realloc(buf, 512)`

#### mlp_calloc(count, size)
Allocate zeroed memory.
- **Parameters**:
  - `count` (int) - Number of elements
  - `size` (int) - Size per element
- **Returns**: pointer - Allocated zeroed memory
- **Example**: `int arr = mlp_calloc(10, 8)`

#### get_allocated_bytes()
Get total allocated memory.
- **Returns**: int - Bytes currently allocated
- **Example**: `int total = get_allocated_bytes()`

#### check_memory_leaks()
Check for memory leaks.
- **Returns**: int - Number of leaked blocks
- **Example**: `int leaks = check_memory_leaks()`

### System Utilities (3 functions)

#### get_env(name)
Get environment variable.
- **Parameters**: `name` (string) - Variable name
- **Returns**: string - Value (empty if not found)
- **Example**: `string home = get_env("HOME")`

#### current_timestamp()
Get current Unix timestamp.
- **Returns**: int - Seconds since epoch
- **Example**: `int now = current_timestamp()`

#### sleep_ms(milliseconds)
Sleep for milliseconds.
- **Parameters**: `milliseconds` (int) - Time to sleep
- **Returns**: void
- **Example**: `sleep_ms(1000)`
```

**Deliverable**: Updated `API_REFERENCE.md` (+100 lines)

---

## 📋 Part 5: Roadmap Updates

### 5.1 Update ROADMAP.md

Mark Phase 5.2 progress:

```markdown
### Phase 5: Self-Hosting Preparation 🔄
**Goal:** Implement remaining features needed for self-hosting

**Planned Features:**
- ✅ Command-line argument parsing (Phase 5.1)
- ✅ Error handling improvements (Phase 5.2)
- ✅ Better memory management (Phase 5.2)
- ❌ Module system (import/export)
- ❌ Preprocessor macros
- ❌ Conditional compilation
- ❌ Floating-point support
- ❌ Standard library organization

**Completion**: ~37% (3/8 features)
```

**Deliverable**: Updated `ROADMAP.md` (+20 lines)

---

## 📂 File Summary

Your deliverables:
```
/home/user/MLP/
├── docs/
│   ├── ERROR_HANDLING_DESIGN.md               (NEW: ~600 lines)
│   └── MEMORY_MANAGEMENT_DESIGN.md            (NEW: ~700 lines)
├── examples/
│   ├── error_handling_demo.mlp                (NEW: ~200 lines)
│   └── memory_management_demo.mlp             (NEW: ~250 lines)
├── API_REFERENCE.md                           (MODIFIED: +100 lines)
└── ROADMAP.md                                 (MODIFIED: +20 lines)
```

**Total**: ~1,870 lines of documentation

---

## ✅ Success Criteria

1. ✅ Design documents are comprehensive and clear
2. ✅ Examples demonstrate all features
3. ✅ API reference is complete and accurate
4. ✅ Documentation follows existing style
5. ✅ All functions are well-documented
6. ✅ Code examples compile and run

---

## 🔧 Git Workflow

```bash
git checkout -b claude/phase5-2-docs-[SESSION_ID]

# Step 1: Design documents
git add docs/ERROR_HANDLING_DESIGN.md
git commit -m "docs(phase5.2): Add error handling design specification"

git add docs/MEMORY_MANAGEMENT_DESIGN.md
git commit -m "docs(phase5.2): Add memory management design specification"

# Step 2: Examples
git add examples/error_handling_demo.mlp
git commit -m "docs(phase5.2): Add error handling example program"

git add examples/memory_management_demo.mlp
git commit -m "docs(phase5.2): Add memory management example program"

# Step 3: API updates
git add API_REFERENCE.md
git commit -m "docs(phase5.2): Update API reference for Phase 5.2"

git add ROADMAP.md
git commit -m "docs(phase5.2): Update roadmap progress"

# Push to remote
git push -u origin claude/phase5-2-docs-[SESSION_ID]
```

---

## 🎯 Implementation Order

**Day 1-2**: Design documents
- ERROR_HANDLING_DESIGN.md
- MEMORY_MANAGEMENT_DESIGN.md

**Day 3-4**: Example programs
- error_handling_demo.mlp
- memory_management_demo.mlp

**Day 5-6**: API updates
- API_REFERENCE.md updates
- ROADMAP.md updates

**Day 7**: Review and polish
- Proofread all documentation
- Test example programs (if implementation ready)
- Fix any issues

---

## 💡 Tips

1. **Be comprehensive**: Include all details developers need
2. **Use examples**: Show, don't just tell
3. **Be consistent**: Follow existing documentation style
4. **Think ahead**: Consider future enhancements
5. **Cross-reference**: Link related documents

---

## 🎉 When You're Done

Report:
1. Design documents complete (2/2)
2. Example programs complete (2/2)
3. API reference updated
4. Roadmap updated
5. Branch pushed

**Branch**: `claude/phase5-2-docs-[SESSION_ID]`
**Expected**: 6 commits

Let's document Phase 5.2! 📚
