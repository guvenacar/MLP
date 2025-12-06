#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include "tto_runtime.h"

void test_overflow_detection() {
    printf("=== TTO Runtime Phase 3.1: Overflow Detection Tests ===\n\n");
    
    // Test 1: Safe addition
    printf("Test 1: Safe INT64 addition\n");
    int64_t result;
    int status = tto_safe_add_i64(100, 200, &result);
    printf("  100 + 200 = %ld, overflow: %s\n", result, status ? "YES" : "NO");
    
    // Test 2: Overflow addition
    printf("\nTest 2: INT64 addition overflow\n");
    status = tto_safe_add_i64(INT64_MAX, 1, &result);
    printf("  INT64_MAX + 1, overflow: %s\n", status ? "YES" : "NO");
    if (status) {
        BigDecimal* bd1 = tto_promote_to_bigdecimal(INT64_MAX);
        printf("  Promoted to BigDecimal: %s%s\n", 
               bd1->is_negative ? "-" : "", bd1->digits);
        bigdecimal_free(bd1);
    }
    
    // Test 3: Safe multiplication
    printf("\nTest 3: Safe INT64 multiplication\n");
    status = tto_safe_mul_i64(1000, 2000, &result);
    printf("  1000 * 2000 = %ld, overflow: %s\n", result, status ? "YES" : "NO");
    
    // Test 4: Overflow multiplication
    printf("\nTest 4: INT64 multiplication overflow\n");
    status = tto_safe_mul_i64(INT64_MAX / 2, 3, &result);
    printf("  (INT64_MAX/2) * 3, overflow: %s\n", status ? "YES" : "NO");
    
    // Test 5: Safe subtraction
    printf("\nTest 5: Safe INT64 subtraction\n");
    status = tto_safe_sub_i64(1000, 500, &result);
    printf("  1000 - 500 = %ld, overflow: %s\n", result, status ? "YES" : "NO");
    
    // Test 6: Overflow subtraction (underflow)
    printf("\nTest 6: INT64 subtraction underflow\n");
    status = tto_safe_sub_i64(INT64_MIN, 1, &result);
    printf("  INT64_MIN - 1, overflow: %s\n", status ? "YES" : "NO");
    
    // Test 7: Edge cases
    printf("\nTest 7: Edge cases\n");
    status = tto_safe_mul_i64(0, INT64_MAX, &result);
    printf("  0 * INT64_MAX = %ld, overflow: %s\n", result, status ? "YES" : "NO");
    
    status = tto_safe_mul_i64(-1, INT64_MIN, &result);
    printf("  -1 * INT64_MIN, overflow: %s\n", status ? "YES" : "NO");
}

void test_bigdecimal_promotion() {
    printf("\n\n=== BigDecimal Promotion Tests ===\n\n");
    
    printf("Test 1: Small number promotion\n");
    BigDecimal* bd = tto_promote_to_bigdecimal(12345);
    printf("  12345 → BigDecimal: %s%s\n", 
           bd->is_negative ? "-" : "", bd->digits);
    bigdecimal_free(bd);
    
    printf("\nTest 2: Negative number promotion\n");
    bd = tto_promote_to_bigdecimal(-9876543210);
    printf("  -9876543210 → BigDecimal: %s%s\n", 
           bd->is_negative ? "-" : "", bd->digits);
    bigdecimal_free(bd);
    
    printf("\nTest 3: INT64_MAX promotion\n");
    bd = tto_promote_to_bigdecimal(INT64_MAX);
    printf("  INT64_MAX → BigDecimal: %s\n", bd->digits);
    bigdecimal_free(bd);
}

void test_sso_strings() {
    printf("\n\n=== SSO String Tests ===\n\n");
    
    printf("Test 1: Short string (fits in SSO)\n");
    const char* short_str = "Hello";
    bool fits = sso_would_fit(short_str);
    printf("  \"%s\" (length: %zu) fits in SSO: %s\n", 
           short_str, strlen(short_str), fits ? "YES" : "NO");
    
    if (fits) {
        SSOString sso = sso_create(short_str);
        printf("  SSO data: \"%s\", length: %d\n", sso.data, sso.length);
    }
    
    printf("\nTest 2: Exactly 23 bytes (SSO limit)\n");
    const char* exact_str = "12345678901234567890123"; // 23 chars
    fits = sso_would_fit(exact_str);
    printf("  \"%s\" (length: %zu) fits in SSO: %s\n", 
           exact_str, strlen(exact_str), fits ? "YES" : "NO");
    
    printf("\nTest 3: Long string (needs heap)\n");
    const char* long_str = "This is a very long string that exceeds 23 bytes";
    fits = sso_would_fit(long_str);
    printf("  \"%s\" (length: %zu) fits in SSO: %s\n", 
           long_str, strlen(long_str), fits ? "YES" : "NO");
    
    if (!fits) {
        HeapString* hs = heap_string_create(long_str);
        printf("  HeapString data: \"%s\"\n", hs->data);
        printf("  Length: %zu, Capacity: %zu, RefCount: %d\n", 
               hs->length, hs->capacity, hs->refcount);
        free(hs->data);
        free(hs);
    }
}

int main() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║   TTO Runtime Library - Phase 3.1 Test Suite          ║\n");
    printf("║   Overflow Detection & Type Promotion                  ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    test_overflow_detection();
    test_bigdecimal_promotion();
    test_sso_strings();
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║   ✅ Phase 3.1 Tests Complete                          ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    return 0;
}
