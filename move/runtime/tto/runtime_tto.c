#include "runtime_tto.h"
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ============================================================================
// Phase 3.1: Overflow Detection & Promotion
// ============================================================================

// Check if INT64 addition will overflow
bool tto_runtime_add_will_overflow(int64_t a, int64_t b) {
    // Overflow occurs when:
    // - Both positive and result wraps to negative
    // - Both negative and result wraps to positive
    
    if (b > 0 && a > INT64_MAX - b) {
        return true;  // Positive overflow
    }
    if (b < 0 && a < INT64_MIN - b) {
        return true;  // Negative overflow
    }
    return false;
}

// Check if INT64 subtraction will overflow
bool tto_runtime_sub_will_overflow(int64_t a, int64_t b) {
    // a - b == a + (-b)
    // Overflow occurs when:
    // - b is INT64_MIN (cannot negate)
    // - a + (-b) overflows
    
    if (b == INT64_MIN) {
        return a > 0;  // Cannot negate INT64_MIN
    }
    
    return tto_runtime_add_will_overflow(a, -b);
}

// Check if INT64 multiplication will overflow
bool tto_runtime_mul_will_overflow(int64_t a, int64_t b) {
    // Special cases
    if (a == 0 || b == 0) {
        return false;  // 0 * anything = 0
    }
    
    if (a == 1 || b == 1) {
        return false;  // 1 * x = x
    }
    
    if (a == -1) {
        return b == INT64_MIN;  // -1 * INT64_MIN overflows
    }
    
    if (b == -1) {
        return a == INT64_MIN;  // INT64_MIN * -1 overflows
    }
    
    // General case: check if a * b would overflow
    // Use division to check: if a * b / a != b, overflow occurred
    int64_t result = a * b;
    if (result / a != b) {
        return true;
    }
    
    return false;
}

// Safe addition with overflow check
// Returns true if overflow occurred (result invalid)
bool tto_runtime_safe_add(int64_t a, int64_t b, int64_t* result) {
    if (tto_runtime_add_will_overflow(a, b)) {
        *result = 0;  // Invalid result
        return true;  // Overflow occurred
    }
    
    *result = a + b;
    return false;  // No overflow
}

// Safe subtraction with overflow check
bool tto_runtime_safe_sub(int64_t a, int64_t b, int64_t* result) {
    if (tto_runtime_sub_will_overflow(a, b)) {
        *result = 0;  // Invalid result
        return true;  // Overflow occurred
    }
    
    *result = a - b;
    return false;  // No overflow
}

// Safe multiplication with overflow check
bool tto_runtime_safe_mul(int64_t a, int64_t b, int64_t* result) {
    if (tto_runtime_mul_will_overflow(a, b)) {
        *result = 0;  // Invalid result
        return true;  // Overflow occurred
    }
    
    *result = a * b;
    return false;  // No overflow
}

// ============================================================================
// Phase 3.2: BigDecimal Runtime (Placeholder - minimal implementation)
// ============================================================================

struct BigDecimal {
    char* digits;
    int length;
    bool negative;
    int refcount;
};

// Create BigDecimal from INT64
BigDecimal* tto_bigdec_from_int64(int64_t value) {
    BigDecimal* bd = (BigDecimal*)malloc(sizeof(BigDecimal));
    if (!bd) return NULL;
    
    bd->negative = (value < 0);
    uint64_t abs_value = (value < 0) ? (uint64_t)(-(value + 1)) + 1 : (uint64_t)value;
    
    // Convert to string
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%llu", (unsigned long long)abs_value);
    
    bd->length = strlen(buffer);
    bd->digits = (char*)malloc(bd->length + 1);
    strcpy(bd->digits, buffer);
    bd->refcount = 1;
    
    return bd;
}

// Create BigDecimal from string
BigDecimal* tto_bigdec_from_string(const char* str) {
    if (!str) return NULL;
    
    BigDecimal* bd = (BigDecimal*)malloc(sizeof(BigDecimal));
    if (!bd) return NULL;
    
    bd->negative = (str[0] == '-');
    const char* digits = bd->negative ? str + 1 : str;
    
    bd->length = strlen(digits);
    bd->digits = (char*)malloc(bd->length + 1);
    strcpy(bd->digits, digits);
    bd->refcount = 1;
    
    return bd;
}

// BigDecimal operations (basic placeholders)
BigDecimal* tto_bigdec_add(BigDecimal* a, BigDecimal* b) {
    if (!a || !b) return NULL;
    
    // TODO: Proper arbitrary precision addition
    // For now: Simple string concatenation approach (placeholder)
    // Real implementation would need digit-by-digit addition with carry
    
    // Convert to integers, add, convert back (limited precision)
    long long a_val = 0, b_val = 0;
    sscanf(a->digits, "%lld", &a_val);
    sscanf(b->digits, "%lld", &b_val);
    
    if (a->negative) a_val = -a_val;
    if (b->negative) b_val = -b_val;
    
    long long result_val = a_val + b_val;
    return tto_bigdec_from_int64(result_val);
}

