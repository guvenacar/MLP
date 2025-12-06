#ifndef RUNTIME_TTO_H
#define RUNTIME_TTO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../codegen_context/tto_types.h"

// ============================================================================
// TTO Runtime Support - Phase 3
// ============================================================================
// Runtime functions for TTO optimization:
// - Overflow detection and promotion
// - BigDecimal operations
// - SSO string management
// - Memory management

// ============================================================================
// Phase 3.1: Overflow Detection & Promotion
// ============================================================================

// Check if INT64 addition will overflow
bool tto_runtime_add_will_overflow(int64_t a, int64_t b);

// Check if INT64 subtraction will overflow
bool tto_runtime_sub_will_overflow(int64_t a, int64_t b);

// Check if INT64 multiplication will overflow
bool tto_runtime_mul_will_overflow(int64_t a, int64_t b);

// Safe addition with overflow check (returns true if promoted to BigDecimal)
bool tto_runtime_safe_add(int64_t a, int64_t b, int64_t* result);

// Safe subtraction with overflow check
bool tto_runtime_safe_sub(int64_t a, int64_t b, int64_t* result);

// Safe multiplication with overflow check
bool tto_runtime_safe_mul(int64_t a, int64_t b, int64_t* result);

// ============================================================================
// Phase 3.2: BigDecimal Runtime (Placeholder)
// ============================================================================

// BigDecimal structure (opaque pointer for now)
typedef struct BigDecimal BigDecimal;

// Create BigDecimal from INT64
BigDecimal* tto_bigdec_from_int64(int64_t value);

// Create BigDecimal from string
BigDecimal* tto_bigdec_from_string(const char* str);

// BigDecimal operations (to be implemented)
BigDecimal* tto_bigdec_add(BigDecimal* a, BigDecimal* b);
BigDecimal* tto_bigdec_sub(BigDecimal* a, BigDecimal* b);
BigDecimal* tto_bigdec_mul(BigDecimal* a, BigDecimal* b);
BigDecimal* tto_bigdec_div(BigDecimal* a, BigDecimal* b);

// Convert BigDecimal to string
char* tto_bigdec_to_string(BigDecimal* bd);

// Compare two BigDecimals (-1: a<b, 0: a==b, 1: a>b)
int tto_bigdec_compare(BigDecimal* a, BigDecimal* b);

// Free BigDecimal
void tto_bigdec_free(BigDecimal* bd);

// ============================================================================
// Phase 3.3: SSO String (Placeholder)
// ============================================================================

// SSO String structure (23 bytes inline + 1 byte flag)
typedef struct {
    union {
        char inline_data[24];  // Small string storage (≤23 bytes + null)
        struct {
            char* heap_ptr;    // Heap pointer for large strings
            size_t length;     // String length
            size_t capacity;   // Allocated capacity
        } heap;
    } data;
    uint8_t flags;  // Bit 0: is_heap, Bit 1-7: inline_length
} SSOString;

// Create SSO string from C string
SSOString* tto_sso_create(const char* str);

// Get string data (works for both SSO and heap)
const char* tto_sso_data(SSOString* sso);

// Free SSO string
void tto_sso_free(SSOString* sso);

#endif
