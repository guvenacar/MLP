# Phase 5.2 - Testing Brief (Claude #3)

**Session**: Phase 5.2 - Error Handling & Memory Management
**Your Role**: Testing & Quality Assurance
**Branch**: `claude/phase5-2-testing-[SESSION_ID]`
**Timeline**: ~2 weeks

---

## 🎯 Your Mission

Create comprehensive test suite for Phase 5.2 features:
1. C unit tests for runtime functions (~650 lines)
2. MLP integration tests (~600 lines)
3. Edge case and stress testing (~400 lines)
4. Test documentation (~300 lines)

**Total Expected**: ~1,950 lines of test code

---

## 📋 Part 1: C Unit Tests for Error Handling

### 1.1 Create test/c_unit_tests/test_error_handling.c

**Goal**: Test all 5 error handling functions at the C level.

**Structure** (~300 lines):

```c
/**
 * Phase 5.2 C Unit Tests - Error Handling
 * Tests: exit_with_code, panic, assert, get_error_code, set_error_code
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>

// Forward declarations from runtime.c
extern void exit_with_code(long code);
extern void panic(const char* message);
extern void mlp_assert(long condition, const char* message);
extern long get_error_code(void);
extern void set_error_code(long code);

// Test counter
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    printf("Testing %s... ", name); \
    if (test_##name()) { \
        printf("PASS\n"); \
        tests_passed++; \
    } else { \
        printf("FAIL\n"); \
        tests_failed++; \
    }

// ==================== Error Code Tests ====================

int test_get_set_error_code() {
    // Test initial state
    set_error_code(0);
    long code = get_error_code();
    if (code != 0) return 0;

    // Test setting and getting
    set_error_code(42);
    code = get_error_code();
    if (code != 42) return 0;

    set_error_code(100);
    code = get_error_code();
    if (code != 100) return 0;

    // Test negative values
    set_error_code(-1);
    code = get_error_code();
    if (code != -1) return 0;

    // Reset
    set_error_code(0);
    return 1;
}

// ==================== Assert Tests ====================

int test_assert_pass() {
    // Should not crash
    mlp_assert(1, "This should pass");
    mlp_assert(42, "Non-zero should pass");
    mlp_assert(-1, "Negative should pass");
    return 1;
}

int test_assert_with_null_message() {
    // Should not crash with NULL message
    mlp_assert(1, NULL);
    return 1;
}

// Note: Cannot test assert failure or panic in same process
// These would terminate the program

// ==================== Exit Code Tests (via fork) ====================

int test_exit_with_code_zero() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        exit_with_code(0);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status) == 0;
        }
    }
    return 0;
}

int test_exit_with_code_one() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        exit_with_code(1);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status) == 1;
        }
    }
    return 0;
}

int test_exit_with_code_custom() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        exit_with_code(42);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status) == 42;
        }
    }
    return 0;
}

// ==================== Panic Tests (via fork) ====================

int test_panic_exits_with_one() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process - redirect stderr to avoid output
        freopen("/dev/null", "w", stderr);
        panic("Test panic");
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status) == 1;
        }
    }
    return 0;
}

int test_panic_with_null_message() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        freopen("/dev/null", "w", stderr);
        panic(NULL);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status) == 1;
        }
    }
    return 0;
}

// ==================== Assert Failure Tests (via fork) ====================

int test_assert_failure() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        freopen("/dev/null", "w", stderr);
        mlp_assert(0, "This should fail");
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status) == 1;
        }
    }
    return 0;
}

// ==================== Main ====================

int main() {
    printf("==================================\n");
    printf("Phase 5.2 C Unit Tests - Error Handling\n");
    printf("==================================\n\n");

    // Basic tests
    TEST(get_set_error_code);
    TEST(assert_pass);
    TEST(assert_with_null_message);

    // Exit code tests
    TEST(exit_with_code_zero);
    TEST(exit_with_code_one);
    TEST(exit_with_code_custom);

    // Panic tests
    TEST(panic_exits_with_one);
    TEST(panic_with_null_message);

    // Assert failure tests
    TEST(assert_failure);

    printf("\n==================================\n");
    printf("Results: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("==================================\n");

    return tests_failed > 0 ? 1 : 0;
}
```

**Deliverable**: `test/c_unit_tests/test_error_handling.c` (~300 lines)

---

## 📋 Part 2: C Unit Tests for Memory Management

### 2.1 Create test/c_unit_tests/test_memory_management.c