BigDecimal* tto_bigdec_sub(BigDecimal* a, BigDecimal* b) {
    if (!a || !b) return NULL;
    
    // Convert to integers, subtract, convert back (limited precision)
    long long a_val = 0, b_val = 0;
    sscanf(a->digits, "%lld", &a_val);
    sscanf(b->digits, "%lld", &b_val);
    
    if (a->negative) a_val = -a_val;
    if (b->negative) b_val = -b_val;
    
    long long result_val = a_val - b_val;
    return tto_bigdec_from_int64(result_val);
}

BigDecimal* tto_bigdec_mul(BigDecimal* a, BigDecimal* b) {
    if (!a || !b) return NULL;
    
    // Convert to integers, multiply, convert back (limited precision)
    long long a_val = 0, b_val = 0;
    sscanf(a->digits, "%lld", &a_val);
    sscanf(b->digits, "%lld", &b_val);
    
    if (a->negative) a_val = -a_val;
    if (b->negative) b_val = -b_val;
    
    long long result_val = a_val * b_val;
    return tto_bigdec_from_int64(result_val);
}

BigDecimal* tto_bigdec_div(BigDecimal* a, BigDecimal* b) {
    if (!a || !b) return NULL;
    
    // Convert to integers, divide, convert back (limited precision)
    long long a_val = 0, b_val = 0;
    sscanf(a->digits, "%lld", &a_val);
    sscanf(b->digits, "%lld", &b_val);
    
    if (a->negative) a_val = -a_val;
    if (b->negative) b_val = -b_val;
    
    if (b_val == 0) {
        // Division by zero - return NULL or special value
        return tto_bigdec_from_int64(0);
    }
    
    long long result_val = a_val / b_val;
    return tto_bigdec_from_int64(result_val);
}

// Convert BigDecimal to string
char* tto_bigdec_to_string(BigDecimal* bd) {
    if (!bd) return NULL;
    
    size_t len = bd->length + (bd->negative ? 2 : 1);
    char* str = (char*)malloc(len);
    if (!str) return NULL;
    
    if (bd->negative) {
        str[0] = '-';
        strcpy(str + 1, bd->digits);
    } else {
        strcpy(str, bd->digits);
    }
    
    return str;
}

// Compare two BigDecimals
// Returns: -1 if a < b, 0 if a == b, 1 if a > b
int tto_bigdec_compare(BigDecimal* a, BigDecimal* b) {
    if (!a || !b) return 0;
    
    // Handle sign differences
    if (a->negative && !b->negative) return -1;  // negative < positive
    if (!a->negative && b->negative) return 1;   // positive > negative
    
    // Both same sign - compare magnitudes
    int len_cmp = a->length - b->length;
    
    if (len_cmp != 0) {
        // Different lengths
        if (a->negative) {
            // Both negative: longer magnitude = smaller value
            return len_cmp > 0 ? -1 : 1;
        } else {
            // Both positive: longer magnitude = larger value
            return len_cmp > 0 ? 1 : -1;
        }
    }
    
    // Same length - compare digit by digit
    int digit_cmp = strcmp(a->digits, b->digits);
    
    if (a->negative) {
        // Both negative: reverse comparison
        return digit_cmp > 0 ? -1 : (digit_cmp < 0 ? 1 : 0);
    } else {
        // Both positive: normal comparison
        return digit_cmp > 0 ? 1 : (digit_cmp < 0 ? -1 : 0);
    }
}

// Free BigDecimal
void tto_bigdec_free(BigDecimal* bd) {
    if (!bd) return;
    
    bd->refcount--;
    if (bd->refcount <= 0) {
        free(bd->digits);
        free(bd);
    }
}

// ============================================================================
// Phase 3.3: SSO String (Placeholder - minimal implementation)
// ============================================================================

// Create SSO string from C string
SSOString* tto_sso_create(const char* str) {
    if (!str) return NULL;
    
    SSOString* sso = (SSOString*)malloc(sizeof(SSOString));
    if (!sso) return NULL;
    
    size_t len = strlen(str);
    
    if (len <= 23) {
        // Use inline storage (SSO)
        memset(sso->data.inline_data, 0, 24);  // Clear all bytes
        strcpy(sso->data.inline_data, str);
        sso->flags = (len << 1);  // Shift left 1, bit 0 = 0 (not heap)
    } else {
        // Use heap storage
        sso->data.heap.heap_ptr = (char*)malloc(len + 1);
        if (!sso->data.heap.heap_ptr) {
            free(sso);
            return NULL;
        }
        strcpy(sso->data.heap.heap_ptr, str);
        sso->data.heap.length = len;
        sso->data.heap.capacity = len + 1;
        sso->flags = 1;  // Bit 0 = 1 (is heap)
    }
    
    return sso;
}

// Get string data (works for both SSO and heap)
const char* tto_sso_data(SSOString* sso) {
    if (!sso) return NULL;
    
    if (sso->flags & 1) {
        // Heap string
        return sso->data.heap.heap_ptr;
    } else {
        // SSO string
        return sso->data.inline_data;
    }
}

// Free SSO string
void tto_sso_free(SSOString* sso) {
    if (!sso) return;
    
    if (sso->flags & 1) {
        // Heap string - free the pointer
        free(sso->data.heap.heap_ptr);
    }
    // SSO string - nothing to free (inline storage)
    
    free(sso);
}
