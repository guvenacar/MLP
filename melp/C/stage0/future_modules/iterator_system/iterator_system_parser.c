#include "iterator_system.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Create context
IteratorContext* iter_context_create(void) {
    IteratorContext* ctx = (IteratorContext*)malloc(sizeof(IteratorContext));
    ctx->declarations = (IteratorDecl**)malloc(sizeof(IteratorDecl*) * 10);
    ctx->decl_count = 0;
    ctx->decl_capacity = 10;
    
    ctx->operations = (IteratorOpNode**)malloc(sizeof(IteratorOpNode*) * 20);
    ctx->op_count = 0;
    ctx->op_capacity = 20;
    
    ctx->error_message = NULL;
    return ctx;
}

// Free context
void iter_context_free(IteratorContext* ctx) {
    if (!ctx) return;
    
    for (int i = 0; i < ctx->decl_count; i++) {
        if (ctx->declarations[i]) {
            free(ctx->declarations[i]->var_name);
            free(ctx->declarations[i]->source);
            free(ctx->declarations[i]->transform);
            free(ctx->declarations[i]);
        }
    }
    free(ctx->declarations);
    
    for (int i = 0; i < ctx->op_count; i++) {
        if (ctx->operations[i]) {
            free(ctx->operations[i]->iterator_name);
            free(ctx->operations[i]->function);
            free(ctx->operations[i]->result_var);
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

// Get iterator type from string
IteratorType iter_get_type(const char* type_str) {
    if (strstr(type_str, "array")) return ITER_ARRAY;
    if (strstr(type_str, "range")) return ITER_RANGE;
    if (strstr(type_str, "map")) return ITER_MAP;
    if (strstr(type_str, "filter")) return ITER_FILTER;
    if (strstr(type_str, "chain")) return ITER_CHAIN;
    if (strstr(type_str, "zip")) return ITER_ZIP;
    if (strstr(type_str, "enumerate")) return ITER_ENUMERATE;
    return ITER_UNKNOWN;
}

// Get operation type from string
IteratorOp iter_get_operation(const char* op_str) {
    if (strcmp(op_str, "new") == 0) return ITER_OP_NEW;
    if (strcmp(op_str, "next") == 0) return ITER_OP_NEXT;
    if (strcmp(op_str, "has_next") == 0) return ITER_OP_HAS_NEXT;
    if (strcmp(op_str, "map") == 0) return ITER_OP_MAP;
    if (strcmp(op_str, "filter") == 0) return ITER_OP_FILTER;
    if (strcmp(op_str, "reduce") == 0) return ITER_OP_REDUCE;
    if (strcmp(op_str, "collect") == 0) return ITER_OP_COLLECT;
    if (strcmp(op_str, "chain") == 0) return ITER_OP_CHAIN;
    if (strcmp(op_str, "zip") == 0) return ITER_OP_ZIP;
    if (strcmp(op_str, "enumerate") == 0) return ITER_OP_ENUMERATE;
    if (strcmp(op_str, "take") == 0) return ITER_OP_TAKE;
    if (strcmp(op_str, "skip") == 0) return ITER_OP_SKIP;
    if (strcmp(op_str, "for_each") == 0) return ITER_OP_FOREACH;
    if (strcmp(op_str, "count") == 0) return ITER_OP_COUNT;
    return ITER_OP_UNKNOWN;
}

// Parse iterator declaration: "let iter = array.iter()"
bool iter_parse_declaration(IteratorContext* ctx, const char* input) {
    char buffer[256];
    strncpy(buffer, input, 255);
    buffer[255] = '\0';
    
    // Find "let" keyword
    char* let = strstr(buffer, "let ");
    if (!let) return false;
    
    // Extract variable name
    char* name_start = let + 4;
    while (isspace(*name_start)) name_start++;
    char* name_end = strchr(name_start, '=');
    if (!name_end) return false;
    
    IteratorDecl* decl = (IteratorDecl*)malloc(sizeof(IteratorDecl));
    
    // Extract and trim variable name
    int name_len = name_end - name_start;
    decl->var_name = (char*)malloc(name_len + 1);
    strncpy(decl->var_name, name_start, name_len);
    decl->var_name[name_len] = '\0';
    
    // Trim whitespace from variable name
    char* p = decl->var_name + strlen(decl->var_name) - 1;
    while (p >= decl->var_name && isspace(*p)) *p-- = '\0';
    
    // Extract source and determine type
    char* source_start = name_end + 1;
    while (isspace(*source_start)) source_start++;
    
    if (strstr(source_start, ".iter()")) {
        decl->type = ITER_ARRAY;
        char* dot = strstr(source_start, ".iter");
        decl->source = strndup(source_start, dot - source_start);
    } else if (strstr(source_start, "..")) {
        decl->type = ITER_RANGE;
        decl->source = strdup(source_start);
    } else {
        decl->type = ITER_UNKNOWN;
        decl->source = strdup(source_start);
    }
    
    decl->transform = NULL;
    decl->is_lazy = true;
    
    iter_add_declaration(ctx, decl);
    return true;
}

// Parse iterator operation: "iter.map(fn).filter(pred).collect()"
bool iter_parse_operation(IteratorContext* ctx, const char* input) {
    IteratorOpNode* op = (IteratorOpNode*)malloc(sizeof(IteratorOpNode));
    op->arg_count = 0;
    op->result_var = NULL;
    
    // Parse operation chain
    if (strstr(input, ".map(")) {
        op->op = ITER_OP_MAP;
        char* map_start = strstr(input, ".map(");
        char* fn_start = map_start + 5;
        char* fn_end = strchr(fn_start, ')');
        if (fn_end) {
            op->function = strndup(fn_start, fn_end - fn_start);
        }
        
        // Extract iterator name
        op->iterator_name = strndup(input, map_start - input);
        
    } else if (strstr(input, ".filter(")) {
        op->op = ITER_OP_FILTER;
        char* filter_start = strstr(input, ".filter(");
        char* pred_start = filter_start + 8;
        char* pred_end = strchr(pred_start, ')');
        if (pred_end) {
            op->function = strndup(pred_start, pred_end - pred_start);
        }
        
        op->iterator_name = strndup(input, filter_start - input);
        
    } else if (strstr(input, ".collect()")) {
        op->op = ITER_OP_COLLECT;
        char* collect_pos = strstr(input, ".collect");
        op->iterator_name = strndup(input, collect_pos - input);
        op->function = NULL;
        
    } else if (strstr(input, ".reduce(")) {
        op->op = ITER_OP_REDUCE;
        char* reduce_start = strstr(input, ".reduce(");
        char* fn_start = reduce_start + 8;
        char* fn_end = strchr(fn_start, ')');
        if (fn_end) {
            op->function = strndup(fn_start, fn_end - fn_start);
        }
        
        op->iterator_name = strndup(input, reduce_start - input);
        
    } else {
        op->op = ITER_OP_UNKNOWN;
        op->iterator_name = strdup(input);
        op->function = NULL;
    }
    
    iter_add_operation(ctx, op);
    return true;
}

// Add declaration to context
void iter_add_declaration(IteratorContext* ctx, IteratorDecl* decl) {
    if (ctx->decl_count >= ctx->decl_capacity) {
        ctx->decl_capacity *= 2;
        ctx->declarations = (IteratorDecl**)realloc(ctx->declarations,
                                                     sizeof(IteratorDecl*) * ctx->decl_capacity);
    }
    ctx->declarations[ctx->decl_count++] = decl;
}

// Add operation to context
void iter_add_operation(IteratorContext* ctx, IteratorOpNode* op) {
    if (ctx->op_count >= ctx->op_capacity) {
        ctx->op_capacity *= 2;
        ctx->operations = (IteratorOpNode**)realloc(ctx->operations,
                                                     sizeof(IteratorOpNode*) * ctx->op_capacity);
    }
    ctx->operations[ctx->op_count++] = op;
}

// Find declaration by name
IteratorDecl* iter_find_declaration(IteratorContext* ctx, const char* name) {
    for (int i = 0; i < ctx->decl_count; i++) {
        if (strcmp(ctx->declarations[i]->var_name, name) == 0) {
            return ctx->declarations[i];
        }
    }
    return NULL;
}
