# Phase 5.2 - Implementation Brief (Claude #1)

**Session**: Phase 5.2 - Error Handling & Memory Management
**Your Role**: Core Implementation
**Branch**: `claude/phase5-2-implementation-[SESSION_ID]`
**Timeline**: ~2 weeks

---

## 🎯 Your Mission

Implement Phase 5.2 features for self-hosting preparation:
1. Error handling functions (5 functions)
2. Memory management functions (6 functions)
3. System utility functions (3 functions)
4. Full compiler integration (lexer, parser, codegen)

**Total Expected**: ~450 lines of code

---

## 📋 Part 1: Error Handling Functions

### 1.1 Runtime Functions (runtime/runtime.c)

Add 5 error handling functions:

```c
// Global error code storage
static int mlp_last_error_code = 0;

/**
 * exit_with_code - Exit program with specific return code
 * @param code: Exit code (0 = success, non-zero = error)
 *
 * MLP Usage: exit_with_code(1);
 */
void exit_with_code(long code) {
    exit((int)code);
}

/**
 * panic - Print error message to stderr and exit with code 1
 * @param message: Error message to print
 *
 * MLP Usage: panic("Fatal error: division by zero");
 */
void panic(const char* message) {
    if (message) {
        fprintf(stderr, "PANIC: %s\n", message);
    } else {
        fprintf(stderr, "PANIC: Unknown error\n");
    }
    exit(1);
}

/**
 * mlp_assert - Check condition, panic if false
 * @param condition: Condition to check (0 = false, non-zero = true)
 * @param message: Error message if condition fails
 *
 * MLP Usage: mlp_assert(x > 0, "x must be positive");
 */
void mlp_assert(long condition, const char* message) {
    if (!condition) {
        if (message) {
            fprintf(stderr, "ASSERTION FAILED: %s\n", message);
        } else {
            fprintf(stderr, "ASSERTION FAILED\n");
        }
        exit(1);
    }
}

/**
 * get_error_code - Get last error code
 * @return: Last error code set
 *
 * MLP Usage: int code = get_error_code();
 */
long get_error_code(void) {
    return mlp_last_error_code;
}

/**
 * set_error_code - Set error code
 * @param code: Error code to set
 *
 * MLP Usage: set_error_code(42);
 */
void set_error_code(long code) {
    mlp_last_error_code = code;
}
```

**Estimated**: ~80 lines

---

## 📋 Part 2: Memory Management Functions

### 2.1 Memory Tracking System

Add memory tracking infrastructure:

```c
// Memory tracking structure
typedef struct MemoryBlock {
    void* ptr;
    size_t size;
    struct MemoryBlock* next;
} MemoryBlock;

static MemoryBlock* memory_list_head = NULL;
static size_t total_allocated = 0;
static size_t total_freed = 0;

/**
 * track_allocation - Add allocation to tracking list
 */
static void track_allocation(void* ptr, size_t size) {
    if (!ptr) return;

    MemoryBlock* block = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    if (!block) return;  // Tracking failure shouldn't crash program

    block->ptr = ptr;
    block->size = size;
    block->next = memory_list_head;
    memory_list_head = block;
    total_allocated += size;
}

/**
 * untrack_allocation - Remove allocation from tracking list
 */
static void untrack_allocation(void* ptr) {
    if (!ptr) return;

    MemoryBlock** current = &memory_list_head;
    while (*current) {
        if ((*current)->ptr == ptr) {
            MemoryBlock* to_free = *current;
            total_freed += to_free->size;
            *current = to_free->next;
            free(to_free);
            return;
        }
        current = &(*current)->next;
    }
}
```

### 2.2 Memory Management Functions

