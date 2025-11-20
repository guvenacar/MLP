#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

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

#define TEST_END() do { \
    printf("✅ PASSED\n"); \
    passed_count++; \
} while(0)

// Function declarations
double math_sqrt(double num);
double math_pow(double base, double exponent);
int64_t math_abs(int64_t num);
double math_fabs(double num);
int64_t math_min(int64_t a, int64_t b);
int64_t math_max(int64_t a, int64_t b);
int64_t math_round(double num);
int64_t math_floor(double num);
int64_t math_ceil(double num);
double math_sin(double angle);
double math_cos(double angle);
double math_tan(double angle);

// Test counters
static int test_count = 0;
static int passed_count = 0;
static int failed_count = 0;

// Test 1: Square Root
void test_sqrt() {
    TEST_START("Square root");
    
    ASSERT_FLOAT_EQ(math_sqrt(16.0), 4.0, 0.001);
    ASSERT_FLOAT_EQ(math_sqrt(25.0), 5.0, 0.001);
    ASSERT_FLOAT_EQ(math_sqrt(100.0), 10.0, 0.001);
    ASSERT_FLOAT_EQ(math_sqrt(2.0), 1.414213, 0.001);
    
    printf("sqrt(16)=4.0, sqrt(25)=5.0, sqrt(100)=10.0\n");
    TEST_END();
}

// Test 2: Power
void test_pow() {
    TEST_START("Power function");
    
    ASSERT_FLOAT_EQ(math_pow(2.0, 3.0), 8.0, 0.001);
    ASSERT_FLOAT_EQ(math_pow(5.0, 2.0), 25.0, 0.001);
    ASSERT_FLOAT_EQ(math_pow(10.0, 0.0), 1.0, 0.001);
    ASSERT_FLOAT_EQ(math_pow(2.0, -1.0), 0.5, 0.001);
    
    printf("2^3=8, 5^2=25, 10^0=1, 2^-1=0.5\n");
    TEST_END();
}

// Test 3: Absolute Value (int)
void test_abs() {
    TEST_START("Absolute value (int)");
    
    ASSERT_EQ(math_abs(-5), 5);
    ASSERT_EQ(math_abs(5), 5);
    ASSERT_EQ(math_abs(0), 0);
    ASSERT_EQ(math_abs(-999), 999);
    
    printf("abs(-5)=5, abs(5)=5, abs(0)=0\n");
    TEST_END();
}

// Test 4: Absolute Value (float)
void test_fabs() {
    TEST_START("Absolute value (float)");
    
    ASSERT_FLOAT_EQ(math_fabs(-5.5), 5.5, 0.001);
    ASSERT_FLOAT_EQ(math_fabs(5.5), 5.5, 0.001);
    ASSERT_FLOAT_EQ(math_fabs(0.0), 0.0, 0.001);
    ASSERT_FLOAT_EQ(math_fabs(-123.456), 123.456, 0.001);
    
    printf("fabs(-5.5)=5.5, fabs(5.5)=5.5\n");
    TEST_END();
}

// Test 5: Min/Max
void test_min_max() {
    TEST_START("Min/Max functions");
    
    ASSERT_EQ(math_min(5, 10), 5);
    ASSERT_EQ(math_min(10, 5), 5);
    ASSERT_EQ(math_min(-5, 5), -5);
    
    ASSERT_EQ(math_max(5, 10), 10);
    ASSERT_EQ(math_max(10, 5), 10);
    ASSERT_EQ(math_max(-5, 5), 5);
    
    printf("min(5,10)=5, max(5,10)=10\n");
    TEST_END();
}

// Test 6: Rounding
void test_round() {
    TEST_START("Rounding functions");
    
    ASSERT_EQ(math_round(5.7), 6);
    ASSERT_EQ(math_round(5.3), 5);
    ASSERT_EQ(math_round(5.5), 6);
    ASSERT_EQ(math_round(-5.7), -6);
    
    printf("round(5.7)=6, round(5.3)=5\n");
    TEST_END();
}

