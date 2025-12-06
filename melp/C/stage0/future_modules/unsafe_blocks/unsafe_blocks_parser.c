#include "unsafe_blocks.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Create context
UnsafeModuleContext* unsafe_context_create(void) {
    UnsafeModuleContext* ctx = (UnsafeModuleContext*)malloc(sizeof(UnsafeModuleContext));
    ctx->contexts = (UnsafeContext**)malloc(sizeof(UnsafeContext*) * 10);
    ctx->context_count = 0;
    ctx->context_capacity = 10;
    
    ctx->operations = (UnsafeOpNode**)malloc(sizeof(UnsafeOpNode*) * 20);
    ctx->op_count = 0;
    ctx->op_capacity = 20;
    
    ctx->raw_pointers = (RawPointer**)malloc(sizeof(RawPointer*) * 10);
    ctx->pointer_count = 0;
    ctx->pointer_capacity = 10;
    
    ctx->current_context = NULL;
    ctx->error_message = NULL;
    return ctx;
}

// Free context
void unsafe_context_free(UnsafeModuleContext* ctx) {
    if (!ctx) return;
    
    for (int i = 0; i < ctx->context_count; i++) {
        if (ctx->contexts[i]) {
            free(ctx->contexts[i]->name);
            free(ctx->contexts[i]);
        }
    }
    free(ctx->contexts);
    
    for (int i = 0; i < ctx->op_count; i++) {
        if (ctx->operations[i]) {
            free(ctx->operations[i]->code);
            free(ctx->operations[i]->target);
            free(ctx->operations[i]->type_from);
            free(ctx->operations[i]->type_to);
            free(ctx->operations[i]);
        }
    }
    free(ctx->operations);
    
    for (int i = 0; i < ctx->pointer_count; i++) {
        if (ctx->raw_pointers[i]) {
            free(ctx->raw_pointers[i]->name);
            free(ctx->raw_pointers[i]->pointee_type);
            free(ctx->raw_pointers[i]);
        }
    }
    free(ctx->raw_pointers);
    
    if (ctx->error_message) free(ctx->error_message);
    free(ctx);
}

// Get context type from string
UnsafeContextType unsafe_get_context_type(const char* type_str) {
    if (strstr(type_str, "unsafe fn")) return UNSAFE_FUNCTION;
    if (strstr(type_str, "unsafe trait")) return UNSAFE_TRAIT;
    if (strstr(type_str, "unsafe {")) return UNSAFE_BLOCK;
    return UNSAFE_UNKNOWN;
}

// Get operation type from string
UnsafeOp unsafe_get_operation(const char* op_str) {
    if (strstr(op_str, "*")) return UNSAFE_OP_DEREF_RAW;
    if (strstr(op_str, "as *")) return UNSAFE_OP_RAW_PTR_CREATE;
    if (strstr(op_str, "cast")) return UNSAFE_OP_RAW_PTR_CAST;
    if (strstr(op_str, "transmute")) return UNSAFE_OP_TRANSMUTE;
    if (strstr(op_str, "asm!")) return UNSAFE_OP_INLINE_ASM;
    if (strstr(op_str, "extern")) return UNSAFE_OP_FFI_CALL;
    return UNSAFE_OP_UNKNOWN;
}

// Parse unsafe context: "unsafe { ... }"
bool unsafe_parse_context(UnsafeModuleContext* ctx, const char* input) {
    char buffer[256];
    strncpy(buffer, input, 255);
    buffer[255] = '\0';
    
    UnsafeContext* unsafe_ctx = (UnsafeContext*)malloc(sizeof(UnsafeContext));
    unsafe_ctx->type = unsafe_get_context_type(buffer);
    unsafe_ctx->start_line = 0;
    unsafe_ctx->end_line = 0;
    unsafe_ctx->is_active = true;
    
    // Extract name if it's a function
    if (unsafe_ctx->type == UNSAFE_FUNCTION) {
        char* fn_start = strstr(buffer, "fn ");
        if (fn_start) {
            fn_start += 3;
            while (isspace(*fn_start)) fn_start++;
            char* fn_end = strchr(fn_start, '(');
            if (fn_end) {
                unsafe_ctx->name = strndup(fn_start, fn_end - fn_start);
            } else {
                unsafe_ctx->name = strdup("unnamed_fn");
            }
        } else {
            unsafe_ctx->name = strdup("unnamed_fn");
        }
    } else {
        unsafe_ctx->name = strdup("unsafe_block");
    }
    
    // Set as current context
    ctx->current_context = unsafe_ctx;
    unsafe_add_context(ctx, unsafe_ctx);
    
    return true;
}

