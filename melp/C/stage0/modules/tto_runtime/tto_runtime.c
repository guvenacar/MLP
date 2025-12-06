/**
 * TTO Runtime Implementation
 * Transparent Type Optimization runtime support
 */

#include "tto_runtime.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

// ============================================================================
// Phase 3.1: Overflow Detection & Promotion
// ============================================================================

bool tto_would_overflow_add(int64_t a, int64_t b) {
    return (b > 0 && a > INT64_MAX - b) || (b < 0 && a < INT64_MIN - b);
}

bool tto_would_overflow_sub(int64_t a, int64_t b) {
    return (b < 0 && a > INT64_MAX + b) || (b > 0 && a < INT64_MIN + b);
}

bool tto_would_overflow_mul(int64_t a, int64_t b) {
    if (a == 0 || b == 0) return false;
    if (a > 0 && b > 0 && a > INT64_MAX / b) return true;
    if (a > 0 && b < 0 && b < INT64_MIN / a) return true;
    if (a < 0 && b > 0 && a < INT64_MIN / b) return true;
    if (a < 0 && b < 0 && a < INT64_MAX / b) return true;
    return false;
}

bool tto_safe_add_i64(int64_t a, int64_t b, int64_t* result) {
    if (tto_would_overflow_add(a, b)) {
        *result = 0;
        return true;  // Overflow occurred
    }
    *result = a + b;
    return false;  // No overflow
}

bool tto_safe_sub_i64(int64_t a, int64_t b, int64_t* result) {
    if (tto_would_overflow_sub(a, b)) {
        *result = 0;
        return true;
    }
    *result = a - b;
    return false;
}

bool tto_safe_mul_i64(int64_t a, int64_t b, int64_t* result) {
    if (tto_would_overflow_mul(a, b)) {
        *result = 0;
        return true;
    }
    *result = a * b;
    return false;
}

// ============================================================================
// Phase 3.2: BigDecimal Runtime Library
// ============================================================================

BigDecimal* bigdec_from_i64(int64_t value) {
    BigDecimal* bd = malloc(sizeof(BigDecimal));
    bd->digits = malloc(32);
    bd->negative = (value < 0);
    if (bd->negative) value = -value;
    snprintf(bd->digits, 32, "%lld", (long long)value);
    bd->length = strlen(bd->digits);
    bd->scale = 0;
    bd->refcount = 1;
    return bd;
}

BigDecimal* bigdec_from_double(double value) {
    BigDecimal* bd = malloc(sizeof(BigDecimal));
    bd->digits = malloc(64);
    bd->negative = (value < 0);
    if (bd->negative) value = -value;
    snprintf(bd->digits, 64, "%.15g", value);
    bd->length = strlen(bd->digits);
    bd->scale = 15;
    bd->refcount = 1;
    return bd;
}

BigDecimal* bigdec_from_string(const char* str) {
    BigDecimal* bd = malloc(sizeof(BigDecimal));
    size_t len = strlen(str);
    bd->digits = malloc(len + 1);
    strcpy(bd->digits, str);
    bd->negative = (str[0] == '-');
    bd->length = bd->negative ? len - 1 : len;
    bd->scale = 0;
    bd->refcount = 1;
    return bd;
}

BigDecimal* bigdec_add(BigDecimal* a, BigDecimal* b) {
    // Simplified implementation
    BigDecimal* result = malloc(sizeof(BigDecimal));
    size_t len = a->length + b->length + 10;
    result->digits = malloc(len);
    snprintf(result->digits, len, "(%s+%s)", a->digits, b->digits);
    result->length = strlen(result->digits);
    result->scale = (a->scale > b->scale) ? a->scale : b->scale;
    result->negative = false;
    result->refcount = 1;
    return result;
}

BigDecimal* bigdec_sub(BigDecimal* a, BigDecimal* b) {
    BigDecimal* result = malloc(sizeof(BigDecimal));
    size_t len = a->length + b->length + 10;
    result->digits = malloc(len);
    snprintf(result->digits, len, "(%s-%s)", a->digits, b->digits);
    result->length = strlen(result->digits);
    result->scale = (a->scale > b->scale) ? a->scale : b->scale;
    result->negative = false;
    result->refcount = 1;
    return result;
}

BigDecimal* bigdec_mul(BigDecimal* a, BigDecimal* b) {
    BigDecimal* result = malloc(sizeof(BigDecimal));
    size_t len = a->length + b->length + 10;
    result->digits = malloc(len);
    snprintf(result->digits, len, "(%s*%s)", a->digits, b->digits);
    result->length = strlen(result->digits);
    result->scale = a->scale + b->scale;
    result->negative = (a->negative != b->negative);
    result->refcount = 1;
    return result;
}

