# Phase 5.1 - Testing Brief (Claude #3)

**Session**: Phase 5.1 - Command-Line Args & Enhanced Strings
**Your Role**: Testing & Quality Assurance
**Branch**: `claude/phase5-1-testing-[SESSION_ID]`
**Timeline**: ~2 weeks

---

## 🎯 Your Mission

Create comprehensive test suite for Phase 5.1 features:
1. C unit tests for runtime functions
2. MLP integration tests for all features
3. Edge case and stress testing
4. Validation and verification

**Total Expected**: ~800 lines of test code

---

## 📋 Part 1: C Unit Tests for Runtime Functions

### 1.1 Create test/c_unit_tests/test_string_ops.c

**Goal**: Test 4 new string functions at the C level.

**Structure**:

```c
/**
 * Phase 5.1 C Unit Tests - String Operations
 * Tests: string_length, string_substring, string_index_of, string_last_index_of
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Forward declarations from runtime.c
extern long string_length(const char* str);
extern char* string_substring(const char* str, long start, long length);
extern long string_index_of(const char* str, const char* needle);
extern long string_last_index_of(const char* str, const char* needle);

// Test counter
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    printf("Running: %s... ", #name); \
    test_##name(); \
    printf("PASSED\n"); \
    tests_passed++; \
} while(0)

#define ASSERT_EQ(actual, expected) do { \
    if ((actual) != (expected)) { \
        fprintf(stderr, "\nAssertion failed: %s == %s\n", #actual, #expected); \
        fprintf(stderr, "  Expected: %ld\n", (long)(expected)); \
        fprintf(stderr, "  Actual: %ld\n", (long)(actual)); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_STR_EQ(actual, expected) do { \
    if (strcmp((actual), (expected)) != 0) { \
        fprintf(stderr, "\nAssertion failed: %s == %s\n", #actual, #expected); \
        fprintf(stderr, "  Expected: \"%s\"\n", (expected)); \
        fprintf(stderr, "  Actual: \"%s\"\n", (actual)); \
        tests_failed++; \
        return; \
    } \
} while(0)

// ===========================
// string_length() Tests
// ===========================

TEST(string_length_basic) {
    ASSERT_EQ(string_length("Hello"), 5);
    ASSERT_EQ(string_length(""), 0);
    ASSERT_EQ(string_length("a"), 1);
}

TEST(string_length_null) {
    ASSERT_EQ(string_length(NULL), 0);
}

TEST(string_length_long) {
    // Test with 1000 character string
    char* long_str = (char*)malloc(1001);
    memset(long_str, 'a', 1000);
    long_str[1000] = '\0';

    ASSERT_EQ(string_length(long_str), 1000);

    free(long_str);
}

TEST(string_length_unicode) {
    // Note: This tests byte length, not character count
    ASSERT_EQ(string_length("Hello 世界"), 12);  // UTF-8 bytes
}

// ===========================
// string_substring() Tests
// ===========================

TEST(string_substring_basic) {
    char* result;

    result = string_substring("Hello World", 0, 5);
    ASSERT_STR_EQ(result, "Hello");
    free(result);

    result = string_substring("Hello World", 6, 5);
    ASSERT_STR_EQ(result, "World");
    free(result);
}

TEST(string_substring_full) {
    char* result = string_substring("Hello", 0, 5);
    ASSERT_STR_EQ(result, "Hello");
    free(result);
}

TEST(string_substring_empty) {
    char* result;

    result = string_substring("", 0, 5);
    ASSERT_STR_EQ(result, "");
    free(result);

    result = string_substring("Hello", 0, 0);
    ASSERT_STR_EQ(result, "");
    free(result);
}

TEST(string_substring_out_of_bounds_start) {
    char* result;

    result = string_substring("Hello", -1, 3);
    ASSERT_STR_EQ(result, "");
    free(result);

    result = string_substring("Hello", 10, 3);
    ASSERT_STR_EQ(result, "");
    free(result);
}

TEST(string_substring_truncate) {
    // Length exceeds string
    char* result = string_substring("Hello", 3, 100);
    ASSERT_STR_EQ(result, "lo");
    free(result);
}

TEST(string_substring_null) {
    char* result = string_substring(NULL, 0, 5);
    ASSERT_STR_EQ(result, "");
    free(result);
}

TEST(string_substring_single_char) {
    char* result = string_substring("Hello", 1, 1);
    ASSERT_STR_EQ(result, "e");
    free(result);
}

// ===========================
// string_index_of() Tests
// ===========================

TEST(string_index_of_basic) {
    ASSERT_EQ(string_index_of("Hello World", "World"), 6);
    ASSERT_EQ(string_index_of("Hello World", "Hello"), 0);
    ASSERT_EQ(string_index_of("Hello World", "o"), 4);
}

TEST(string_index_of_not_found) {
    ASSERT_EQ(string_index_of("Hello World", "xyz"), -1);
    ASSERT_EQ(string_index_of("Hello World", "world"), -1);  // Case sensitive
}

TEST(string_index_of_empty) {
    ASSERT_EQ(string_index_of("", "Hello"), -1);
    ASSERT_EQ(string_index_of("Hello", ""), 0);  // Empty needle found at start
}

TEST(string_index_of_null) {
    ASSERT_EQ(string_index_of(NULL, "Hello"), -1);
    ASSERT_EQ(string_index_of("Hello", NULL), -1);
}

TEST(string_index_of_multiple_occurrences) {
    // Should return first occurrence
    ASSERT_EQ(string_index_of("Hello Hello", "Hello"), 0);
    ASSERT_EQ(string_index_of("Hello Hello", "ello"), 1);
}

TEST(string_index_of_single_char) {
    ASSERT_EQ(string_index_of("abcdef", "c"), 2);
    ASSERT_EQ(string_index_of("abcdef", "a"), 0);
    ASSERT_EQ(string_index_of("abcdef", "f"), 5);
}

TEST(string_index_of_overlapping) {
    ASSERT_EQ(string_index_of("aaaa", "aa"), 0);  // First occurrence
}

// ===========================
// string_last_index_of() Tests
// ===========================

TEST(string_last_index_of_basic) {
    ASSERT_EQ(string_last_index_of("Hello World Hello", "Hello"), 12);
    ASSERT_EQ(string_last_index_of("Hello World Hello", "World"), 6);
    ASSERT_EQ(string_last_index_of("Hello World", "o"), 7);
}

TEST(string_last_index_of_not_found) {
    ASSERT_EQ(string_last_index_of("Hello World", "xyz"), -1);
    ASSERT_EQ(string_last_index_of("Hello World", "world"), -1);  // Case sensitive
}

TEST(string_last_index_of_single_occurrence) {
    ASSERT_EQ(string_last_index_of("Hello World", "World"), 6);
}

TEST(string_last_index_of_empty) {
    ASSERT_EQ(string_last_index_of("", "Hello"), -1);
}

TEST(string_last_index_of_null) {
    ASSERT_EQ(string_last_index_of(NULL, "Hello"), -1);
    ASSERT_EQ(string_last_index_of("Hello", NULL), -1);
}

TEST(string_last_index_of_multiple_occurrences) {
    ASSERT_EQ(string_last_index_of("abc abc abc", "abc"), 8);
    ASSERT_EQ(string_last_index_of("abcabcabc", "abc"), 6);
}

TEST(string_last_index_of_overlapping) {
    ASSERT_EQ(string_last_index_of("aaaa", "aa"), 2);  // Last occurrence
}

TEST(string_last_index_of_at_end) {
    ASSERT_EQ(string_last_index_of("Hello World", "ld"), 9);
}

// ===========================
// Main Test Runner
// ===========================

int main(void) {
    printf("========================================\n");
    printf("Phase 5.1 C Unit Tests - String Operations\n");
    printf("========================================\n\n");

    // string_length tests
    printf("--- string_length() tests ---\n");
    RUN_TEST(string_length_basic);
    RUN_TEST(string_length_null);
    RUN_TEST(string_length_long);
    RUN_TEST(string_length_unicode);
    printf("\n");

    // string_substring tests
    printf("--- string_substring() tests ---\n");
    RUN_TEST(string_substring_basic);
    RUN_TEST(string_substring_full);
    RUN_TEST(string_substring_empty);
    RUN_TEST(string_substring_out_of_bounds_start);
    RUN_TEST(string_substring_truncate);
    RUN_TEST(string_substring_null);
    RUN_TEST(string_substring_single_char);
    printf("\n");

    // string_index_of tests
    printf("--- string_index_of() tests ---\n");
    RUN_TEST(string_index_of_basic);
    RUN_TEST(string_index_of_not_found);
    RUN_TEST(string_index_of_empty);
    RUN_TEST(string_index_of_null);
    RUN_TEST(string_index_of_multiple_occurrences);
    RUN_TEST(string_index_of_single_char);
    RUN_TEST(string_index_of_overlapping);
    printf("\n");

    // string_last_index_of tests
    printf("--- string_last_index_of() tests ---\n");
    RUN_TEST(string_last_index_of_basic);
    RUN_TEST(string_last_index_of_not_found);
    RUN_TEST(string_last_index_of_single_occurrence);
    RUN_TEST(string_last_index_of_empty);
    RUN_TEST(string_last_index_of_null);
    RUN_TEST(string_last_index_of_multiple_occurrences);
    RUN_TEST(string_last_index_of_overlapping);
    RUN_TEST(string_last_index_of_at_end);
    printf("\n");

    // Summary
    printf("========================================\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
```

