#include "smart_pointers.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Create context
SmartPtrContext* sp_context_create(void) {
    SmartPtrContext* ctx = (SmartPtrContext*)malloc(sizeof(SmartPtrContext));
    ctx->declarations = (SmartPtrDecl**)malloc(sizeof(SmartPtrDecl*) * 10);
    ctx->decl_count = 0;
    ctx->decl_capacity = 10;
    
    ctx->operations = (SmartPtrOpNode**)malloc(sizeof(SmartPtrOpNode*) * 20);
    ctx->op_count = 0;
    ctx->op_capacity = 20;
    
    ctx->error_message = NULL;
    return ctx;
}

// Free context
void sp_context_free(SmartPtrContext* ctx) {
    if (!ctx) return;
    
    for (int i = 0; i < ctx->decl_count; i++) {
        if (ctx->declarations[i]) {
            free(ctx->declarations[i]->var_name);
            free(ctx->declarations[i]->inner_type);
            free(ctx->declarations[i]->init_value);
            free(ctx->declarations[i]);
        }
    }
    free(ctx->declarations);
    
    for (int i = 0; i < ctx->op_count; i++) {
        if (ctx->operations[i]) {
            free(ctx->operations[i]->ptr_name);
            for (int j = 0; j < ctx->operations[i]->arg_count; j++) {
                free(ctx->operations[i]->args[j]);
            }
            free(ctx->operations[i]);
        }
    }
    free(ctx->operations);
    
    if (ctx->error_message) free(ctx->error_message);
    free(ctx);
}

// Get smart pointer type from string
SmartPtrType sp_get_type(const char* type_str) {
    if (strncmp(type_str, "Box<", 4) == 0) return SP_BOX;
    if (strncmp(type_str, "Rc<", 3) == 0) return SP_RC;
    if (strncmp(type_str, "Arc<", 4) == 0) return SP_ARC;
    if (strncmp(type_str, "Weak<", 5) == 0) return SP_WEAK;
    return SP_UNKNOWN;
}

// Get operation type from string
SmartPtrOp sp_get_operation(const char* op_str) {
    if (strcmp(op_str, "new") == 0) return SP_OP_NEW;
    if (strcmp(op_str, "clone") == 0) return SP_OP_CLONE;
    if (strcmp(op_str, "downgrade") == 0) return SP_OP_DOWNGRADE;
    if (strcmp(op_str, "upgrade") == 0) return SP_OP_UPGRADE;
    if (strcmp(op_str, "strong_count") == 0) return SP_OP_STRONG_COUNT;
    if (strcmp(op_str, "weak_count") == 0) return SP_OP_WEAK_COUNT;
    return SP_OP_UNKNOWN;
}

// Parse smart pointer declaration: "let ptr: Box<i32> = Box::new(42)"
bool sp_parse_declaration(SmartPtrContext* ctx, const char* input) {
    char buffer[256];
    strncpy(buffer, input, 255);
    buffer[255] = '\0';
    
    // Simple parser: find "let", type, and initialization
    char* let = strstr(buffer, "let ");
    if (!let) return false;
    
    // Extract variable name
    char* name_start = let + 4;
    while (isspace(*name_start)) name_start++;
    char* name_end = strchr(name_start, ':');
    if (!name_end) return false;
    
    SmartPtrDecl* decl = (SmartPtrDecl*)malloc(sizeof(SmartPtrDecl));
    decl->var_name = strndup(name_start, name_end - name_start);
    
    // Extract type
    char* type_start = name_end + 1;
    while (isspace(*type_start)) type_start++;
    char* type_end = strchr(type_start, '=');
    if (!type_end) type_end = strchr(type_start, '\n');
    if (!type_end) type_end = type_start + strlen(type_start);
    
    char type_buffer[128];
    int type_len = type_end - type_start;
    strncpy(type_buffer, type_start, type_len);
    type_buffer[type_len] = '\0';
    
    // Trim whitespace
    char* p = type_buffer + strlen(type_buffer) - 1;
    while (p >= type_buffer && isspace(*p)) *p-- = '\0';
    
    decl->type = sp_get_type(type_buffer);
    
    // Extract inner type (T in Box<T>)
    char* inner_start = strchr(type_buffer, '<');
    char* inner_end = strchr(type_buffer, '>');
    if (inner_start && inner_end) {
        decl->inner_type = strndup(inner_start + 1, inner_end - inner_start - 1);
    } else {
        decl->inner_type = strdup("i32");
    }
    
    // Extract initialization value
    char* init_start = strchr(type_end, '=');
    if (init_start) {
        init_start++;
        while (isspace(*init_start)) init_start++;
        decl->init_value = strdup(init_start);
    } else {
        decl->init_value = NULL;
    }
    
    sp_add_declaration(ctx, decl);
    return true;
}

// Parse smart pointer operation: "ptr.clone()" or "Rc::downgrade(&ptr)"
bool sp_parse_operation(SmartPtrContext* ctx, const char* input) {
    SmartPtrOpNode* op = (SmartPtrOpNode*)malloc(sizeof(SmartPtrOpNode));
    op->arg_count = 0;
    
    // Simple operation parsing
    if (strstr(input, ".clone()")) {
        op->op = SP_OP_CLONE;
        char* ptr_end = strstr(input, ".clone");
        if (ptr_end) {
            op->ptr_name = strndup(input, ptr_end - input);
        }
    } else if (strstr(input, "::new(")) {
        op->op = SP_OP_NEW;
        op->ptr_name = strdup("new_ptr");
    } else {
        op->op = SP_OP_UNKNOWN;
        op->ptr_name = strdup("unknown");
    }
    
    sp_add_operation(ctx, op);
    return true;
}

// Add declaration to context
void sp_add_declaration(SmartPtrContext* ctx, SmartPtrDecl* decl) {
    if (ctx->decl_count >= ctx->decl_capacity) {
        ctx->decl_capacity *= 2;
        ctx->declarations = (SmartPtrDecl**)realloc(ctx->declarations, 
                                                     sizeof(SmartPtrDecl*) * ctx->decl_capacity);
    }
    ctx->declarations[ctx->decl_count++] = decl;
}

// Add operation to context
void sp_add_operation(SmartPtrContext* ctx, SmartPtrOpNode* op) {
    if (ctx->op_count >= ctx->op_capacity) {
        ctx->op_capacity *= 2;
        ctx->operations = (SmartPtrOpNode**)realloc(ctx->operations,
                                                     sizeof(SmartPtrOpNode*) * ctx->op_capacity);
    }
    ctx->operations[ctx->op_count++] = op;
}

// Find declaration by name
SmartPtrDecl* sp_find_declaration(SmartPtrContext* ctx, const char* name) {
    for (int i = 0; i < ctx->decl_count; i++) {
        if (strcmp(ctx->declarations[i]->var_name, name) == 0) {
            return ctx->declarations[i];
        }
    }
    return NULL;
}
