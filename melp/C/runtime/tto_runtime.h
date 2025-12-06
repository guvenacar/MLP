#ifndef TTO_RUNTIME_H
#define TTO_RUNTIME_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>  // for size_t

// ============================================================================
// TTO Runtime Support
// ============================================================================
// Phase 3: Runtime functions for Transparent Type Optimization
// Handles overflow detection, type promotions, and runtime operations

// ============================================================================
// Phase 3.1: Overflow Detection & Promotion
// ============================================================================

// Check if INT64 addition would overflow
bool tto_would_overflow_add(int64_t a, int64_t b);

// Check if INT64 multiplication would overflow
bool tto_would_overflow_mul(int64_t a, int64_t b);

// Check if INT64 subtraction would overflow
bool tto_would_overflow_sub(int64_t a, int64_t b);

// Perform INT64 addition with overflow check
// Returns: 0 = success (result in *out), 1 = overflow (need promotion)
int tto_safe_add_i64(int64_t a, int64_t b, int64_t* out);

// Perform INT64 multiplication with overflow check
int tto_safe_mul_i64(int64_t a, int64_t b, int64_t* out);

// Perform INT64 subtraction with overflow check
int tto_safe_sub_i64(int64_t a, int64_t b, int64_t* out);

// ============================================================================
// Phase 3.2: BigDecimal Structure & Operations
// ============================================================================

typedef struct BigDecimal {
    char* digits;      // String representation of number
    size_t length;
    bool is_negative;
    int decimal_point; // Position of decimal point
} BigDecimal;

// Promote INT64 to BigDecimal
BigDecimal* tto_promote_to_bigdecimal(int64_t value);

// BigDecimal operations (stubs for Phase 3.2)
BigDecimal* bigdecimal_add(BigDecimal* a, BigDecimal* b);
BigDecimal* bigdecimal_mul(BigDecimal* a, BigDecimal* b);
BigDecimal* bigdecimal_sub(BigDecimal* a, BigDecimal* b);
void bigdecimal_free(BigDecimal* bd);

// ============================================================================
// Phase 3.3: SSO String Forward Declarations (to be implemented)
// ============================================================================

// Small String Optimization structure (23 bytes inline)
typedef struct {
    char data[23];
    uint8_t length;  // 0-23
} SSOString;

// Heap string structure
typedef struct {
    char* data;
    size_t length;
    size_t capacity;
    int refcount;
} HeapString;

// String operations (stubs for Phase 3.3)
SSOString sso_create(const char* str);
HeapString* heap_string_create(const char* str);
bool sso_would_fit(const char* str);

#endif