**Compilation**:
```bash
gcc -o test_string_ops test/c_unit_tests/test_string_ops.c runtime/runtime.c -I.
./test_string_ops
```

**Estimated**: ~300 lines

---

### 1.2 Create test/c_unit_tests/test_argc_argv.c

**Goal**: Test argc/argv runtime functions.

```c
/**
 * Phase 5.1 C Unit Tests - Command-Line Arguments
 * Tests: mlp_set_args, mlp_get_argc, mlp_get_argv
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Forward declarations from runtime.c
extern void mlp_set_args(int argc, char** argv);
extern long mlp_get_argc(void);
extern char* mlp_get_argv(long index);

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    printf("Running: %s... ", #name); \
    test_##name(); \
    printf("PASSED\n"); \
    tests_passed++; \
} while(0)

#define ASSERT_EQ(actual, expected) do { \
    if ((actual) != (expected)) { \
        fprintf(stderr, "\nAssertion failed: %s == %s\n", #actual, #expected); \
        fprintf(stderr, "  Expected: %ld\n", (long)(expected)); \
        fprintf(stderr, "  Actual: %ld\n", (long)(actual)); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_STR_EQ(actual, expected) do { \
    if (strcmp((actual), (expected)) != 0) { \
        fprintf(stderr, "\nAssertion failed: %s == %s\n", #actual, #expected); \
        fprintf(stderr, "  Expected: \"%s\"\n", (expected)); \
        fprintf(stderr, "  Actual: \"%s\"\n", (actual)); \
        tests_failed++; \
        return; \
    } \
} while(0)

// ===========================
// argc/argv Tests
// ===========================

TEST(argc_basic) {
    char* argv[] = {"program", "arg1", "arg2"};
    mlp_set_args(3, argv);

    ASSERT_EQ(mlp_get_argc(), 3);
}

TEST(argc_single) {
    char* argv[] = {"program"};
    mlp_set_args(1, argv);

    ASSERT_EQ(mlp_get_argc(), 1);
}

TEST(argc_many) {
    char* argv[] = {"p", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    mlp_set_args(11, argv);

    ASSERT_EQ(mlp_get_argc(), 11);
}

TEST(argv_basic) {
    char* argv[] = {"program", "arg1", "arg2"};
    mlp_set_args(3, argv);

    char* result;

    result = mlp_get_argv(0);
    ASSERT_STR_EQ(result, "program");
    free(result);

    result = mlp_get_argv(1);
    ASSERT_STR_EQ(result, "arg1");
    free(result);

    result = mlp_get_argv(2);
    ASSERT_STR_EQ(result, "arg2");
    free(result);
}

TEST(argv_out_of_bounds_positive) {
    char* argv[] = {"program", "arg1"};
    mlp_set_args(2, argv);

    char* result = mlp_get_argv(10);
    ASSERT_STR_EQ(result, "");
    free(result);
}

TEST(argv_out_of_bounds_negative) {
    char* argv[] = {"program", "arg1"};
    mlp_set_args(2, argv);

    char* result = mlp_get_argv(-1);
    ASSERT_STR_EQ(result, "");
    free(result);
}

TEST(argv_spaces) {
    char* argv[] = {"program", "arg with spaces", "another arg"};
    mlp_set_args(3, argv);

    char* result = mlp_get_argv(1);
    ASSERT_STR_EQ(result, "arg with spaces");
    free(result);
}

TEST(argv_special_chars) {
    char* argv[] = {"program", "--flag=value", "-v", "file.txt"};
    mlp_set_args(4, argv);

    char* result;

    result = mlp_get_argv(1);
    ASSERT_STR_EQ(result, "--flag=value");
    free(result);

    result = mlp_get_argv(2);
    ASSERT_STR_EQ(result, "-v");
    free(result);
}

TEST(argv_empty_string) {
    char* argv[] = {"program", "", "arg2"};
    mlp_set_args(3, argv);

    char* result = mlp_get_argv(1);
    ASSERT_STR_EQ(result, "");
    free(result);
}

// ===========================
// Main Test Runner
// ===========================

int main(void) {
    printf("========================================\n");
    printf("Phase 5.1 C Unit Tests - argc/argv\n");
    printf("========================================\n\n");

    printf("--- argc tests ---\n");
    RUN_TEST(argc_basic);
    RUN_TEST(argc_single);
    RUN_TEST(argc_many);
    printf("\n");

    printf("--- argv tests ---\n");
    RUN_TEST(argv_basic);
    RUN_TEST(argv_out_of_bounds_positive);
    RUN_TEST(argv_out_of_bounds_negative);
    RUN_TEST(argv_spaces);
    RUN_TEST(argv_special_chars);
    RUN_TEST(argv_empty_string);
    printf("\n");

    printf("========================================\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
```

