#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// Test macros
#define TEST_START(name) printf("\n=== Test %d: %s ===\n", ++test_count, name)
#define ASSERT_EQ(a, b) do { \
    if ((a) != (b)) { \
        fprintf(stderr, "❌ FAIL: Expected %ld, got %ld\n", (long)(b), (long)(a)); \
        failed_count++; \
        return; \
    } \
} while(0)

#define ASSERT_TRUE(expr) do { \
    if (!(expr)) { \
        fprintf(stderr, "❌ FAIL: Expression false: %s\n", #expr); \
        failed_count++; \
        return; \
    } \
} while(0)

#define ASSERT_STR_EQ(a, b) do { \
    if (strcmp((a), (b)) != 0) { \
        fprintf(stderr, "❌ FAIL: Expected '%s', got '%s'\n", (b), (a)); \
        failed_count++; \
        return; \
    } \
} while(0)

#define TEST_END() do { \
    printf("✅ PASSED\n"); \
    passed_count++; \
} while(0)

// Function declarations from runtime.c
char** string_split(const char* str, const char* delim, int* count);
char* string_join(char** parts, int count, const char* delim);
char* string_replace(const char* str, const char* old, const char* new);
char* string_trim(const char* str);
char* string_upper(const char* str);
char* string_lower(const char* str);
int string_find(const char* str, const char* needle);
int string_starts_with(const char* str, const char* prefix);
int string_ends_with(const char* str, const char* suffix);

// Test counters
static int test_count = 0;
static int passed_count = 0;
static int failed_count = 0;

// Test 1: String split
void test_string_split() {
    TEST_START("String split");
    
    int count = 0;
    char** parts = string_split("a,b,c", ",", &count);
    
    ASSERT_EQ(count, 3);
    ASSERT_STR_EQ(parts[0], "a");
    ASSERT_STR_EQ(parts[1], "b");
    ASSERT_STR_EQ(parts[2], "c");
    
    printf("Split result: %d parts\n", count);
    
    // Free parts
    for (int i = 0; i < count; i++) {
        free(parts[i]);
    }
    free(parts);
    
    TEST_END();
}

// Test 2: String join
void test_string_join() {
    TEST_START("String join");
    
    char* parts[] = {"apple", "banana", "cherry"};
    char* joined = string_join(parts, 3, "-");
    
    ASSERT_STR_EQ(joined, "apple-banana-cherry");
    printf("Join result: %s\n", joined);
    
    free(joined);
    TEST_END();
}

// Test 3: String replace
void test_string_replace() {
    TEST_START("String replace");
    
    char* replaced = string_replace("hello world", "world", "MLP");
    ASSERT_STR_EQ(replaced, "hello MLP");
    printf("Replace result: %s\n", replaced);
    
    free(replaced);
    TEST_END();
}

// Test 4: String trim
void test_string_trim() {
    TEST_START("String trim");
    
    char* trimmed = string_trim("  hello  ");
    ASSERT_STR_EQ(trimmed, "hello");
    printf("Trim result: '%s'\n", trimmed);
    
    free(trimmed);
    TEST_END();
}

// Test 5: String upper/lower
void test_string_case() {
    TEST_START("Upper/Lower case");
    
    char* upper = string_upper("hello");
    char* lower = string_lower("WORLD");
    
    ASSERT_STR_EQ(upper, "HELLO");
    ASSERT_STR_EQ(lower, "world");
    printf("Upper: %s, Lower: %s\n", upper, lower);
    
    free(upper);
    free(lower);
    TEST_END();
}

// Test 6: String find
void test_string_find() {
    TEST_START("String find");
    
    int pos = string_find("hello world", "world");
    ASSERT_EQ(pos, 6);
    printf("Find position: %d\n", pos);
    
    int not_found = string_find("hello world", "xyz");
    ASSERT_EQ(not_found, -1);
    
    TEST_END();
}

// Test 7: Starts/ends with
void test_starts_ends_with() {
    TEST_START("Starts/Ends with");
    
    ASSERT_EQ(string_starts_with("hello world", "hello"), 1);
    ASSERT_EQ(string_ends_with("hello world", "world"), 1);
    ASSERT_EQ(string_starts_with("hello world", "bye"), 0);
    ASSERT_EQ(string_ends_with("hello world", "xyz"), 0);
    
    printf("Starts/Ends checks passed\n");
    TEST_END();
}

// Main test runner
int main() {
    printf("╔══════════════════════════════════════╗\n");
    printf("║ String Operations Tests - Phase 3   ║\n");
    printf("╚══════════════════════════════════════╝\n");
    
    test_string_split();
    test_string_join();
    test_string_replace();
    test_string_trim();
    test_string_case();
    test_string_find();
    test_starts_ends_with();
    
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║          Test Results                ║\n");
    printf("╠══════════════════════════════════════╣\n");
    printf("║ Passed:  %2d                         ║\n", passed_count);
    printf("║ Failed:  %2d                         ║\n", failed_count);
    printf("║ Total:   %2d                         ║\n", passed_count + failed_count);
    printf("╚══════════════════════════════════════╝\n");
    
    return failed_count == 0 ? 0 : 1;
}