**Goal**: Test all 6 memory management functions at the C level.

**Structure** (~350 lines):

```c
/**
 * Phase 5.2 C Unit Tests - Memory Management
 * Tests: mlp_malloc, mlp_free, mlp_realloc, mlp_calloc,
 *        get_allocated_bytes, check_memory_leaks
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Forward declarations from runtime.c
extern void* mlp_malloc(long size);
extern void mlp_free(void* ptr);
extern void* mlp_realloc(void* ptr, long size);
extern void* mlp_calloc(long count, long size);
extern long get_allocated_bytes(void);
extern long check_memory_leaks(void);

// Test counter
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    printf("Testing %s... ", name); \
    if (test_##name()) { \
        printf("PASS\n"); \
        tests_passed++; \
    } else { \
        printf("FAIL\n"); \
        tests_failed++; \
    }

// ==================== Malloc Tests ====================

int test_malloc_basic() {
    void* ptr = mlp_malloc(100);
    if (ptr == NULL) return 0;

    long allocated = get_allocated_bytes();
    if (allocated < 100) {
        mlp_free(ptr);
        return 0;
    }

    mlp_free(ptr);
    return 1;
}

int test_malloc_zero() {
    void* ptr = mlp_malloc(0);
    if (ptr != NULL) {
        mlp_free(ptr);
        return 0;
    }
    return 1;
}

int test_malloc_negative() {
    void* ptr = mlp_malloc(-10);
    if (ptr != NULL) {
        mlp_free(ptr);
        return 0;
    }
    return 1;
}

int test_malloc_large() {
    void* ptr = mlp_malloc(1024 * 1024);  // 1MB
    if (ptr == NULL) return 0;

    long allocated = get_allocated_bytes();
    if (allocated < 1024 * 1024) {
        mlp_free(ptr);
        return 0;
    }

    mlp_free(ptr);
    return 1;
}

// ==================== Free Tests ====================

int test_free_basic() {
    void* ptr = mlp_malloc(100);
    if (ptr == NULL) return 0;

    long before = get_allocated_bytes();
    mlp_free(ptr);
    long after = get_allocated_bytes();

    return after < before;
}

int test_free_null() {
    // Should not crash
    mlp_free(NULL);
    return 1;
}

int test_free_multiple() {
    void* ptr1 = mlp_malloc(100);
    void* ptr2 = mlp_malloc(200);
    void* ptr3 = mlp_malloc(300);

    if (!ptr1 || !ptr2 || !ptr3) {
        mlp_free(ptr1);
        mlp_free(ptr2);
        mlp_free(ptr3);
        return 0;
    }

    long before = get_allocated_bytes();
    mlp_free(ptr1);
    mlp_free(ptr2);
    mlp_free(ptr3);
    long after = get_allocated_bytes();

    return after < before;
}

// ==================== Calloc Tests ====================

int test_calloc_basic() {
    void* ptr = mlp_calloc(10, 10);
    if (ptr == NULL) return 0;

    // Check that memory is zeroed
    unsigned char* bytes = (unsigned char*)ptr;
    for (int i = 0; i < 100; i++) {
        if (bytes[i] != 0) {
            mlp_free(ptr);
            return 0;
        }
    }

    mlp_free(ptr);
    return 1;
}

int test_calloc_zero_count() {
    void* ptr = mlp_calloc(0, 10);
    if (ptr != NULL) {
        mlp_free(ptr);
        return 0;
    }
    return 1;
}

int test_calloc_zero_size() {
    void* ptr = mlp_calloc(10, 0);
    if (ptr != NULL) {
        mlp_free(ptr);
        return 0;
    }
    return 1;
}

// ==================== Realloc Tests ====================

int test_realloc_grow() {
    void* ptr = mlp_malloc(100);
    if (ptr == NULL) return 0;

    void* new_ptr = mlp_realloc(ptr, 200);
    if (new_ptr == NULL) {
        mlp_free(ptr);
        return 0;
    }

    long allocated = get_allocated_bytes();
    if (allocated < 200) {
        mlp_free(new_ptr);
        return 0;
    }

    mlp_free(new_ptr);
    return 1;
}

int test_realloc_shrink() {
    void* ptr = mlp_malloc(200);
    if (ptr == NULL) return 0;

    void* new_ptr = mlp_realloc(ptr, 100);
    if (new_ptr == NULL) {
        mlp_free(ptr);
        return 0;
    }

    mlp_free(new_ptr);
    return 1;
}

int test_realloc_null() {
    // Should behave like malloc
    void* ptr = mlp_realloc(NULL, 100);
    if (ptr == NULL) return 0;

    mlp_free(ptr);
    return 1;
}

int test_realloc_zero_size() {
    void* ptr = mlp_malloc(100);
    if (ptr == NULL) return 0;

    // Should behave like free
    void* new_ptr = mlp_realloc(ptr, 0);
    if (new_ptr != NULL) {
        mlp_free(new_ptr);
        return 0;
    }

    return 1;
}

// ==================== Allocation Tracking Tests ====================

int test_get_allocated_bytes_zero() {
    // Assume clean state
    long allocated = get_allocated_bytes();
    return allocated == 0;
}

int test_get_allocated_bytes_after_malloc() {
    long before = get_allocated_bytes();

    void* ptr = mlp_malloc(256);
    if (ptr == NULL) return 0;

    long after = get_allocated_bytes();
    mlp_free(ptr);

    return after > before;
}

int test_get_allocated_bytes_after_free() {
    void* ptr = mlp_malloc(256);
    if (ptr == NULL) return 0;

    long with_alloc = get_allocated_bytes();
    mlp_free(ptr);
    long after_free = get_allocated_bytes();

    return after_free < with_alloc;
}

// ==================== Memory Leak Tests ====================

int test_check_memory_leaks_none() {
    void* ptr = mlp_malloc(100);
    if (ptr == NULL) return 0;

    mlp_free(ptr);

    // Redirect stderr to avoid output
    FILE* old_stderr = stderr;
    stderr = fopen("/dev/null", "w");

    long leaks = check_memory_leaks();

    fclose(stderr);
    stderr = old_stderr;

    return leaks == 0;
}

int test_check_memory_leaks_one() {
    void* ptr = mlp_malloc(100);
    if (ptr == NULL) return 0;

    // Intentionally don't free

    // Redirect stderr
    FILE* old_stderr = stderr;
    stderr = fopen("/dev/null", "w");

    long leaks = check_memory_leaks();

    fclose(stderr);
    stderr = old_stderr;

    // Clean up
    mlp_free(ptr);

    return leaks == 1;
}

int test_check_memory_leaks_multiple() {
    void* ptr1 = mlp_malloc(100);
    void* ptr2 = mlp_malloc(200);
    void* ptr3 = mlp_malloc(300);

    if (!ptr1 || !ptr2 || !ptr3) {
        mlp_free(ptr1);
        mlp_free(ptr2);
        mlp_free(ptr3);
        return 0;
    }

    // Redirect stderr
    FILE* old_stderr = stderr;
    stderr = fopen("/dev/null", "w");

    long leaks = check_memory_leaks();

    fclose(stderr);
    stderr = old_stderr;

    // Clean up
    mlp_free(ptr1);
    mlp_free(ptr2);
    mlp_free(ptr3);

    return leaks == 3;
}

// ==================== Main ====================

int main() {
    printf("==================================\n");
    printf("Phase 5.2 C Unit Tests - Memory Management\n");
    printf("==================================\n\n");

    // Malloc tests
    TEST(malloc_basic);
    TEST(malloc_zero);
    TEST(malloc_negative);
    TEST(malloc_large);

    // Free tests
    TEST(free_basic);
    TEST(free_null);
    TEST(free_multiple);

    // Calloc tests
    TEST(calloc_basic);
    TEST(calloc_zero_count);
    TEST(calloc_zero_size);

    // Realloc tests
    TEST(realloc_grow);
    TEST(realloc_shrink);
    TEST(realloc_null);
    TEST(realloc_zero_size);

    // Tracking tests
    TEST(get_allocated_bytes_zero);
    TEST(get_allocated_bytes_after_malloc);
    TEST(get_allocated_bytes_after_free);

    // Leak detection tests
    TEST(check_memory_leaks_none);
    TEST(check_memory_leaks_one);
    TEST(check_memory_leaks_multiple);

    printf("\n==================================\n");
    printf("Results: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("==================================\n");

    return tests_failed > 0 ? 1 : 0;
}
```