**Estimated**: ~180 lines

---

## 📋 Part 2: MLP Integration Tests

### 2.1 Create test/test_phase5_1_string_ops.mlp

**Goal**: Test all 4 string functions in MLP code.

```mlp
-- Phase 5.1 Integration Test: String Operations

function test_string_length() then
    int result = 0;

    print "=== string_length() tests ==="

    -- Basic tests
    result = string_length("Hello")
    if result != 5 then
        print "FAIL: Expected 5, got:"
        print result
        return 1
    end
    print "✓ string_length('Hello') = 5"

    result = string_length("")
    if result != 0 then
        print "FAIL: Expected 0, got:"
        print result
        return 1
    end
    print "✓ string_length('') = 0"

    result = string_length("a")
    if result != 1 then
        print "FAIL: Expected 1, got:"
        print result
        return 1
    end
    print "✓ string_length('a') = 1"

    -- Long string
    string long_str = "This is a longer string with many characters in it";
    result = string_length(long_str)
    if result != 50 then
        print "FAIL: Expected 50, got:"
        print result
        return 1
    end
    print "✓ string_length(long) = 50"

    print "✓ All string_length tests passed!"
    print ""
    return 0
end

function test_string_substring() then
    string result = "";

    print "=== string_substring() tests ==="

    -- Basic extraction
    result = string_substring("Hello World", 0, 5)
    if string_equals(result, "Hello") == 0 then
        print "FAIL: Expected 'Hello', got:"
        print result
        return 1
    end
    print "✓ substring('Hello World', 0, 5) = 'Hello'"

    result = string_substring("Hello World", 6, 5)
    if string_equals(result, "World") == 0 then
        print "FAIL: Expected 'World', got:"
        print result
        return 1
    end
    print "✓ substring('Hello World', 6, 5) = 'World'"

    -- Full string
    result = string_substring("Hello", 0, 5)
    if string_equals(result, "Hello") == 0 then
        print "FAIL: Expected 'Hello', got:"
        print result
        return 1
    end
    print "✓ substring('Hello', 0, 5) = 'Hello'"

    -- Empty result
    result = string_substring("Hello", 0, 0)
    if string_equals(result, "") == 0 then
        print "FAIL: Expected empty string"
        return 1
    end
    print "✓ substring('Hello', 0, 0) = ''"

    -- Out of bounds (should return empty)
    result = string_substring("Hello", 10, 5)
    if string_equals(result, "") == 0 then
        print "FAIL: Expected empty string for out of bounds"
        return 1
    end
    print "✓ substring out of bounds = ''"

    -- Truncation
    result = string_substring("Hello", 3, 100)
    if string_equals(result, "lo") == 0 then
        print "FAIL: Expected 'lo', got:"
        print result
        return 1
    end
    print "✓ substring truncation = 'lo'"

    -- Single character
    result = string_substring("Hello", 1, 1)
    if string_equals(result, "e") == 0 then
        print "FAIL: Expected 'e', got:"
        print result
        return 1
    end
    print "✓ substring single char = 'e'"

    print "✓ All string_substring tests passed!"
    print ""
    return 0
end

function test_string_index_of() then
    int result = 0;

    print "=== string_index_of() tests ==="

    -- Basic search
    result = string_index_of("Hello World", "World")
    if result != 6 then
        print "FAIL: Expected 6, got:"
        print result
        return 1
    end
    print "✓ index_of('Hello World', 'World') = 6"

    result = string_index_of("Hello World", "Hello")
    if result != 0 then
        print "FAIL: Expected 0, got:"
        print result
        return 1
    end
    print "✓ index_of('Hello World', 'Hello') = 0"

    -- Single character
    result = string_index_of("Hello World", "o")
    if result != 4 then
        print "FAIL: Expected 4, got:"
        print result
        return 1
    end
    print "✓ index_of('Hello World', 'o') = 4"

    -- Not found
    result = string_index_of("Hello World", "xyz")
    if result != -1 then
        print "FAIL: Expected -1, got:"
        print result
        return 1
    end
    print "✓ index_of not found = -1"

    -- Case sensitive
    result = string_index_of("Hello World", "world")
    if result != -1 then
        print "FAIL: Expected -1 (case sensitive), got:"
        print result
        return 1
    end
    print "✓ index_of case sensitive = -1"

    -- Multiple occurrences (first)
    result = string_index_of("Hello Hello", "Hello")
    if result != 0 then
        print "FAIL: Expected 0 (first occurrence), got:"
        print result
        return 1
    end
    print "✓ index_of multiple = 0 (first)"

    print "✓ All string_index_of tests passed!"
    print ""
    return 0
end

function test_string_last_index_of() then
    int result = 0;

    print "=== string_last_index_of() tests ==="

    -- Multiple occurrences (last)
    result = string_last_index_of("Hello World Hello", "Hello")
    if result != 12 then
        print "FAIL: Expected 12, got:"
        print result
        return 1
    end
    print "✓ last_index_of multiple = 12 (last)"

    -- Single occurrence
    result = string_last_index_of("Hello World", "World")
    if result != 6 then
        print "FAIL: Expected 6, got:"
        print result
        return 1
    end
    print "✓ last_index_of single = 6"

    -- Last 'o'
    result = string_last_index_of("Hello World", "o")
    if result != 7 then
        print "FAIL: Expected 7, got:"
        print result
        return 1
    end
    print "✓ last_index_of('Hello World', 'o') = 7"

    -- Not found
    result = string_last_index_of("Hello World", "xyz")
    if result != -1 then
        print "FAIL: Expected -1, got:"
        print result
        return 1
    end
    print "✓ last_index_of not found = -1"

    -- At end
    result = string_last_index_of("Hello World", "ld")
    if result != 9 then
        print "FAIL: Expected 9, got:"
        print result
        return 1
    end
    print "✓ last_index_of at end = 9"

    print "✓ All string_last_index_of tests passed!"
    print ""
    return 0
end

-- Main test runner
print "========================================"
print "Phase 5.1 String Operations Tests"
print "========================================"
print ""

int failures = 0;

failures = failures + test_string_length()
failures = failures + test_string_substring()
failures = failures + test_string_index_of()
failures = failures + test_string_last_index_of()

print "========================================"
if failures == 0 then
    print "✓ ALL TESTS PASSED!"
end

if failures > 0 then
    print "✗ SOME TESTS FAILED:"
    print failures
end
print "========================================"
```

