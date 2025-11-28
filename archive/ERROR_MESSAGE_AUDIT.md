# Error Message Audit & Standardization - Phase 8.10

## Current Error Messages Analysis

### Summary Statistics
- **Total Error Messages:** 12 unique messages
- **Consistency Score:** 75% (9/12 follow "Failed to..." pattern)
- **Status:** ⚠️ Needs Minor Standardization

---

## Error Message Categories

### Category 1: File I/O Errors (5 messages)

| Line | Current Message | Pattern | Status |
|------|----------------|---------|--------|
| 504 | `"File not found"` | Simple statement | ⚠️ Inconsistent |
| 530 | `"Cannot write file"` | Simple statement | ⚠️ Inconsistent |
| 663 | `"Failed to open file"` | Failed to... | ✅ Good |
| 736 | `"Failed to open file for writing"` | Failed to... | ✅ Good |
| 749 | `"Failed to write complete content"` | Failed to... | ✅ Good |

**Analysis:**
- 3/5 use "Failed to..." pattern (consistent)
- 2/5 use simple statements (inconsistent)
- "File not found" vs "Failed to open file" - same error, different messages

**Recommendation:**
```c
// BEFORE:
promise_reject(p, "File not found");               // Line 504
promise_reject(p, "Cannot write file");            // Line 530

// AFTER (Standardized):
promise_reject(p, "Failed to read file");          // Line 504
promise_reject(p, "Failed to write file");         // Line 530
```

---

### Category 2: Thread Creation Errors (4 messages)

| Line | Current Message | Pattern | Status |
|------|----------------|---------|--------|
| 643 | `"Failed to create sleep thread"` | Failed to... | ✅ Good |
| 714 | `"Failed to create read thread"` | Failed to... | ✅ Good |
| 780 | `"Failed to create write thread"` | Failed to... | ✅ Good |
| 880 | `"Failed to create HTTP thread"` | Failed to... | ✅ Good |

**Analysis:**
- 4/4 use consistent "Failed to create X thread" pattern
- Clear, descriptive, uniform

**Recommendation:** ✅ **No changes needed** - already excellent

---

### Category 3: Network/HTTP Errors (1 message)

| Line | Current Message | Pattern | Status |
|------|----------------|---------|--------|
| 826 | `"Failed to initialize CURL"` | Failed to... | ✅ Good |

**Analysis:**
- Consistent with other error messages
- Clear and technical

**Recommendation:** ✅ **No changes needed**

---

### Category 4: Memory Errors (1 message)

| Line | Current Message | Pattern | Status |
|------|----------------|---------|--------|
| 677 | `"Memory allocation failed"` | Noun-first | ⚠️ Minor inconsistency |

**Analysis:**
- Only memory error in codebase
- Uses passive voice ("allocation failed") vs active ("Failed to allocate")
- Still clear and understandable

**Recommendation (Optional):**
```c
// BEFORE:
promise_reject(args->promise, "Memory allocation failed");

// AFTER (Consistent with other errors):
promise_reject(args->promise, "Failed to allocate memory");
```

---

### Category 5: Timeout Errors (1 message)

| Line | Current Message | Pattern | Status |
|------|----------------|---------|--------|
| 1256 | `"Timeout: 0ms"` | Custom format | ⚠️ Inconsistent |

**Analysis:**
- Unique format: "Timeout: Xms"
- Different from all other errors
- Not a failure message - intentional immediate rejection

**Recommendation:**
```c
// BEFORE:
promise_reject(result, "Timeout: 0ms");

// AFTER (Option 1 - Descriptive):
promise_reject(result, "Operation timed out immediately");

// AFTER (Option 2 - Keep informative):
promise_reject(result, "Timeout expired (0ms)");

// AFTER (Option 3 - Consistent with others):
promise_reject(result, "Failed: timeout expired immediately");
```

---

## Standardization Guidelines

### Recommended Pattern

**Primary Pattern:** `"Failed to <action> <object>"`

**Examples:**
- ✅ `"Failed to create sleep thread"`
- ✅ `"Failed to open file for writing"`
- ✅ `"Failed to initialize CURL"`
- ✅ `"Failed to allocate memory"`

### Secondary Pattern (Acceptable)

**Alternative:** `"<Object> <action> failed"`

**Examples:**
- ✅ `"Memory allocation failed"` (currently used)
- ✅ `"File operation failed"`

### Anti-Patterns (Avoid)

❌ Simple statements without context:
- `"File not found"` → Should be `"Failed to read file"` or `"Failed to open file"`
- `"Cannot write file"` → Should be `"Failed to write file"`

❌ Inconsistent formats:
- `"Timeout: 0ms"` → Should follow failed/error pattern

---

## Proposed Changes

### High Priority (Consistency)

#### Change 1: File Read Error (Line 504)
```c
// File: simple_runtime.c
// Function: async_read_file

// BEFORE:
if (!file) {
    promise_reject(p, "File not found");
    free(p);
    return NULL;
}

// AFTER:
if (!file) {
    promise_reject(p, "Failed to read file");  // More accurate - covers not found AND permission errors
    free(p);
    return NULL;
}
```

**Rationale:** 
- "File not found" is specific errno, but fopen() can fail for many reasons
- "Failed to read file" is more accurate and consistent with other file operations

---

#### Change 2: File Write Error (Line 530)
```c
// File: simple_runtime.c
// Function: async_write_file

// BEFORE:
if (!file) {
    promise_reject(p, "Cannot write file");
    free(p);
    return NULL;
}

// AFTER:
if (!file) {
    promise_reject(p, "Failed to write file");  // Consistent with "Failed to..." pattern
    free(p);
    return NULL;
}
```