```c
/**
 * mlp_malloc - Allocate memory with tracking
 * @param size: Number of bytes to allocate
 * @return: Pointer to allocated memory, NULL on failure
 *
 * MLP Usage: string buffer = mlp_malloc(256);
 */
void* mlp_malloc(long size) {
    if (size <= 0) return NULL;

    void* ptr = malloc((size_t)size);
    if (ptr) {
        track_allocation(ptr, (size_t)size);
    }
    return ptr;
}

/**
 * mlp_free - Free memory with tracking
 * @param ptr: Pointer to memory to free
 *
 * MLP Usage: mlp_free(buffer);
 */
void mlp_free(void* ptr) {
    if (!ptr) return;

    untrack_allocation(ptr);
    free(ptr);
}

/**
 * mlp_realloc - Reallocate memory with tracking
 * @param ptr: Pointer to existing memory (or NULL)
 * @param size: New size in bytes
 * @return: Pointer to reallocated memory
 *
 * MLP Usage: buffer = mlp_realloc(buffer, 512);
 */
void* mlp_realloc(void* ptr, long size) {
    if (size <= 0) {
        mlp_free(ptr);
        return NULL;
    }

    if (!ptr) {
        return mlp_malloc(size);
    }

    // Find old size for tracking
    size_t old_size = 0;
    MemoryBlock* current = memory_list_head;
    while (current) {
        if (current->ptr == ptr) {
            old_size = current->size;
            break;
        }
        current = current->next;
    }

    void* new_ptr = realloc(ptr, (size_t)size);
    if (new_ptr) {
        untrack_allocation(ptr);
        track_allocation(new_ptr, (size_t)size);
    }
    return new_ptr;
}

/**
 * mlp_calloc - Allocate zeroed memory with tracking
 * @param count: Number of elements
 * @param size: Size of each element
 * @return: Pointer to allocated zeroed memory
 *
 * MLP Usage: int array = mlp_calloc(10, 8);  // 10 x 8 bytes
 */
void* mlp_calloc(long count, long size) {
    if (count <= 0 || size <= 0) return NULL;

    void* ptr = calloc((size_t)count, (size_t)size);
    if (ptr) {
        track_allocation(ptr, (size_t)(count * size));
    }
    return ptr;
}

/**
 * get_allocated_bytes - Get total allocated memory
 * @return: Total bytes currently allocated
 *
 * MLP Usage: int total = get_allocated_bytes();
 */
long get_allocated_bytes(void) {
    return (long)(total_allocated - total_freed);
}

/**
 * check_memory_leaks - Check for memory leaks and report
 * @return: Number of leaked blocks
 *
 * MLP Usage: int leaks = check_memory_leaks();
 */
long check_memory_leaks(void) {
    long leak_count = 0;
    size_t leaked_bytes = 0;

    MemoryBlock* current = memory_list_head;
    while (current) {
        leak_count++;
        leaked_bytes += current->size;
        fprintf(stderr, "MEMORY LEAK: %zu bytes at %p\n",
                current->size, current->ptr);
        current = current->next;
    }

    if (leak_count > 0) {
        fprintf(stderr, "TOTAL LEAKS: %ld blocks, %zu bytes\n",
                leak_count, leaked_bytes);
    }

    return leak_count;
}
```

**Estimated**: ~150 lines

---

## 📋 Part 3: System Utility Functions

### 3.1 Runtime Functions (runtime/runtime.c)

```c
#include <time.h>

/**
 * get_env - Get environment variable value
 * @param name: Environment variable name
 * @return: Value string, or empty string if not found
 *
 * MLP Usage: string path = get_env("PATH");
 */
char* get_env(const char* name) {
    if (!name) return strdup("");

    char* value = getenv(name);
    if (!value) return strdup("");

    return strdup(value);
}

/**
 * current_timestamp - Get current Unix timestamp
 * @return: Current time in seconds since epoch
 *
 * MLP Usage: int now = current_timestamp();
 */
long current_timestamp(void) {
    return (long)time(NULL);
}

/**
 * sleep_ms - Sleep for milliseconds
 * @param milliseconds: Time to sleep in milliseconds
 *
 * MLP Usage: sleep_ms(1000);  // Sleep 1 second
 */
void sleep_ms(long milliseconds) {
    if (milliseconds <= 0) return;

    #ifdef _WIN32
        Sleep((DWORD)milliseconds);
    #else
        usleep((useconds_t)(milliseconds * 1000));
    #endif
}
```