**Estimated**: ~280 lines

---

### 2.2 Create test/test_phase5_1_cmdline.mlp

**Goal**: Test command-line argument support.

```mlp
-- Phase 5.1 Integration Test: Command-Line Arguments

function main(argc, argv) then
    int i = 0;

    print "========================================"
    print "Phase 5.1 Command-Line Args Test"
    print "========================================"
    print ""

    -- Test argc
    print "Argument count (argc):"
    print argc
    print ""

    if argc == 0 then
        print "ERROR: argc should never be 0!"
        return 1
    end

    -- Test argv.get() for all arguments
    print "Arguments (argv):"
    i = 0
    while i < argc then
        string arg = argv.get(i);
        print "  ["
        print i
        print "] = '"
        print arg
        print "'"
        i = i + 1
    end
    print ""

    -- Test program name (should be at index 0)
    string program_name = argv.get(0);
    int prog_len = string_length(program_name);
    if prog_len == 0 then
        print "ERROR: Program name (argv[0]) should not be empty!"
        return 1
    end
    print "Program name:"
    print program_name
    print ""

    -- Test out-of-bounds access (should return empty string)
    string out_of_bounds = argv.get(1000);
    int oob_len = string_length(out_of_bounds);
    if oob_len != 0 then
        print "ERROR: Out-of-bounds access should return empty string!"
        return 1
    end
    print "✓ Out-of-bounds access returns empty string"
    print ""

    -- Test negative index (should return empty string)
    string negative = argv.get(-1);
    int neg_len = string_length(negative);
    if neg_len != 0 then
        print "ERROR: Negative index should return empty string!"
        return 1
    end
    print "✓ Negative index returns empty string"
    print ""

    -- Parse flags if provided
    int verbose = 0;
    int help = 0;
    i = 1
    while i < argc then
        string arg = argv.get(i);

        if string_equals(arg, "-v") then
            verbose = 1
        end

        if string_equals(arg, "--verbose") then
            verbose = 1
        end

        if string_equals(arg, "-h") then
            help = 1
        end

        if string_equals(arg, "--help") then
            help = 1
        end

        i = i + 1
    end

    if verbose == 1 then
        print "✓ Verbose flag detected!"
    end

    if help == 1 then
        print "✓ Help flag detected!"
        print ""
        print "Usage:"
        print program_name
        print " [options] [files]"
        print ""
        print "Options:"
        print "  -v, --verbose    Enable verbose output"
        print "  -h, --help       Show this help message"
        return 0
    end

    print "========================================"
    print "✓ ALL TESTS PASSED!"
    print "========================================"
    print ""
    print "Run with different arguments to test:"
    print "  ./program"
    print "  ./program -v"
    print "  ./program --help"
    print "  ./program file1 file2 file3"
    print "  ./program -v --help arg1 arg2"

    return 0
end
```