**Deliverable**: `test/c_unit_tests/test_memory_management.c` (~350 lines)

---

## 📋 Part 3: MLP Integration Tests

### 3.1 Create test/test_error_handling.mlp

**Goal**: Test error handling functions in MLP programs.

**Content** (~200 lines):

```mlp
-- Phase 5.2 MLP Integration Tests - Error Handling

print "===================================="
print "Error Handling Integration Tests"
print "===================================="
print ""

-- Test counter
int passed = 0
int failed = 0

-- ==================== Test: Error Code Management ====================

function test_error_codes() then
    print "Test: Error Code Management"

    set_error_code(0)
    int code = get_error_code()
    if code == 0 then
        print "  PASS: Initial error code is 0"
        passed = passed + 1
    else
        print "  FAIL: Initial error code not 0"
        failed = failed + 1
    end

    set_error_code(42)
    code = get_error_code()
    if code == 42 then
        print "  PASS: Error code set to 42"
        passed = passed + 1
    else
        print "  FAIL: Error code not 42"
        failed = failed + 1
    end

    set_error_code(-1)
    code = get_error_code()
    if code == -1 then
        print "  PASS: Negative error codes work"
        passed = passed + 1
    else
        print "  FAIL: Negative error codes don't work"
        failed = failed + 1
    end

    set_error_code(0)
    print ""
    return 0
end

-- ==================== Test: Assertions Pass ====================

function test_assertions_pass() then
    print "Test: Assertions That Pass"

    assert(1, "This should pass")
    print "  PASS: assert(1) passed"
    passed = passed + 1

    assert(42, "Non-zero should pass")
    print "  PASS: assert(42) passed"
    passed = passed + 1

    int x = 10
    assert(x > 0, "x should be positive")
    print "  PASS: assert(x > 0) passed"
    passed = passed + 1

    print ""
    return 0
end

-- ==================== Test: Error Propagation ====================

function inner_function() then
    set_error_code(100)
    return 0
end

function middle_function() then
    int result = inner_function()
    if result == 0 then
        return 0
    end
    return 1
end

function outer_function() then
    int result = middle_function()
    if result == 0 then
        return get_error_code()
    end
    return 0
end

function test_error_propagation() then
    print "Test: Error Propagation"

    int error = outer_function()
    if error == 100 then
        print "  PASS: Error propagated through call stack"
        passed = passed + 1
    else
        print "  FAIL: Error not propagated correctly"
        failed = failed + 1
    end

    set_error_code(0)
    print ""
    return 0
end

-- ==================== Test: Safe Division ====================

function safe_divide(a, b) then
    if b == 0 then
        set_error_code(1)
        return 0
    end
    set_error_code(0)
    return a / b
end

function test_safe_divide() then
    print "Test: Safe Division"

    int result = safe_divide(10, 2)
    int error = get_error_code()
    if error == 0 then
        if result == 5 then
            print "  PASS: 10 / 2 = 5"
            passed = passed + 1
        else
            print "  FAIL: 10 / 2 != 5"
            failed = failed + 1
        end
    else
        print "  FAIL: Error on valid division"
        failed = failed + 1
    end

    result = safe_divide(10, 0)
    error = get_error_code()
    if error != 0 then
        print "  PASS: Division by zero detected"
        passed = passed + 1
    else
        print "  FAIL: Division by zero not detected"
        failed = failed + 1
    end

    set_error_code(0)
    print ""
    return 0
end

-- ==================== Test: Precondition Checks ====================

function checked_array_get(index, max) then
    assert(index >= 0, "Index must be non-negative")
    assert(index < max, "Index must be less than max")
    return index * 10
end

function test_preconditions() then
    print "Test: Precondition Checks"

    int value = checked_array_get(5, 10)
    if value == 50 then
        print "  PASS: Valid array access"
        passed = passed + 1
    else
        print "  FAIL: Invalid result"
        failed = failed + 1
    end

    value = checked_array_get(0, 10)
    if value == 0 then
        print "  PASS: Boundary case (index=0)"
        passed = passed + 1
    else
        print "  FAIL: Boundary case failed"
        failed = failed + 1
    end

    print ""
    return 0
end

-- ==================== Run All Tests ====================

test_error_codes()
test_assertions_pass()
test_error_propagation()
test_safe_divide()
test_preconditions()

print "===================================="
print "Results:"
print "  Passed: "
print passed
print "  Failed: "
print failed
print "===================================="

if failed > 0 then
    exit_with_code(1)
end
exit_with_code(0)
```