**Estimated**: ~40 lines

---

## 📋 Part 4: Compiler Integration

### 4.1 Lexer Updates (c_compiler/c_lexer.h + c_lexer.c)

Add tokens:
```c
// In c_lexer.h
typedef enum {
    ...
    // Phase 5.2: Error Handling
    TOKEN_BUILTIN_EXIT_WITH_CODE,
    TOKEN_BUILTIN_PANIC,
    TOKEN_BUILTIN_ASSERT,
    TOKEN_BUILTIN_GET_ERROR_CODE,
    TOKEN_BUILTIN_SET_ERROR_CODE,

    // Phase 5.2: Memory Management
    TOKEN_BUILTIN_MLP_MALLOC,
    TOKEN_BUILTIN_MLP_FREE,
    TOKEN_BUILTIN_MLP_REALLOC,
    TOKEN_BUILTIN_MLP_CALLOC,
    TOKEN_BUILTIN_GET_ALLOCATED_BYTES,
    TOKEN_BUILTIN_CHECK_MEMORY_LEAKS,

    // Phase 5.2: System Utilities
    TOKEN_BUILTIN_GET_ENV,
    TOKEN_BUILTIN_CURRENT_TIMESTAMP,
    TOKEN_BUILTIN_SLEEP_MS,
    ...
} TokenType;
```

Add keyword mappings in `c_lexer.c`:
```c
// Phase 5.2: Error Handling
{"exit_with_code",        TOKEN_BUILTIN_EXIT_WITH_CODE},
{"panic",                 TOKEN_BUILTIN_PANIC},
{"assert",                TOKEN_BUILTIN_ASSERT},
{"get_error_code",        TOKEN_BUILTIN_GET_ERROR_CODE},
{"set_error_code",        TOKEN_BUILTIN_SET_ERROR_CODE},

// Phase 5.2: Memory Management
{"mlp_malloc",            TOKEN_BUILTIN_MLP_MALLOC},
{"mlp_free",              TOKEN_BUILTIN_MLP_FREE},
{"mlp_realloc",           TOKEN_BUILTIN_MLP_REALLOC},
{"mlp_calloc",            TOKEN_BUILTIN_MLP_CALLOC},
{"get_allocated_bytes",   TOKEN_BUILTIN_GET_ALLOCATED_BYTES},
{"check_memory_leaks",    TOKEN_BUILTIN_CHECK_MEMORY_LEAKS},

// Phase 5.2: System Utilities
{"get_env",               TOKEN_BUILTIN_GET_ENV},
{"current_timestamp",     TOKEN_BUILTIN_CURRENT_TIMESTAMP},
{"sleep_ms",              TOKEN_BUILTIN_SLEEP_MS},
```

**Estimated**: ~28 lines

---

### 4.2 Parser Updates (c_compiler/c_parser.c)

Add to `birincil()`:
```c
if (current_token->type == TOKEN_BUILTIN_EXIT_WITH_CODE ||
    current_token->type == TOKEN_BUILTIN_PANIC ||
    current_token->type == TOKEN_BUILTIN_ASSERT ||
    current_token->type == TOKEN_BUILTIN_GET_ERROR_CODE ||
    current_token->type == TOKEN_BUILTIN_SET_ERROR_CODE ||
    current_token->type == TOKEN_BUILTIN_MLP_MALLOC ||
    current_token->type == TOKEN_BUILTIN_MLP_FREE ||
    current_token->type == TOKEN_BUILTIN_MLP_REALLOC ||
    current_token->type == TOKEN_BUILTIN_MLP_CALLOC ||
    current_token->type == TOKEN_BUILTIN_GET_ALLOCATED_BYTES ||
    current_token->type == TOKEN_BUILTIN_CHECK_MEMORY_LEAKS ||
    current_token->type == TOKEN_BUILTIN_GET_ENV ||
    current_token->type == TOKEN_BUILTIN_CURRENT_TIMESTAMP ||
    current_token->type == TOKEN_BUILTIN_SLEEP_MS) {

    TokenType func_type = current_token->type;
    consume(current_token->type);
    consume(TOKEN_LEFT_PAREN);

    ASTNode* arg1 = NULL;
    ASTNode* arg2 = NULL;
    ASTNode* arg3 = NULL;

    // Parse arguments based on function
    if (current_token->type != TOKEN_RIGHT_PAREN) {
        arg1 = ifade();

        if (current_token->type == TOKEN_COMMA) {
            consume(TOKEN_COMMA);
            arg2 = ifade();

            if (current_token->type == TOKEN_COMMA) {
                consume(TOKEN_COMMA);
                arg3 = ifade();
            }
        }
    }

    consume(TOKEN_RIGHT_PAREN);
    return createAST_BuiltinCall(func_type, arg1, arg2, arg3);
}
```

