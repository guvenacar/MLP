/**
 * Phase 5.1 C Unit Tests - String Operations
 * Tests: string_length, string_substring, string_index_of, string_last_index_of
 * 
 * Expected: 12 tests
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

#define ASSERT_NULL(ptr) do { \
    if ((ptr) != NULL) { \
        fprintf(stderr, "\nAssertion failed: %s should be NULL\n", #ptr); \
        tests_failed++; \
        return; \
    } \
} while(0)

// ===========================
// string_length() Tests (3 tests)
// ===========================

TEST(string_length_basic) {
    ASSERT_EQ(string_length("Hello"), 5);
    ASSERT_EQ(string_length(""), 0);
    ASSERT_EQ(string_length("a"), 1);
    ASSERT_EQ(string_length("Hello World"), 11);
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

// ===========================
// string_substring() Tests (4 tests)
// ===========================

TEST(string_substring_basic) {
    char* result;

    result = string_substring("Hello World", 0, 5);
    ASSERT_STR_EQ(result, "Hello");
    free(result);

    result = string_substring("Hello World", 6, 5);
    ASSERT_STR_EQ(result, "World");
    free(result);

    result = string_substring("Hello World", 0, 11);
    ASSERT_STR_EQ(result, "Hello World");
    free(result);
}

TEST(string_substring_empty) {
    char* result;

    result = string_substring("Hello", 0, 0);
    ASSERT_STR_EQ(result, "");
    free(result);

    result = string_substring("Hello", 5, 0);
    ASSERT_STR_EQ(result, "");
    free(result);
}

TEST(string_substring_out_of_bounds) {
    char* result;

    // Start beyond length
    result = string_substring("Hello", 10, 5);
    ASSERT_STR_EQ(result, "");
    free(result);

    // Length extends beyond string
    result = string_substring("Hello", 3, 10);
    ASSERT_STR_EQ(result, "lo");  // Should return what's available
    free(result);

    // Negative start
    result = string_substring("Hello", -1, 5);
    ASSERT_STR_EQ(result, "");
    free(result);
}

TEST(string_substring_null) {
    char* result = string_substring(NULL, 0, 5);
    ASSERT_NULL(result);
}

// ===========================
// string_index_of() Tests (3 tests)
// ===========================

TEST(string_index_of_basic) {
    ASSERT_EQ(string_index_of("Hello World", "Hello"), 0);
    ASSERT_EQ(string_index_of("Hello World", "World"), 6);
    ASSERT_EQ(string_index_of("Hello World", "o"), 4);
    ASSERT_EQ(string_index_of("Hello World", "l"), 2);  // First 'l'
}

TEST(string_index_of_not_found) {
    ASSERT_EQ(string_index_of("Hello World", "xyz"), -1);
    ASSERT_EQ(string_index_of("Hello World", "HELLO"), -1);  // Case sensitive
    ASSERT_EQ(string_index_of("Hello", "Hello World"), -1);  // Needle longer
}

TEST(string_index_of_null) {
    ASSERT_EQ(string_index_of(NULL, "test"), -1);
    ASSERT_EQ(string_index_of("test", NULL), -1);
    ASSERT_EQ(string_index_of(NULL, NULL), -1);
}

// ===========================
// string_last_index_of() Tests (2 tests)
// ===========================

TEST(string_last_index_of_basic) {
    ASSERT_EQ(string_last_index_of("Hello World", "o"), 7);  // Last 'o'
    ASSERT_EQ(string_last_index_of("Hello World", "l"), 9);  // Last 'l'
    ASSERT_EQ(string_last_index_of("abcabcabc", "abc"), 6);  // Last occurrence
    ASSERT_EQ(string_last_index_of("test", "test"), 0);  // Full match
}

TEST(string_last_index_of_not_found) {
    ASSERT_EQ(string_last_index_of("Hello World", "xyz"), -1);
    ASSERT_EQ(string_last_index_of("Hello World", "HELLO"), -1);
    ASSERT_EQ(string_last_index_of(NULL, "test"), -1);
    ASSERT_EQ(string_last_index_of("test", NULL), -1);
}

// ===========================
// Main
// ===========================

int main() {
    printf("===========================================\n");
    printf("Phase 5.1 C Unit Tests - String Operations\n");
    printf("===========================================\n\n");

    // string_length tests (3)
    RUN_TEST(string_length_basic);
    RUN_TEST(string_length_null);
    RUN_TEST(string_length_long);

    // string_substring tests (4)
    RUN_TEST(string_substring_basic);
    RUN_TEST(string_substring_empty);
    RUN_TEST(string_substring_out_of_bounds);
    RUN_TEST(string_substring_null);

    // string_index_of tests (3)
    RUN_TEST(string_index_of_basic);
    RUN_TEST(string_index_of_not_found);
    RUN_TEST(string_index_of_null);

    // string_last_index_of tests (2)
    RUN_TEST(string_last_index_of_basic);
    RUN_TEST(string_last_index_of_not_found);

    printf("\n===========================================\n");
    printf("Tests Passed: %d\n", tests_passed);
    printf("Tests Failed: %d\n", tests_failed);
    printf("===========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