**Deliverable**: `test/test_error_handling.mlp` (~200 lines)

---

### 3.2 Create test/test_memory_management.mlp

**Goal**: Test memory management functions in MLP programs.

**Content** (~250 lines):

```mlp
-- Phase 5.2 MLP Integration Tests - Memory Management

print "===================================="
print "Memory Management Integration Tests"
print "===================================="
print ""

int passed = 0
int failed = 0

-- ==================== Test: Basic Malloc/Free ====================

function test_basic_malloc_free() then
    print "Test: Basic Malloc/Free"

    int before = get_allocated_bytes()

    int buffer = mlp_malloc(256)
    if buffer != 0 then
        print "  PASS: malloc(256) succeeded"
        passed = passed + 1
    else
        print "  FAIL: malloc(256) failed"
        failed = failed + 1
        return 0
    end

    int during = get_allocated_bytes()
    if during > before then
        print "  PASS: Allocation tracked"
        passed = passed + 1
    else
        print "  FAIL: Allocation not tracked"
        failed = failed + 1
    end

    mlp_free(buffer)

    int after = get_allocated_bytes()
    if after < during then
        print "  PASS: Free tracked"
        passed = passed + 1
    else
        print "  FAIL: Free not tracked"
        failed = failed + 1
    end

    print ""
    return 0
end

-- ==================== Test: Multiple Allocations ====================

function test_multiple_allocations() then
    print "Test: Multiple Allocations"

    int buf1 = mlp_malloc(100)
    int buf2 = mlp_malloc(200)
    int buf3 = mlp_malloc(300)

    if buf1 != 0 then
        if buf2 != 0 then
            if buf3 != 0 then
                print "  PASS: Multiple allocations succeeded"
                passed = passed + 1
            else
                print "  FAIL: buf3 allocation failed"
                failed = failed + 1
            end
        else
            print "  FAIL: buf2 allocation failed"
            failed = failed + 1
        end
    else
        print "  FAIL: buf1 allocation failed"
        failed = failed + 1
    end

    int total = get_allocated_bytes()
    if total >= 600 then
        print "  PASS: Total allocation >= 600 bytes"
        passed = passed + 1
    else
        print "  FAIL: Total allocation < 600 bytes"
        failed = failed + 1
    end

    mlp_free(buf1)
    mlp_free(buf2)
    mlp_free(buf3)

    print ""
    return 0
end

-- ==================== Test: Calloc (Zeroed Memory) ====================

function test_calloc() then
    print "Test: Calloc (Zeroed Memory)"

    int array = mlp_calloc(10, 8)
    if array != 0 then
        print "  PASS: calloc(10, 8) succeeded"
        passed = passed + 1
    else
        print "  FAIL: calloc(10, 8) failed"
        failed = failed + 1
        return 0
    end

    int total = get_allocated_bytes()
    if total >= 80 then
        print "  PASS: Calloc tracked correctly"
        passed = passed + 1
    else
        print "  FAIL: Calloc not tracked correctly"
        failed = failed + 1
    end

    mlp_free(array)

    print ""
    return 0
end

-- ==================== Test: Realloc (Growing Buffer) ====================

function test_realloc() then
    print "Test: Realloc (Growing Buffer)"

    int buffer = mlp_malloc(128)
    if buffer == 0 then
        print "  FAIL: Initial malloc failed"
        failed = failed + 1
        return 0
    end

    buffer = mlp_realloc(buffer, 256)
    if buffer != 0 then
        print "  PASS: realloc(256) succeeded"
        passed = passed + 1
    else
        print "  FAIL: realloc(256) failed"
        failed = failed + 1
        return 0
    end

    buffer = mlp_realloc(buffer, 512)
    if buffer != 0 then
        print "  PASS: realloc(512) succeeded"
        passed = passed + 1
    else
        print "  FAIL: realloc(512) failed"
        failed = failed + 1
        mlp_free(buffer)
        return 0
    end

    mlp_free(buffer)

    print ""
    return 0
end

-- ==================== Test: Memory Leak Detection ====================

function test_leak_detection_clean() then
    print "Test: Leak Detection (Clean)"

    int buf = mlp_malloc(100)
    mlp_free(buf)

    int leaks = check_memory_leaks()
    if leaks == 0 then
        print "  PASS: No leaks detected"
        passed = passed + 1
    else
        print "  FAIL: False positive leaks"
        failed = failed + 1
    end

    print ""
    return 0
end

function test_leak_detection_with_leak() then
    print "Test: Leak Detection (With Leak)"

    int buf = mlp_malloc(100)
    -- Intentionally don't free

    int leaks = check_memory_leaks()
    if leaks > 0 then
        print "  PASS: Leak detected"
        passed = passed + 1
    else
        print "  FAIL: Leak not detected"
        failed = failed + 1
    end

    -- Clean up
    mlp_free(buf)

    print ""
    return 0
end

-- ==================== Test: Tracking Accuracy ====================

function test_tracking_accuracy() then
    print "Test: Tracking Accuracy"

    int before = get_allocated_bytes()

    int buf1 = mlp_malloc(100)
    int during1 = get_allocated_bytes()
    int diff1 = during1 - before

    if diff1 >= 100 then
        print "  PASS: First allocation tracked"
        passed = passed + 1
    else
        print "  FAIL: First allocation not tracked accurately"
        failed = failed + 1
    end

    int buf2 = mlp_malloc(200)
    int during2 = get_allocated_bytes()
    int diff2 = during2 - during1

    if diff2 >= 200 then
        print "  PASS: Second allocation tracked"
        passed = passed + 1
    else
        print "  FAIL: Second allocation not tracked accurately"
        failed = failed + 1
    end

    mlp_free(buf1)
    mlp_free(buf2)

    int after = get_allocated_bytes()
    if after == before then
        print "  PASS: All memory freed"
        passed = passed + 1
    else
        print "  FAIL: Not all memory freed"
        failed = failed + 1
    end

    print ""
    return 0
end

-- ==================== Run All Tests ====================

test_basic_malloc_free()
test_multiple_allocations()
test_calloc()
test_realloc()
test_leak_detection_clean()
test_leak_detection_with_leak()
test_tracking_accuracy()

print "===================================="
print "Results:"
print "  Passed: "
print passed
print "  Failed: "
print failed
print "===================================="

-- Final cleanup check
int final_leaks = check_memory_leaks()
if final_leaks == 0 then
    print "No memory leaks at program end!"
else
    print "WARNING: Memory leaks detected!"
end

if failed > 0 then
    exit_with_code(1)
end
exit_with_code(0)
```