**Estimated**: ~35 lines

---

### 4.3 Code Generator Updates (c_compiler/c_generator.c)

Add function name mappings in `visit_BuiltinCall()`:
```c
switch (func) {
    ...
    // Phase 5.2: Error Handling
    case TOKEN_BUILTIN_EXIT_WITH_CODE: func_name = "exit_with_code"; break;
    case TOKEN_BUILTIN_PANIC: func_name = "panic"; break;
    case TOKEN_BUILTIN_ASSERT: func_name = "mlp_assert"; break;
    case TOKEN_BUILTIN_GET_ERROR_CODE: func_name = "get_error_code"; break;
    case TOKEN_BUILTIN_SET_ERROR_CODE: func_name = "set_error_code"; break;

    // Phase 5.2: Memory Management
    case TOKEN_BUILTIN_MLP_MALLOC: func_name = "mlp_malloc"; break;
    case TOKEN_BUILTIN_MLP_FREE: func_name = "mlp_free"; break;
    case TOKEN_BUILTIN_MLP_REALLOC: func_name = "mlp_realloc"; break;
    case TOKEN_BUILTIN_MLP_CALLOC: func_name = "mlp_calloc"; break;
    case TOKEN_BUILTIN_GET_ALLOCATED_BYTES: func_name = "get_allocated_bytes"; break;
    case TOKEN_BUILTIN_CHECK_MEMORY_LEAKS: func_name = "check_memory_leaks"; break;

    // Phase 5.2: System Utilities
    case TOKEN_BUILTIN_GET_ENV: func_name = "get_env"; break;
    case TOKEN_BUILTIN_CURRENT_TIMESTAMP: func_name = "current_timestamp"; break;
    case TOKEN_BUILTIN_SLEEP_MS: func_name = "sleep_ms"; break;
    ...
}
```

**Estimated**: ~20 lines

---

## 📋 Part 5: Integration Test

### 5.1 Create test/test_phase5_2_integration.mlp

```mlp
-- Phase 5.2 Integration Test

function test_error_handling() then
    print "=== Error Handling Test ==="

    -- Test error code functions
    set_error_code(42)
    int code = get_error_code()
    print "Error code: "
    print code

    -- Test assert (should pass)
    assert(1, "This should pass")
    print "Assert passed"

    -- Note: panic and exit_with_code will terminate program
    print "panic() and exit_with_code() tests in separate files"

    return 0
end

function test_memory_management() then
    print "=== Memory Management Test ==="

    -- Test malloc/free
    int buffer = mlp_malloc(256)
    print "Allocated 256 bytes"

    int before = get_allocated_bytes()
    print "Allocated bytes: "
    print before

    mlp_free(buffer)
    print "Freed buffer"

    int after = get_allocated_bytes()
    print "Allocated bytes after free: "
    print after

    -- Test calloc
    int array = mlp_calloc(10, 8)
    print "Allocated 10x8 bytes with calloc"
    mlp_free(array)

    -- Test realloc
    int buf2 = mlp_malloc(100)
    buf2 = mlp_realloc(buf2, 200)
    print "Reallocated from 100 to 200 bytes"
    mlp_free(buf2)

    return 0
end

function test_system_utilities() then
    print "=== System Utilities Test ==="

    -- Test get_env
    string home = get_env("HOME")
    print "HOME env var: "
    print home

    -- Test current_timestamp
    int timestamp = current_timestamp()
    print "Current timestamp: "
    print timestamp

    -- Test sleep_ms
    print "Sleeping for 100ms..."
    sleep_ms(100)
    print "Awake!"

    return 0
end

print "==================================="
print "Phase 5.2 Integration Tests"
print "==================================="
print ""

test_error_handling()
print ""

test_memory_management()
print ""

test_system_utilities()
print ""

-- Check for memory leaks at end
int leaks = check_memory_leaks()
if leaks == 0 then
    print "No memory leaks detected!"
end

print "==================================="
print "All Tests Complete!"
print "==================================="
```