**Estimated**: ~120 lines

---

## 📋 Part 3: Comprehensive Integration Test

### 3.1 Enhance test/test_phase5_1_integration.mlp

**Goal**: Create comprehensive test that exercises all features together.

```mlp
-- Phase 5.1 Comprehensive Integration Test
-- Tests all string operations and validates functionality

function test_realistic_csv_parser() then
    print "=== CSV Parser Test ==="

    string csv_line = "Alice,30,Engineer,San Francisco";
    list[string] fields = list();

    -- Parse CSV manually using new string functions
    int pos = 0;
    int field_start = 0;
    int len = string_length(csv_line);

    while pos <= len then
        int comma_pos = -1;

        if pos < len then
            string remaining = string_substring(csv_line, pos, len - pos);
            comma_pos = string_index_of(remaining, ",");
        end

        if comma_pos == -1 then
            -- Last field
            if pos <= len then
                string field = string_substring(csv_line, field_start, len - field_start);
                fields.add(field)
            end
            pos = len + 1  -- Exit
        end

        if comma_pos >= 0 then
            -- Extract field
            int field_len = (pos + comma_pos) - field_start;
            string field = string_substring(csv_line, field_start, field_len);
            fields.add(field)

            pos = pos + comma_pos + 1
            field_start = pos
        end
    end

    -- Validate results
    int field_count = fields.size();
    if field_count != 4 then
        print "FAIL: Expected 4 fields, got:"
        print field_count
        return 1
    end

    string field0 = fields.get(0);
    if string_equals(field0, "Alice") == 0 then
        print "FAIL: Field 0 should be 'Alice'"
        return 1
    end

    print "✓ Parsed 4 fields correctly"
    print "  Name: Alice"
    print "  Age: 30"
    print "  Job: Engineer"
    print "  City: San Francisco"
    print ""

    return 0
end

function test_realistic_url_parser() then
    print "=== URL Parser Test ==="

    string url = "https://github.com/user/repo/issues";
    int len = string_length(url);

    -- Extract protocol
    int protocol_end = string_index_of(url, "://");
    if protocol_end == -1 then
        print "FAIL: No protocol found"
        return 1
    end

    string protocol = string_substring(url, 0, protocol_end);
    if string_equals(protocol, "https") == 0 then
        print "FAIL: Protocol should be 'https'"
        return 1
    end

    -- Extract hostname
    int hostname_start = protocol_end + 3;
    string after_protocol = string_substring(url, hostname_start, len - hostname_start);
    int first_slash = string_index_of(after_protocol, "/");

    string hostname = "";
    if first_slash == -1 then
        hostname = after_protocol
    end

    if first_slash >= 0 then
        hostname = string_substring(after_protocol, 0, first_slash);
    end

    if string_equals(hostname, "github.com") == 0 then
        print "FAIL: Hostname should be 'github.com'"
        return 1
    end

    print "✓ Parsed URL correctly"
    print "  Protocol: https"
    print "  Hostname: github.com"
    print "  Path: /user/repo/issues"
    print ""

    return 0
end

function test_realistic_file_extension() then
    print "=== File Extension Test ==="

    string filename1 = "document.txt";
    string filename2 = "archive.tar.gz";
    string filename3 = "README";

    -- Test 1: Simple extension
    int dot_pos = string_last_index_of(filename1, ".");
    if dot_pos == -1 then
        print "FAIL: Should find dot in 'document.txt'"
        return 1
    end

    int len1 = string_length(filename1);
    string ext1 = string_substring(filename1, dot_pos + 1, len1 - dot_pos - 1);
    if string_equals(ext1, "txt") == 0 then
        print "FAIL: Extension should be 'txt'"
        return 1
    end

    -- Test 2: Double extension
    int dot_pos2 = string_last_index_of(filename2, ".");
    int len2 = string_length(filename2);
    string ext2 = string_substring(filename2, dot_pos2 + 1, len2 - dot_pos2 - 1);
    if string_equals(ext2, "gz") == 0 then
        print "FAIL: Extension should be 'gz'"
        return 1
    end

    -- Test 3: No extension
    int dot_pos3 = string_last_index_of(filename3, ".");
    if dot_pos3 != -1 then
        print "FAIL: Should not find dot in 'README'"
        return 1
    end

    print "✓ File extensions parsed correctly"
    print "  document.txt → txt"
    print "  archive.tar.gz → gz"
    print "  README → (no extension)"
    print ""

    return 0
end

function test_realistic_word_count() then
    print "=== Word Count Test ==="

    string text = "hello world hello universe hello";
    int count = 0;
    int pos = 0;
    int text_len = string_length(text);
    string word = "hello";
    int word_len = string_length(word);

    -- Count occurrences of "hello"
    while pos < text_len then
        string remaining = string_substring(text, pos, text_len - pos);
        int found_pos = string_index_of(remaining, word);

        if found_pos == -1 then
            pos = text_len  -- Exit
        end

        if found_pos >= 0 then
            count = count + 1
            pos = pos + found_pos + word_len
        end
    end

    if count != 3 then
        print "FAIL: Expected 3 occurrences of 'hello', got:"
        print count
        return 1
    end

    print "✓ Word count correct"
    print "  'hello' appears 3 times"
    print ""

    return 0
end

-- Main test runner
print "========================================"
print "Phase 5.1 Comprehensive Integration Test"
print "========================================"
print ""

int failures = 0;

failures = failures + test_realistic_csv_parser()
failures = failures + test_realistic_url_parser()
failures = failures + test_realistic_file_extension()
failures = failures + test_realistic_word_count()

print "========================================"
if failures == 0 then
    print "✓ ALL INTEGRATION TESTS PASSED!"
end

if failures > 0 then
    print "✗ SOME TESTS FAILED:"
    print failures
end
print "========================================"
```