**Deliverable**: `test/test_memory_management.mlp` (~250 lines)

---

## 📋 Part 4: Edge Cases and Stress Tests

### 4.1 Create test/test_phase5_2_edge_cases.mlp

**Goal**: Test edge cases and boundary conditions.

**Content** (~200 lines):

```mlp
-- Phase 5.2 Edge Cases and Stress Tests

print "===================================="
print "Phase 5.2 Edge Cases and Stress Tests"
print "===================================="
print ""

int passed = 0
int failed = 0

-- ==================== Edge Case: Zero-Size Allocation ====================

function test_zero_size_allocation() then
    print "Test: Zero-Size Allocation"

    int ptr = mlp_malloc(0)
    if ptr == 0 then
        print "  PASS: malloc(0) returns NULL"
        passed = passed + 1
    else
        print "  FAIL: malloc(0) should return NULL"
        mlp_free(ptr)
        failed = failed + 1
    end

    print ""
    return 0
end

-- ==================== Edge Case: Negative Size ====================

function test_negative_size() then
    print "Test: Negative Size"

    int ptr = mlp_malloc(-100)
    if ptr == 0 then
        print "  PASS: malloc(-100) returns NULL"
        passed = passed + 1
    else
        print "  FAIL: malloc(-100) should return NULL"
        mlp_free(ptr)
        failed = failed + 1
    end

    print ""
    return 0
end

-- ==================== Edge Case: Free NULL ====================

function test_free_null() then
    print "Test: Free NULL"

    -- Should not crash
    mlp_free(0)
    print "  PASS: free(NULL) didn't crash"
    passed = passed + 1

    print ""
    return 0
end

-- ==================== Edge Case: Large Allocation ====================

function test_large_allocation() then
    print "Test: Large Allocation (1MB)"

    int large = mlp_malloc(1048576)  -- 1MB
    if large != 0 then
        print "  PASS: Large allocation succeeded"
        passed = passed + 1

        int allocated = get_allocated_bytes()
        if allocated >= 1048576 then
            print "  PASS: Large allocation tracked"
            passed = passed + 1
        else
            print "  FAIL: Large allocation not tracked"
            failed = failed + 1
        end

        mlp_free(large)
    else
        print "  FAIL: Large allocation failed"
        failed = failed + 1
    end

    print ""
    return 0
end

-- ==================== Stress Test: Many Small Allocations ====================

function test_many_small_allocations() then
    print "Test: Many Small Allocations (100 x 16 bytes)"

    int i = 0
    int count = 0
    while i < 100 then
        int ptr = mlp_malloc(16)
        if ptr != 0 then
            count = count + 1
            mlp_free(ptr)
        end
        i = i + 1
    end

    if count == 100 then
        print "  PASS: All 100 allocations succeeded"
        passed = passed + 1
    else
        print "  FAIL: Only "
        print count
        print " allocations succeeded"
        failed = failed + 1
    end

    print ""
    return 0
end

-- ==================== Stress Test: Realloc Chain ====================

function test_realloc_chain() then
    print "Test: Realloc Chain (Growing)"

    int buffer = mlp_malloc(64)
    if buffer == 0 then
        print "  FAIL: Initial allocation failed"
        failed = failed + 1
        return 0
    end

    -- Grow in steps
    buffer = mlp_realloc(buffer, 128)
    if buffer == 0 then
        print "  FAIL: realloc to 128 failed"
        failed = failed + 1
        return 0
    end

    buffer = mlp_realloc(buffer, 256)
    if buffer == 0 then
        print "  FAIL: realloc to 256 failed"
        failed = failed + 1
        return 0
    end

    buffer = mlp_realloc(buffer, 512)
    if buffer == 0 then
        print "  FAIL: realloc to 512 failed"
        failed = failed + 1
        return 0
    end

    print "  PASS: Realloc chain succeeded"
    passed = passed + 1

    mlp_free(buffer)

    print ""
    return 0
end

-- ==================== Edge Case: Error Code Boundaries ====================

function test_error_code_boundaries() then
    print "Test: Error Code Boundaries"

    set_error_code(0)
    if get_error_code() == 0 then
        passed = passed + 1
    else
        failed = failed + 1
    end

    set_error_code(2147483647)  -- Max int
    if get_error_code() == 2147483647 then
        print "  PASS: Large error codes work"
        passed = passed + 1
    else
        print "  FAIL: Large error codes don't work"
        failed = failed + 1
    end

    set_error_code(-2147483648)  -- Min int
    if get_error_code() == -2147483648 then
        print "  PASS: Negative error codes work"
        passed = passed + 1
    else
        print "  FAIL: Negative error codes don't work"
        failed = failed + 1
    end

    set_error_code(0)
    print ""
    return 0
end

-- ==================== Run All Tests ====================

test_zero_size_allocation()
test_negative_size()
test_free_null()
test_large_allocation()
test_many_small_allocations()
test_realloc_chain()
test_error_code_boundaries()

print "===================================="
print "Results:"
print "  Passed: "
print passed
print "  Failed: "
print failed
print "===================================="

int final_leaks = check_memory_leaks()
if final_leaks == 0 then
    print "No memory leaks!"
end

if failed > 0 then
    exit_with_code(1)
end
exit_with_code(0)
```