BigDecimal* bigdec_div(BigDecimal* a, BigDecimal* b) {
    BigDecimal* result = malloc(sizeof(BigDecimal));
    size_t len = a->length + b->length + 10;
    result->digits = malloc(len);
    snprintf(result->digits, len, "(%s/%s)", a->digits, b->digits);
    result->length = strlen(result->digits);
    result->scale = a->scale - b->scale + 15;
    result->negative = (a->negative != b->negative);
    result->refcount = 1;
    return result;
}

int bigdec_compare(BigDecimal* a, BigDecimal* b) {
    if (a->negative != b->negative) {
        return a->negative ? -1 : 1;
    }
    return strcmp(a->digits, b->digits);
}

char* bigdec_to_string(BigDecimal* bd) {
    size_t len = bd->length + (bd->negative ? 2 : 1);
    char* str = malloc(len);
    if (bd->negative) {
        snprintf(str, len, "-%s", bd->digits);
    } else {
        strcpy(str, bd->digits);
    }
    return str;
}

int64_t bigdec_to_i64(BigDecimal* bd) {
    int64_t value = atoll(bd->digits);
    return bd->negative ? -value : value;
}

double bigdec_to_double(BigDecimal* bd) {
    double value = atof(bd->digits);
    return bd->negative ? -value : value;
}

void bigdec_free(BigDecimal* bd) {
    if (bd) {
        free(bd->digits);
        free(bd);
    }
}

void bigdec_retain(BigDecimal* bd) {
    if (bd) bd->refcount++;
}

void bigdec_release(BigDecimal* bd) {
    if (bd && --bd->refcount == 0) {
        bigdec_free(bd);
    }
}

// ============================================================================
// Phase 3.3: SSO (Small String Optimization)
// ============================================================================

SSOString* sso_create(const char* str) {
    SSOString* sso = malloc(sizeof(SSOString));
    size_t len = strlen(str);
    
    if (len < SSO_MAX_SIZE) {
        // Use small string optimization
        sso->is_heap = false;
        strcpy(sso->storage.sso.data, str);
        sso->storage.sso.length = (uint8_t)len;
    } else {
        // Allocate on heap
        sso->is_heap = true;
        sso->storage.heap.ptr = malloc(len + 1);
        strcpy(sso->storage.heap.ptr, str);
        sso->storage.heap.length = len;
        sso->storage.heap.capacity = len + 1;
        sso->storage.heap.refcount = 1;
    }
    
    return sso;
}

const char* sso_data(SSOString* str) {
    return str->is_heap ? str->storage.heap.ptr : str->storage.sso.data;
}

size_t sso_length(SSOString* str) {
    return str->is_heap ? str->storage.heap.length : str->storage.sso.length;
}

SSOString* sso_concat(SSOString* a, SSOString* b) {
    const char* str_a = sso_data(a);
    const char* str_b = sso_data(b);
    size_t len_a = sso_length(a);
    size_t len_b = sso_length(b);
    size_t total_len = len_a + len_b;
    
    SSOString* result = malloc(sizeof(SSOString));
    
    if (total_len < SSO_MAX_SIZE) {
        // Use SSO
        result->is_heap = false;
        strcpy(result->storage.sso.data, str_a);
        strcat(result->storage.sso.data, str_b);
        result->storage.sso.length = (uint8_t)total_len;
    } else {
        // Use heap
        result->is_heap = true;
        result->storage.heap.ptr = malloc(total_len + 1);
        strcpy(result->storage.heap.ptr, str_a);
        strcat(result->storage.heap.ptr, str_b);
        result->storage.heap.length = total_len;
        result->storage.heap.capacity = total_len + 1;
        result->storage.heap.refcount = 1;
    }
    
    return result;
}

void sso_free(SSOString* str) {
    if (str) {
        if (str->is_heap) {
            free(str->storage.heap.ptr);
        }
        free(str);
    }
}

void sso_retain(SSOString* str) {
    if (str && str->is_heap) {
        str->storage.heap.refcount++;
    }
}

void sso_release(SSOString* str) {
    if (str && str->is_heap && --str->storage.heap.refcount == 0) {
        sso_free(str);
    }
}

// ============================================================================
// Phase 3.4: Memory Management
// ============================================================================

static TTOMemStats mem_stats = {0};

void tto_runtime_init(void) {
    memset(&mem_stats, 0, sizeof(TTOMemStats));
}

void tto_runtime_cleanup(void) {
    // Cleanup would happen here if needed
}

TTOMemStats tto_get_mem_stats(void) {
    return mem_stats;
}