**Estimated**: ~250 lines

---

## 📋 Part 4: Testing Documentation

### 4.1 Create test/PHASE5_1_TEST_PLAN.md

**Goal**: Document testing strategy and results.

```markdown
# Phase 5.1 Test Plan

**Version**: 1.0
**Date**: [DATE]
**Status**: In Progress

---

## Test Coverage

### C Unit Tests
- **test_string_ops.c**: 26 tests for string functions
- **test_argc_argv.c**: 9 tests for command-line arguments
- **Total**: 35 C unit tests

### MLP Integration Tests
- **test_phase5_1_string_ops.mlp**: 28 tests for string operations in MLP
- **test_phase5_1_cmdline.mlp**: Command-line argument validation
- **test_phase5_1_integration.mlp**: 4 comprehensive realistic tests
- **Total**: 32+ MLP integration tests

---

## Test Matrix

| Feature | C Unit Test | MLP Test | Integration | Edge Cases |
|---------|-------------|----------|-------------|------------|
| string_length() | ✅ (4) | ✅ (4) | ✅ | ✅ |
| string_substring() | ✅ (7) | ✅ (7) | ✅ | ✅ |
| string_index_of() | ✅ (7) | ✅ (6) | ✅ | ✅ |
| string_last_index_of() | ✅ (8) | ✅ (5) | ✅ | ✅ |
| argc | ✅ (3) | ✅ | ✅ | ✅ |
| argv | ✅ (6) | ✅ | ✅ | ✅ |

---

## Running Tests

### C Unit Tests
```bash
# Compile string operations tests
gcc -o test_string_ops test/c_unit_tests/test_string_ops.c runtime/runtime.c -I.
./test_string_ops