**Deliverable**: `test/test_phase5_2_edge_cases.mlp` (~200 lines)

---

## 📋 Part 5: Test Documentation

### 5.1 Create test/README_TESTS_PHASE5_2.md

**Goal**: Document all Phase 5.2 tests.

**Content** (~300 lines):

```markdown
# Phase 5.2 Test Suite

**Phase**: Error Handling & Memory Management
**Features**: 14 new functions
**Test Coverage**: 100%
**Last Updated**: [DATE]

---

## Test Summary

| Test Suite | Test Count | Lines of Code | Status |
|------------|-----------|---------------|---------|
| C Unit Tests (Error Handling) | 9 | ~300 | ✅ Pass |
| C Unit Tests (Memory Mgmt) | 21 | ~350 | ✅ Pass |
| MLP Integration (Error) | 5 | ~200 | ✅ Pass |
| MLP Integration (Memory) | 7 | ~250 | ✅ Pass |
| Edge Cases & Stress Tests | 7 | ~200 | ✅ Pass |
| **Total** | **49** | **~1,300** | **✅ Pass** |

---

## Running Tests

### C Unit Tests

```bash
# Compile runtime with test suite
cd test/c_unit_tests
gcc -o test_error_handling test_error_handling.c ../../runtime/runtime.c -I../../runtime
./test_error_handling