// Parse unsafe operation: "*raw_ptr" or "std::mem::transmute(x)"
bool unsafe_parse_operation(UnsafeModuleContext* ctx, const char* input) {
    UnsafeOpNode* op = (UnsafeOpNode*)malloc(sizeof(UnsafeOpNode));
    op->code = strdup(input);
    op->target = NULL;
    op->type_from = NULL;
    op->type_to = NULL;
    op->context = ctx->current_context;
    op->line = 0;
    
    // Detect operation type
    if (strstr(input, "*") && strstr(input, "=")) {
        // Dereference operation: "let x = *raw_ptr"
        op->op = UNSAFE_OP_DEREF_RAW;
        
        char* eq = strchr(input, '=');
        if (eq) {
            char* ptr_start = eq + 1;
            while (isspace(*ptr_start)) ptr_start++;
            if (*ptr_start == '*') {
                ptr_start++;
                op->target = strdup(ptr_start);
            }
        }
        
    } else if (strstr(input, "as *")) {
        // Raw pointer creation: "x as *const i32"
        op->op = UNSAFE_OP_RAW_PTR_CREATE;
        
    } else if (strstr(input, "transmute")) {
        // Type transmutation
        op->op = UNSAFE_OP_TRANSMUTE;
        
    } else if (strstr(input, "asm!")) {
        // Inline assembly
        op->op = UNSAFE_OP_INLINE_ASM;
        
    } else if (strstr(input, "extern") && strstr(input, "(")) {
        // FFI call
        op->op = UNSAFE_OP_FFI_CALL;
        
    } else {
        op->op = UNSAFE_OP_UNKNOWN;
    }
    
    unsafe_add_operation(ctx, op);
    return true;
}

// Parse raw pointer declaration: "let ptr: *const i32"
bool unsafe_parse_raw_pointer(UnsafeModuleContext* ctx, const char* input) {
    char buffer[256];
    strncpy(buffer, input, 255);
    buffer[255] = '\0';
    
    RawPointer* ptr = (RawPointer*)malloc(sizeof(RawPointer));
    
    // Parse: "let ptr: *const i32" or "let ptr: *mut i32"
    char* let = strstr(buffer, "let ");
    if (!let) {
        free(ptr);
        return false;
    }
    
    // Extract name
    char* name_start = let + 4;
    while (isspace(*name_start)) name_start++;
    char* name_end = strchr(name_start, ':');
    if (!name_end) {
        free(ptr);
        return false;
    }
    
    ptr->name = strndup(name_start, name_end - name_start);
    
    // Check if const or mut
    char* type_start = name_end + 1;
    while (isspace(*type_start)) type_start++;
    
    if (strstr(type_start, "*const")) {
        ptr->is_const = true;
        ptr->is_mutable = false;
        type_start = strstr(type_start, "*const") + 6;
    } else if (strstr(type_start, "*mut")) {
        ptr->is_const = false;
        ptr->is_mutable = true;
        type_start = strstr(type_start, "*mut") + 4;
    } else {
        ptr->is_const = false;
        ptr->is_mutable = false;
    }
    
    // Extract pointee type
    while (isspace(*type_start)) type_start++;
    ptr->pointee_type = strdup(type_start);
    
    unsafe_add_raw_pointer(ctx, ptr);
    return true;
}

// Add unsafe context
void unsafe_add_context(UnsafeModuleContext* ctx, UnsafeContext* unsafe_ctx) {
    if (ctx->context_count >= ctx->context_capacity) {
        ctx->context_capacity *= 2;
        ctx->contexts = (UnsafeContext**)realloc(ctx->contexts,
                                                  sizeof(UnsafeContext*) * ctx->context_capacity);
    }
    ctx->contexts[ctx->context_count++] = unsafe_ctx;
}

// Add operation
void unsafe_add_operation(UnsafeModuleContext* ctx, UnsafeOpNode* op) {
    if (ctx->op_count >= ctx->op_capacity) {
        ctx->op_capacity *= 2;
        ctx->operations = (UnsafeOpNode**)realloc(ctx->operations,
                                                   sizeof(UnsafeOpNode*) * ctx->op_capacity);
    }
    ctx->operations[ctx->op_count++] = op;
}

// Add raw pointer
void unsafe_add_raw_pointer(UnsafeModuleContext* ctx, RawPointer* ptr) {
    if (ctx->pointer_count >= ctx->pointer_capacity) {
        ctx->pointer_capacity *= 2;
        ctx->raw_pointers = (RawPointer**)realloc(ctx->raw_pointers,
                                                   sizeof(RawPointer*) * ctx->pointer_capacity);
    }
    ctx->raw_pointers[ctx->pointer_count++] = ptr;
}

// Find context by name
UnsafeContext* unsafe_find_context(UnsafeModuleContext* ctx, const char* name) {
    for (int i = 0; i < ctx->context_count; i++) {
        if (strcmp(ctx->contexts[i]->name, name) == 0) {
            return ctx->contexts[i];
        }
    }
    return NULL;
}

// Check if currently in unsafe context
bool unsafe_is_in_unsafe_context(UnsafeModuleContext* ctx) {
    return ctx->current_context != NULL && ctx->current_context->is_active;
}