# Compile argc/argv tests
gcc -o test_argc_argv test/c_unit_tests/test_argc_argv.c runtime/runtime.c -I.
./test_argc_argv
```

### MLP Integration Tests
```bash
# String operations test
python3 mlp_preprocessor.py test/test_phase5_1_string_ops.mlp
nasm -f elf64 output.asm
gcc output.o runtime/runtime.o -o test_string_ops_mlp
./test_string_ops_mlp

# Command-line arguments test
python3 mlp_preprocessor.py test/test_phase5_1_cmdline.mlp
nasm -f elf64 output.asm
gcc output.o runtime/runtime.o -o test_cmdline_mlp
./test_cmdline_mlp arg1 arg2 -v --help

# Comprehensive integration test
python3 mlp_preprocessor.py test/test_phase5_1_integration.mlp
nasm -f elf64 output.asm
gcc output.o runtime/runtime.o -o test_integration_mlp
./test_integration_mlp
```

---

## Test Results

### Phase 5.1 Test Results

| Test Suite | Tests | Passed | Failed | Notes |
|------------|-------|--------|--------|-------|
| C Unit: String Ops | 26 | TBD | TBD | |
| C Unit: argc/argv | 9 | TBD | TBD | |
| MLP: String Ops | 28 | TBD | TBD | |
| MLP: Command-Line | 1 | TBD | TBD | Manual testing required |
| MLP: Integration | 4 | TBD | TBD | |
| **TOTAL** | **68** | **TBD** | **TBD** | |

---

## Edge Cases Tested

### string_length()
- ✅ Empty string
- ✅ NULL pointer
- ✅ Very long strings (1000+ chars)
- ✅ UTF-8 (byte count, not char count)

### string_substring()
- ✅ Empty string
- ✅ NULL pointer
- ✅ Negative start index
- ✅ Out-of-bounds start
- ✅ Length exceeds string
- ✅ Zero length
- ✅ Single character extraction

### string_index_of()
- ✅ Not found (returns -1)
- ✅ NULL parameters
- ✅ Empty strings
- ✅ Case sensitivity
- ✅ Multiple occurrences (returns first)
- ✅ Overlapping matches

### string_last_index_of()
- ✅ Not found (returns -1)
- ✅ NULL parameters
- ✅ Single occurrence
- ✅ Multiple occurrences (returns last)
- ✅ At end of string
- ✅ Overlapping matches

### argc/argv
- ✅ Single argument (program name only)
- ✅ Many arguments (10+)
- ✅ Out-of-bounds positive
- ✅ Out-of-bounds negative
- ✅ Arguments with spaces
- ✅ Special characters
- ✅ Empty string arguments

---

## Known Limitations

1. **Unicode**: string_length() returns byte count, not character count for UTF-8
2. **Performance**: No optimization for repeated searches
3. **Memory**: string_substring() allocates new memory (caller must free via runtime)

---

## Success Criteria

- ✅ All C unit tests pass (35/35)
- ✅ All MLP integration tests pass
- ✅ No memory leaks (verify with valgrind)
- ✅ No segmentation faults
- ✅ Edge cases handled gracefully

---

## Next Steps

1. Run all test suites
2. Document results in this file
3. Fix any failures
4. Add stress tests (large inputs, many iterations)
5. Performance benchmarking

---

**Last Updated**: [DATE]
```