gcc -o test_memory_management test_memory_management.c ../../runtime/runtime.c -I../../runtime
./test_memory_management
```

### MLP Integration Tests

```bash
# Compile and run error handling tests
./mlpc test/test_error_handling.mlp -o test_error
./test_error

# Compile and run memory management tests
./mlpc test/test_memory_management.mlp -o test_memory
./test_memory

# Compile and run edge cases
./mlpc test/test_phase5_2_edge_cases.mlp -o test_edge
./test_edge
```

### Run All Tests

```bash
cd test
./run_phase5_2_tests.sh
```

---

## Test Coverage

### Error Handling (5 functions)

| Function | C Tests | MLP Tests | Coverage |
|----------|---------|-----------|----------|
| exit_with_code() | 3 | 1 | 100% |
| panic() | 2 | 0 | 100% |
| assert() | 3 | 5 | 100% |
| get_error_code() | 1 | 8 | 100% |
| set_error_code() | 1 | 8 | 100% |

### Memory Management (6 functions)

| Function | C Tests | MLP Tests | Coverage |
|----------|---------|-----------|----------|
| mlp_malloc() | 4 | 6 | 100% |
| mlp_free() | 3 | 6 | 100% |
| mlp_realloc() | 4 | 3 | 100% |
| mlp_calloc() | 3 | 1 | 100% |
| get_allocated_bytes() | 3 | 5 | 100% |
| check_memory_leaks() | 3 | 3 | 100% |

### System Utilities (3 functions)

| Function | C Tests | MLP Tests | Coverage |
|----------|---------|-----------|----------|
| get_env() | N/A | 0 | 0% |
| current_timestamp() | N/A | 0 | 0% |
| sleep_ms() | N/A | 0 | 0% |

**Note**: System utilities will be tested in integration tests.

---

## Test Details

[... continue with detailed descriptions of each test ...]
```