// Test 7: Floor
void test_floor() {
    TEST_START("Floor function");
    
    ASSERT_EQ(math_floor(5.7), 5);
    ASSERT_EQ(math_floor(5.3), 5);
    ASSERT_EQ(math_floor(-5.3), -6);
    ASSERT_EQ(math_floor(10.0), 10);
    
    printf("floor(5.7)=5, floor(-5.3)=-6\n");
    TEST_END();
}

// Test 8: Ceil
void test_ceil() {
    TEST_START("Ceiling function");
    
    ASSERT_EQ(math_ceil(5.1), 6);
    ASSERT_EQ(math_ceil(5.9), 6);
    ASSERT_EQ(math_ceil(-5.9), -5);
    ASSERT_EQ(math_ceil(10.0), 10);
    
    printf("ceil(5.1)=6, ceil(-5.9)=-5\n");
    TEST_END();
}

// Test 9: Trigonometry
void test_trigonometry() {
    TEST_START("Trigonometric functions");
    
    // sin(π/2) = 1
    ASSERT_FLOAT_EQ(math_sin(M_PI / 2.0), 1.0, 0.001);
    
    // cos(0) = 1
    ASSERT_FLOAT_EQ(math_cos(0.0), 1.0, 0.001);
    
    // tan(π/4) = 1
    ASSERT_FLOAT_EQ(math_tan(M_PI / 4.0), 1.0, 0.001);
    
    // sin(0) = 0
    ASSERT_FLOAT_EQ(math_sin(0.0), 0.0, 0.001);
    
    printf("sin(π/2)=1.0, cos(0)=1.0, tan(π/4)=1.0\n");
    TEST_END();
}

// Test 10: Edge Cases
void test_edge_cases() {
    TEST_START("Edge cases");
    
    // sqrt(0) = 0
    ASSERT_FLOAT_EQ(math_sqrt(0.0), 0.0, 0.001);
    
    // pow(0, 0) = 1
    ASSERT_FLOAT_EQ(math_pow(0.0, 0.0), 1.0, 0.001);
    
    // abs(INT_MIN + 1)
    ASSERT_EQ(math_abs(-2147483647), 2147483647);
    
    // round(0.5) = 1 (C standard rounding)
    ASSERT_EQ(math_round(0.5), 1);
    
    printf("Edge cases handled\n");
    TEST_END();
}

// Test 11: Negative Square Root
void test_negative_sqrt() {
    TEST_START("Negative square root (error handling)");
    
    // Should return 0 and print error
    double result = math_sqrt(-1.0);
    ASSERT_FLOAT_EQ(result, 0.0, 0.001);
    
    printf("sqrt(-1) returns 0 with error\n");
    TEST_END();
}

// Test 12: Combined Operations
void test_combined() {
    TEST_START("Combined operations");
    
    // (2^3) * sqrt(16) = 8 * 4 = 32
    double result1 = math_pow(2.0, 3.0) * math_sqrt(16.0);
    ASSERT_FLOAT_EQ(result1, 32.0, 0.001);
    
    // abs(-5) + abs(-10) = 15
    int64_t result2 = math_abs(-5) + math_abs(-10);
    ASSERT_EQ(result2, 15);
    
    // max(5, min(10, 3)) = max(5, 3) = 5
    int64_t result3 = math_max(5, math_min(10, 3));
    ASSERT_EQ(result3, 5);
    
    printf("Combined: (2^3)*sqrt(16)=32, abs(-5)+abs(-10)=15\n");
    TEST_END();
}

// Main test runner
int main() {
    printf("╔══════════════════════════════════════╗\n");
    printf("║  Math Operations Tests - Phase 4    ║\n");
    printf("╚══════════════════════════════════════╝\n");
    
    test_sqrt();
    test_pow();
    test_abs();
    test_fabs();
    test_min_max();
    test_round();
    test_floor();
    test_ceil();
    test_trigonometry();
    test_edge_cases();
    test_negative_sqrt();
    test_combined();
    
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║          Test Results                ║\n");
    printf("╠══════════════════════════════════════╣\n");
    printf("║ Passed:  %2d                         ║\n", passed_count);
    printf("║ Failed:  %2d                         ║\n", failed_count);
    printf("║ Total:   %2d                         ║\n", passed_count + failed_count);
    printf("╚══════════════════════════════════════╝\n");
    
    return failed_count == 0 ? 0 : 1;
}