**Estimated**: ~120 lines

---

## 📂 File Summary

Your deliverables:
```
/home/user/MLP/
├── runtime/runtime.c                     (MODIFIED: +270 lines)
├── c_compiler/c_lexer.h                  (MODIFIED: +14 lines)
├── c_compiler/c_lexer.c                  (MODIFIED: +14 lines)
├── c_compiler/c_parser.c                 (MODIFIED: +35 lines)
├── c_compiler/c_generator.c              (MODIFIED: +20 lines)
└── test/test_phase5_2_integration.mlp    (NEW: ~120 lines)
```

**Total**: ~473 lines

---

## ✅ Success Criteria

1. ✅ All 14 runtime functions compile and link
2. ✅ Error handling functions work correctly
3. ✅ Memory tracking accurately reports allocations
4. ✅ Memory leak detection works
5. ✅ System utilities return correct values
6. ✅ Integration test compiles
7. ✅ No breaking changes to existing code

---

## 🔧 Git Workflow

```bash
git checkout -b claude/phase5-2-implementation-[SESSION_ID]

# Step 1: Runtime functions
git add runtime/runtime.c
git commit -m "feat(phase5.2): Add error handling, memory management, and system utilities"

# Step 2: Compiler integration - Lexer
git add c_compiler/c_lexer.h c_compiler/c_lexer.c
git commit -m "feat(phase5.2): Add lexer tokens for Phase 5.2 built-ins"

# Step 3: Compiler integration - Parser
git add c_compiler/c_parser.c
git commit -m "feat(phase5.2): Add parser support for Phase 5.2 built-ins"

# Step 4: Compiler integration - Codegen
git add c_compiler/c_generator.c
git commit -m "feat(phase5.2): Add codegen for Phase 5.2 built-ins"

# Step 5: Integration test
git add test/test_phase5_2_integration.mlp
git commit -m "test(phase5.2): Add integration tests"

# Push to remote
git push -u origin claude/phase5-2-implementation-[SESSION_ID]
```

---

## 🎯 Implementation Order

**Day 1-3**: Runtime functions
- Error handling functions
- Memory management with tracking
- System utilities

**Day 4-6**: Compiler integration
- Lexer tokens
- Parser updates
- Code generation

**Day 7-8**: Testing
- Integration test
- Manual verification
- Bug fixes

**Day 9-10**: Polish
- Code review
- Documentation comments
- Final testing

---

## 💡 Tips

1. **Memory tracking**: Use linked list to track all allocations
2. **Error handling**: fprintf to stderr, not stdout
3. **Testing**: Test each function individually before integration
4. **Safety**: Check all pointers before dereferencing
5. **Portability**: Use #ifdef for platform-specific code (sleep_ms)

---

## 🎉 When You're Done

Report:
1. Functions implemented (X/14)
2. Compiler integration complete
3. Tests passing
4. Branch pushed

**Branch**: `claude/phase5-2-implementation-[SESSION_ID]`
**Expected**: 5-6 commits

Let's implement Phase 5.2! 🚀