**Deliverable**: `test/README_TESTS_PHASE5_2.md` (~300 lines)

---

## 📂 File Summary

Your deliverables:
```
/home/user/MLP/test/
├── c_unit_tests/
│   ├── test_error_handling.c              (NEW: ~300 lines)
│   └── test_memory_management.c           (NEW: ~350 lines)
├── test_error_handling.mlp                (NEW: ~200 lines)
├── test_memory_management.mlp             (NEW: ~250 lines)
├── test_phase5_2_edge_cases.mlp           (NEW: ~200 lines)
└── README_TESTS_PHASE5_2.md               (NEW: ~300 lines)
```

**Total**: ~1,600 lines of test code

---

## ✅ Success Criteria

1. ✅ All C unit tests pass (30+ tests)
2. ✅ All MLP integration tests pass (12+ tests)
3. ✅ Edge cases and stress tests pass (7+ tests)
4. ✅ 100% function coverage
5. ✅ Memory leak detection works
6. ✅ Test documentation complete

---

## 🔧 Git Workflow

```bash
git checkout -b claude/phase5-2-testing-[SESSION_ID]

# C unit tests
git add test/c_unit_tests/test_error_handling.c
git commit -m "test(phase5.2): Add C unit tests for error handling"

git add test/c_unit_tests/test_memory_management.c
git commit -m "test(phase5.2): Add C unit tests for memory management"

# MLP integration tests
git add test/test_error_handling.mlp
git commit -m "test(phase5.2): Add MLP integration tests for error handling"

git add test/test_memory_management.mlp
git commit -m "test(phase5.2): Add MLP integration tests for memory management"

# Edge cases
git add test/test_phase5_2_edge_cases.mlp
git commit -m "test(phase5.2): Add edge cases and stress tests"

# Documentation
git add test/README_TESTS_PHASE5_2.md
git commit -m "docs(phase5.2): Add Phase 5.2 test documentation"

# Push
git push -u origin claude/phase5-2-testing-[SESSION_ID]
```

---

## 🎯 Implementation Order

**Day 1-3**: C unit tests
- Error handling tests
- Memory management tests
- Compile and verify

**Day 4-6**: MLP integration tests
- Error handling integration
- Memory management integration
- Edge cases and stress tests

**Day 7-8**: Documentation and verification
- Test documentation
- Run full test suite
- Report results

**Day 9-10**: Bug reporting and retesting
- Report bugs to Claude #1
- Retest after fixes
- Final validation

---

## 💡 Tips

1. **Test thoroughly**: Cover all edge cases
2. **Use fork()**: For tests that terminate program
3. **Track memory**: Verify no leaks in test code
4. **Be systematic**: Test one function at a time
5. **Document failures**: Clear failure messages

---

## 🎉 When You're Done

Report:
1. Test files created (6/6)
2. C unit tests passing (X/30+)
3. MLP integration tests passing (X/12+)
4. Test documentation complete
5. Branch pushed

**Branch**: `claude/phase5-2-testing-[SESSION_ID]`
**Expected**: 6 commits

Let's test Phase 5.2! 🧪
