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

#define ASSERT_FLOAT_EQ(a, b, epsilon) do { \
    if (fabs((a) - (b)) > (epsilon)) { \
        fprintf(stderr, "❌ FAIL: Expected %.6f, got %.6f\n", (b), (a)); \
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

// Function declarations
int64_t string_to_int(const char* str);
char* int_to_string(int64_t num);
double string_to_float(const char* str);
char* float_to_string(double num);
int string_to_bool(const char* str);
char* bool_to_string(int value);

// Test counters
static int test_count = 0;
static int passed_count = 0;
static int failed_count = 0;

// Test 1: String to Int
void test_string_to_int() {
    TEST_START("String to Int conversion");
    
    ASSERT_EQ(string_to_int("123"), 123);
    ASSERT_EQ(string_to_int("-456"), -456);
    ASSERT_EQ(string_to_int("0"), 0);
    ASSERT_EQ(string_to_int("999999"), 999999);
    
    printf("Conversions: 123, -456, 0, 999999\n");
    TEST_END();
}

// Test 2: Int to String
void test_int_to_string() {
    TEST_START("Int to String conversion");
    
    char* str1 = int_to_string(123);
    char* str2 = int_to_string(-456);
    char* str3 = int_to_string(0);
    
    ASSERT_STR_EQ(str1, "123");
    ASSERT_STR_EQ(str2, "-456");
    ASSERT_STR_EQ(str3, "0");
    
    printf("Conversions: %s, %s, %s\n", str1, str2, str3);
    
    free(str1);
    free(str2);
    free(str3);
    
    TEST_END();
}

// Test 3: String to Float
void test_string_to_float() {
    TEST_START("String to Float conversion");
    
    ASSERT_FLOAT_EQ(string_to_float("123.45"), 123.45, 0.001);
    ASSERT_FLOAT_EQ(string_to_float("-67.89"), -67.89, 0.001);
    ASSERT_FLOAT_EQ(string_to_float("0.0"), 0.0, 0.001);
    ASSERT_FLOAT_EQ(string_to_float("3.14159"), 3.14159, 0.00001);
    
    printf("Conversions: 123.45, -67.89, 0.0, 3.14159\n");
    TEST_END();
}

// Test 4: Float to String
void test_float_to_string() {
    TEST_START("Float to String conversion");
    
    char* str1 = float_to_string(123.45);
    char* str2 = float_to_string(-67.89);
    char* str3 = float_to_string(0.0);
    
    // Check prefix (first few chars)
    assert(strncmp(str1, "123.45", 6) == 0);
    assert(strncmp(str2, "-67.89", 6) == 0);
    assert(strncmp(str3, "0.000", 5) == 0);
    
    printf("Conversions: %s, %s, %s\n", str1, str2, str3);
    
    free(str1);
    free(str2);
    free(str3);
    
    TEST_END();
}

// Test 5: String to Bool
void test_string_to_bool() {
    TEST_START("String to Bool conversion");
    
    ASSERT_EQ(string_to_bool("true"), 1);
    ASSERT_EQ(string_to_bool("TRUE"), 1);
    ASSERT_EQ(string_to_bool("1"), 1);
    ASSERT_EQ(string_to_bool("yes"), 1);
    ASSERT_EQ(string_to_bool("false"), 0);
    ASSERT_EQ(string_to_bool("FALSE"), 0);
    ASSERT_EQ(string_to_bool("0"), 0);
    ASSERT_EQ(string_to_bool("no"), 0);
    
    printf("true->1, false->0, 1->1, 0->0\n");
    TEST_END();
}

// Test 6: Bool to String
void test_bool_to_string() {
    TEST_START("Bool to String conversion");
    
    char* str1 = bool_to_string(1);
    char* str2 = bool_to_string(0);
    
    ASSERT_STR_EQ(str1, "true");
    ASSERT_STR_EQ(str2, "false");
    
    printf("1->true, 0->false\n");
    
    free(str1);
    free(str2);
    
    TEST_END();
}

// Test 7: Edge Cases - Empty String
void test_empty_string() {
    TEST_START("Empty string conversions");
    
    ASSERT_EQ(string_to_int(""), 0);
    ASSERT_FLOAT_EQ(string_to_float(""), 0.0, 0.001);
    
    printf("Empty string -> 0\n");
    TEST_END();
}

// Test 8: Edge Cases - Large Numbers
void test_large_numbers() {
    TEST_START("Large number conversions");
    
    int64_t large = 999999999;
    char* str = int_to_string(large);
    int64_t back = string_to_int(str);
    
    ASSERT_EQ(back, large);
    printf("Large number round-trip: %ld\n", large);
    
    free(str);
    TEST_END();
}

// Test 9: Whitespace Handling
void test_whitespace() {
    TEST_START("Whitespace handling");
    
    // atoi/atof should handle leading whitespace
    ASSERT_EQ(string_to_int("  123"), 123);
    ASSERT_FLOAT_EQ(string_to_float("  45.67"), 45.67, 0.001);
    
    printf("Leading whitespace handled\n");
    TEST_END();
}

// Test 10: Invalid Conversions
void test_invalid_conversions() {
    TEST_START("Invalid conversions");
    
    // Invalid strings default to 0
    ASSERT_EQ(string_to_int("abc"), 0);
    ASSERT_FLOAT_EQ(string_to_float("xyz"), 0.0, 0.001);
    
    printf("Invalid strings -> 0\n");
    TEST_END();
}

// Main test runner
int main() {
    printf("╔══════════════════════════════════════╗\n");
    printf("║ Type Conversion Tests - Phase 4     ║\n");
    printf("╚══════════════════════════════════════╝\n");
    
    test_string_to_int();
    test_int_to_string();
    test_string_to_float();
    test_float_to_string();
    test_string_to_bool();
    test_bool_to_string();
    test_empty_string();
    test_large_numbers();
    test_whitespace();
    test_invalid_conversions();
    
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║          Test Results                ║\n");
    printf("╠══════════════════════════════════════╣\n");
    printf("║ Passed:  %2d                         ║\n", passed_count);
    printf("║ Failed:  %2d                         ║\n", failed_count);
    printf("║ Total:   %2d                         ║\n", passed_count + failed_count);
    printf("╚══════════════════════════════════════╝\n");
    
    return failed_count == 0 ? 0 : 1;
}
