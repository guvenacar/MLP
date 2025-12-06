#ifndef RESULT_TYPE_H
#define RESULT_TYPE_H

#include "../../lexer.h"

// Result<T, E> type - Rust-style error handling
typedef struct {
    void* ok_value;       // Success value (generic pointer)
    void* err_value;      // Error value (generic pointer)
    int is_ok;            // 1 if Ok, 0 if Err
    char* ok_type;        // Type name of ok value
    char* err_type;       // Type name of error value
} Result;

// Create Result<T, E>
Result* result_create_ok(void* value, const char* type);
Result* result_create_err(void* error, const char* type);

// Check result status
int result_is_ok(Result* result);
int result_is_err(Result* result);

// Unwrap (panics if Err)
void* result_unwrap(Result* result);

// Unwrap or default
void* result_unwrap_or(Result* result, void* default_value);

// Free result
void result_free(Result* result);

#endif