**Estimated**: ~120 lines

---

## 📂 File Summary

Your deliverables:
```
/home/user/MLP/
├── test/c_unit_tests/test_string_ops.c       (NEW: ~300 lines)
├── test/c_unit_tests/test_argc_argv.c        (NEW: ~180 lines)
├── test/test_phase5_1_string_ops.mlp         (NEW: ~280 lines)
├── test/test_phase5_1_cmdline.mlp            (NEW: ~120 lines)
├── test/test_phase5_1_integration.mlp        (NEW: ~250 lines)
└── test/PHASE5_1_TEST_PLAN.md                (NEW: ~120 lines)
```

**Total**: ~1,250 lines of test code + documentation

---

## ✅ Success Criteria

1. ✅ C unit tests complete (2 files, ~480 lines)
2. ✅ MLP integration tests complete (3 files, ~650 lines)
3. ✅ Test plan documentation (~120 lines)
4. ✅ All edge cases covered
5. ✅ Tests are comprehensive and maintainable

---

## 🔧 Git Workflow

```bash
git checkout -b claude/phase5-1-testing-[SESSION_ID]

# Step 1: C unit tests
git add test/c_unit_tests/test_string_ops.c
git commit -m "test(phase5.1): Add C unit tests for string operations"

git add test/c_unit_tests/test_argc_argv.c
git commit -m "test(phase5.1): Add C unit tests for argc/argv"

# Step 2: MLP integration tests
git add test/test_phase5_1_string_ops.mlp
git commit -m "test(phase5.1): Add MLP integration tests for string operations"

git add test/test_phase5_1_cmdline.mlp
git commit -m "test(phase5.1): Add command-line argument validation test"

git add test/test_phase5_1_integration.mlp
git commit -m "test(phase5.1): Add comprehensive integration tests"

# Step 3: Test plan
git add test/PHASE5_1_TEST_PLAN.md
git commit -m "docs(phase5.1): Add test plan and coverage documentation"

git push -u origin claude/phase5-1-testing-[SESSION_ID]
```

---

## 🎯 Testing Strategy

### Test Levels

1. **Unit Tests (C)**: Test individual functions in isolation
2. **Integration Tests (MLP)**: Test functions in MLP code
3. **Realistic Tests**: Test with real-world scenarios
4. **Edge Cases**: Test boundary conditions and error cases

### Coverage Goals

- **Line Coverage**: 100% of new runtime functions
- **Branch Coverage**: All edge cases (NULL, empty, out-of-bounds)
- **Integration Coverage**: All features used together

---

## 💡 Tips

1. **Test First**: Write tests before seeing implementation
2. **Edge Cases**: Always test NULL, empty, and boundary conditions
3. **Realistic**: Include real-world usage scenarios
4. **Documentation**: Document expected vs actual behavior
5. **Automation**: Make tests easy to run repeatedly

---

## 🎉 When You're Done

Report:
1. C unit tests complete (35 tests)
2. MLP integration tests complete (32+ tests)
3. Test plan documentation complete
4. All tests compile successfully
5. Branch pushed

**Branch**: `claude/phase5-1-testing-[SESSION_ID]`
**Expected**: 6 commits

Let's test Phase 5.1 thoroughly! 🧪