**Rationale:**
- "Cannot write" implies permission issue specifically
- "Failed to write" is more general and consistent with threading/HTTP errors

---

### Medium Priority (Clarity)

#### Change 3: Memory Allocation (Line 677)
```c
// File: simple_runtime.c
// Function: async_read_file_thread

// BEFORE:
if (!content) {
    fclose(file);
    promise_reject(args->promise, "Memory allocation failed");
    free(args);
    return NULL;
}

// AFTER (Optional - for consistency):
if (!content) {
    fclose(file);
    promise_reject(args->promise, "Failed to allocate memory");
    free(args);
    return NULL;
}
```

**Rationale:**
- Makes all error messages follow "Failed to..." pattern
- **Optional:** Current version is also clear

---

#### Change 4: Timeout Message (Line 1256)
```c
// File: simple_runtime.c
// Function: async_timeout

// BEFORE:
if (milliseconds <= 0) {
    Promise* result = promise_create();
    promise_reject(result, "Timeout: 0ms");
    return result;
}

// AFTER (Recommended):
if (milliseconds <= 0) {
    Promise* result = promise_create();
    promise_reject(result, "Operation timed out immediately");
    return result;
}

// ALTERNATIVE (More informative):
if (milliseconds <= 0) {
    Promise* result = promise_create();
    char error_msg[64];
    snprintf(error_msg, sizeof(error_msg), "Timeout expired (%dms)", milliseconds);
    promise_reject(result, error_msg);
    return result;
}
```

**Rationale:**
- First option: Consistent with other error messages
- Second option: Preserves timeout value information
- **Consideration:** Second option requires dynamic string allocation (may leak - see MEMORY_LEAK_REPORT.md)

---

## Implementation Summary

### Changes to Apply

| Line | Function | Current | Proposed | Priority |
|------|----------|---------|----------|----------|
| 504 | async_read_file | `"File not found"` | `"Failed to read file"` | HIGH |
| 530 | async_write_file | `"Cannot write file"` | `"Failed to write file"` | HIGH |
| 677 | async_read_file_thread | `"Memory allocation failed"` | `"Failed to allocate memory"` | MEDIUM |
| 1256 | async_timeout | `"Timeout: 0ms"` | `"Operation timed out immediately"` | MEDIUM |

### No Changes Needed (Already Consistent)

✅ Lines 643, 663, 714, 736, 749, 780, 826, 880 - All follow "Failed to..." pattern

---

## Error Code System (Future Enhancement)

### Recommendation for Phase 9+

Consider adding error codes for programmatic error handling:

```c
typedef enum {
    PROMISE_ERROR_FILE_NOT_FOUND = 1001,
    PROMISE_ERROR_FILE_READ_FAILED = 1002,
    PROMISE_ERROR_FILE_WRITE_FAILED = 1003,
    PROMISE_ERROR_MEMORY_ALLOC = 2001,
    PROMISE_ERROR_THREAD_CREATE = 3001,
    PROMISE_ERROR_NETWORK_INIT = 4001,
    PROMISE_ERROR_NETWORK_REQUEST = 4002,
    PROMISE_ERROR_TIMEOUT = 5001,
} PromiseErrorCode;

// Enhanced reject function:
void promise_reject_with_code(Promise* p, const char* message, PromiseErrorCode code) {
    if (!p) return;
    p->error_message = strdup(message);
    p->error_code = code;
    p->state = PROMISE_REJECTED;
}
```

**Benefits:**
- Programmatic error handling in MLP code
- Better debugging and logging
- Error categorization for monitoring

**Example Usage in MLP:**
```mlp
task = async_read_file("data.txt")
result = promise_await_safe(task)
if result == -1 then
    error_code = promise_get_error_code(task)
    if error_code == 1001 then
        print("File not found - using default")
    else
        print("Unknown file error")
    end
end
```

---

## Testing Validation

After applying changes, verify error messages appear correctly:

### Test 1: File Errors
```bash
# Create test that tries to read non-existent file
echo 'function main()
    p = async_read_file("/nonexistent/path.txt")
    status = promise_await_safe(p)
    if promise_is_rejected(p) then
        error = promise_get_error(p)
        print(error)  # Should print: "Failed to read file"
    end
end' > test_file_error.mlp

# Compile and run
./mlpc test_file_error.mlp test_file_error.asm
# ... compile and run
```

### Test 2: Timeout Errors
```bash
# Test immediate timeout (0ms)
echo 'function main()
    slow = async_sleep(5000)
    timeout_wrapper = async_timeout(slow, 0)
    status = promise_await_safe(timeout_wrapper)
    if promise_is_rejected(timeout_wrapper) then
        error = promise_get_error(timeout_wrapper)
        print(error)  # Should print: "Operation timed out immediately"
    end
end' > test_timeout_error.mlp
```

---

## Conclusion

**Current State:** ✅ **GOOD** - 75% consistency already achieved

**Required Changes:** 4 error messages (2 high priority, 2 medium priority)

**Impact:** Low - simple string replacements, no logic changes

**Time Estimate:** 15 minutes implementation + 15 minutes testing = **30 minutes total**

**Next Steps:**
1. Apply 4 changes to simple_runtime.c
2. Recompile runtime
3. Run existing tests to verify no regressions
4. Create error message tests (optional)
5. Mark Task 3 complete

**Future Work (Phase 9+):**
- Add error code enum system
- Create error code getter function
- Document error codes in API_REFERENCE.md
