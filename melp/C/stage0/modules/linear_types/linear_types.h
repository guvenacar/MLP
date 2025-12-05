#ifndef LINEAR_TYPES_H
#define LINEAR_TYPES_H

#include <stddef.h>
#include <stdbool.h>

// ============================================================================
// MLP LINEAR TYPE SYSTEM - MODULE #61
// ============================================================================
// Linear types: use-once semantics, prevents accidental copies
// Affine types: use-at-most-once semantics
// Move semantics: explicit ownership transfer
// Based on linear logic and Rust's move semantics

typedef enum {
    LINEAR_TYPE,    // Must be used exactly once
    AFFINE_TYPE,    // Can be used at most once
    NORMAL_TYPE     // Can be used any number of times
} TypeLinearity;

typedef enum {
    USE_UNINITIALIZED,
    USE_INITIALIZED,
    USE_MOVED,
    USE_CONSUMED
} UseState;

typedef struct LinearVar {
    char* name;
    TypeLinearity linearity;
    UseState state;
    size_t use_count;
    bool is_moved;
    struct LinearVar* next;
} LinearVar;

typedef struct LinearContext {
    LinearVar** vars;
    size_t var_count;
    size_t var_capacity;
    size_t total_moves;
    size_t total_uses;
    size_t violations;
} LinearContext;

// Context management
LinearContext* linear_context_create(void);
void linear_context_destroy(LinearContext* ctx);

// Variable tracking
LinearVar* linear_var_create(const char* name, TypeLinearity linearity);
void linear_var_use(LinearContext* ctx, LinearVar* var);
void linear_var_move(LinearContext* ctx, LinearVar* var);
bool linear_var_check(LinearContext* ctx, LinearVar* var);
void linear_var_free(LinearVar* var);

// Register variable
void linear_register(LinearContext* ctx, LinearVar* var);

// Find variable
LinearVar* linear_find(LinearContext* ctx, const char* name);

// Validation
bool linear_validate_all(LinearContext* ctx);

// Parser
LinearVar* linear_parse_declaration(const char* line);

// Codegen
char* codegen_linear_check(LinearVar* var);

#endif
