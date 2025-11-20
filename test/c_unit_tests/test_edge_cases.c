/**
 * Phase 5.1 C Unit Tests - Edge Cases & Boundary Conditions
 * Tests: All Phase 5.1 functions with extreme inputs
 * 
 * Expected: 13 tests
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
extern void mlp_set_args(int argc, char** argv);
extern long mlp_get_argc(void);
extern char* mlp_get_argv(long index);

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
// String Length Edge Cases (3 tests)
// ===========================

TEST(string_length_unicode_multibyte) {
    // UTF-8 multibyte characters
    ASSERT_EQ(string_length("Hello 世界"), 12);  // Bytes, not characters
    ASSERT_EQ(string_length("Привет мир"), 19);
    ASSERT_EQ(string_length("مرحبا"), 10);
}

TEST(string_length_special_chars) {
    ASSERT_EQ(string_length("\n\r\t"), 3);
    ASSERT_EQ(string_length("\\n\\r\\t"), 6);  // Escaped
    ASSERT_EQ(string_length("\"quotes\""), 8);
}

TEST(string_length_very_long) {
    // Test with 10,000 character string
    char* very_long = (char*)malloc(10001);
    memset(very_long, 'X', 10000);
    very_long[10000] = '\0';

    ASSERT_EQ(string_length(very_long), 10000);

    free(very_long);
}

// ===========================
// Substring Edge Cases (3 tests)
// ===========================

TEST(substring_zero_length) {
    char* result;

    result = string_substring("Hello", 0, 0);
    ASSERT_STR_EQ(result, "");
    free(result);

    result = string_substring("Hello", 2, 0);
    ASSERT_STR_EQ(result, "");
    free(result);
}

TEST(substring_exact_boundaries) {
    char* result;

    // Exact start and end
    result = string_substring("Hello", 0, 5);
    ASSERT_STR_EQ(result, "Hello");
    free(result);

    // Start at last char
    result = string_substring("Hello", 4, 1);
    ASSERT_STR_EQ(result, "o");
    free(result);

    // Start at last char, length extends
    result = string_substring("Hello", 4, 10);
    ASSERT_STR_EQ(result, "o");
    free(result);
}

TEST(substring_unicode) {
    char* result;

    // Note: substring works on bytes, not characters
    result = string_substring("Hello 世界", 0, 6);
    ASSERT_STR_EQ(result, "Hello ");
    free(result);

    // Getting the Chinese part (bytes 6-12)
    result = string_substring("Hello 世界", 6, 6);
    ASSERT_STR_EQ(result, "世界");
    free(result);
}

// ===========================
// IndexOf Edge Cases (3 tests)
// ===========================

TEST(index_of_empty_needle) {
    // Empty string should match at position 0
    ASSERT_EQ(string_index_of("Hello", ""), 0);
    ASSERT_EQ(string_index_of("", ""), 0);
}

TEST(index_of_repeated_pattern) {
    // Should find first occurrence
    ASSERT_EQ(string_index_of("abababab", "ab"), 0);
    ASSERT_EQ(string_index_of("abababab", "ba"), 1);
    ASSERT_EQ(string_index_of("aaaa", "aa"), 0);
}

TEST(index_of_overlapping) {
    // Test with overlapping patterns
    ASSERT_EQ(string_index_of("aaaa", "aaa"), 0);  // First match
    ASSERT_EQ(string_index_of("abcabcabc", "abc"), 0);
}

// ===========================
// LastIndexOf Edge Cases (2 tests)
// ===========================

TEST(last_index_of_empty_needle) {
    // Empty string should match at end
    long len = string_length("Hello");
    ASSERT_EQ(string_last_index_of("Hello", ""), len);
}

TEST(last_index_of_repeated_pattern) {
    // Should find last occurrence
    ASSERT_EQ(string_last_index_of("abababab", "ab"), 6);  // Last "ab" at index 6
    ASSERT_EQ(string_last_index_of("abababab", "ba"), 5);  // Last "ba" at index 5
    ASSERT_EQ(string_last_index_of("aaaa", "aa"), 2);  // Last "aa" at index 2
}

// ===========================
// Command-Line Args Edge Cases (2 tests)
// ===========================

TEST(cmdline_args_empty_strings) {
    char* argv[] = {"program", "", "arg2", ""};
    mlp_set_args(4, argv);

    char* arg1 = mlp_get_argv(1);
    ASSERT_STR_EQ(arg1, "");
    free(arg1);

    char* arg3 = mlp_get_argv(3);
    ASSERT_STR_EQ(arg3, "");
    free(arg3);
}

TEST(cmdline_args_very_long) {
    // Create a very long argument
    char* long_arg = (char*)malloc(1001);
    memset(long_arg, 'A', 1000);
    long_arg[1000] = '\0';

    char* argv[] = {"program", long_arg};
    mlp_set_args(2, argv);

    char* result = mlp_get_argv(1);
    ASSERT_EQ(string_length(result), 1000);
    free(result);
    free(long_arg);
}

// ===========================
// Main
// ===========================

int main() {
    printf("====================================================\n");
    printf("Phase 5.1 C Unit Tests - Edge Cases & Boundaries\n");
    printf("====================================================\n\n");

    // String length edge cases (3)
    RUN_TEST(string_length_unicode_multibyte);
    RUN_TEST(string_length_special_chars);
    RUN_TEST(string_length_very_long);

    // Substring edge cases (3)
    RUN_TEST(substring_zero_length);
    RUN_TEST(substring_exact_boundaries);
    RUN_TEST(substring_unicode);

    // IndexOf edge cases (3)
    RUN_TEST(index_of_empty_needle);
    RUN_TEST(index_of_repeated_pattern);
    RUN_TEST(index_of_overlapping);

    // LastIndexOf edge cases (2)
    RUN_TEST(last_index_of_empty_needle);
    RUN_TEST(last_index_of_repeated_pattern);

    // Command-line args edge cases (2)
    RUN_TEST(cmdline_args_empty_strings);
    RUN_TEST(cmdline_args_very_long);

    printf("\n====================================================\n");
    printf("Tests Passed: %d\n", tests_passed);
    printf("Tests Failed: %d\n", tests_failed);
    printf("====================================================\n");

    return tests_failed > 0 ? 1 : 0;
}
