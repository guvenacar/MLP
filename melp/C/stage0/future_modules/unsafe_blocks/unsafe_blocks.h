#ifndef UNSAFE_BLOCKS_H
#define UNSAFE_BLOCKS_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

// Unsafe context types
typedef enum {
    UNSAFE_BLOCK,       // unsafe { ... }
    UNSAFE_FUNCTION,    // unsafe fn name() { ... }
    UNSAFE_TRAIT,       // unsafe trait/impl
    UNSAFE_UNKNOWN
} UnsafeContextType;

// Unsafe operations
typedef enum {
    UNSAFE_OP_DEREF_RAW,       // Dereference raw pointer
    UNSAFE_OP_RAW_PTR_CREATE,  // Create raw pointer
    UNSAFE_OP_RAW_PTR_CAST,    // Cast raw pointer
    UNSAFE_OP_CALL_UNSAFE_FN,  // Call unsafe function
    UNSAFE_OP_ACCESS_UNION,    // Access union field
    UNSAFE_OP_INLINE_ASM,      // Inline assembly
    UNSAFE_OP_FFI_CALL,        // Foreign function call
    UNSAFE_OP_TRANSMUTE,       // Type transmutation
    UNSAFE_OP_STATIC_MUT,      // Access mutable static
    UNSAFE_OP_UNKNOWN
} UnsafeOp;

// Unsafe context definition
typedef struct {
    UnsafeContextType type;
    char* name;          // Function/block name (if any)
    int start_line;
    int end_line;
    bool is_active;      // Currently inside unsafe context
} UnsafeContext;

// Unsafe operation node
typedef struct {
    UnsafeOp op;
    char* code;          // The unsafe code
    char* target;        // Target variable/pointer
    char* type_from;     // For cast/transmute: source type
    char* type_to;       // For cast/transmute: target type
    UnsafeContext* context;  // Parent context
    int line;
} UnsafeOpNode;

// Raw pointer definition
typedef struct {
    char* name;
    char* pointee_type;
    bool is_mutable;
    bool is_const;
} RawPointer;

// Unsafe module context
typedef struct {
    UnsafeContext** contexts;
    int context_count;
    int context_capacity;
    
    UnsafeOpNode** operations;
    int op_count;
    int op_capacity;
    
    RawPointer** raw_pointers;
    int pointer_count;
    int pointer_capacity;
    
    UnsafeContext* current_context;  // Currently active unsafe context
    char* error_message;
} UnsafeModuleContext;

// Lifecycle functions
UnsafeModuleContext* unsafe_context_create(void);
void unsafe_context_free(UnsafeModuleContext* ctx);

// Parser functions
bool unsafe_parse_context(UnsafeModuleContext* ctx, const char* input);
bool unsafe_parse_operation(UnsafeModuleContext* ctx, const char* input);
bool unsafe_parse_raw_pointer(UnsafeModuleContext* ctx, const char* input);
UnsafeContextType unsafe_get_context_type(const char* type_str);
UnsafeOp unsafe_get_operation(const char* op_str);

// Codegen functions
void unsafe_codegen_context(FILE* out, UnsafeContext* ctx);
void unsafe_codegen_operation(FILE* out, UnsafeOpNode* op);
void unsafe_codegen_full(UnsafeModuleContext* ctx, const char* output_file);

// Utility functions
void unsafe_add_context(UnsafeModuleContext* ctx, UnsafeContext* unsafe_ctx);
void unsafe_add_operation(UnsafeModuleContext* ctx, UnsafeOpNode* op);
void unsafe_add_raw_pointer(UnsafeModuleContext* ctx, RawPointer* ptr);
UnsafeContext* unsafe_find_context(UnsafeModuleContext* ctx, const char* name);
bool unsafe_is_in_unsafe_context(UnsafeModuleContext* ctx);

#endif
