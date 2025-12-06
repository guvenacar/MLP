#define _POSIX_C_SOURCE 200809L
#include "linear_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// LINEAR TYPE SYSTEM IMPLEMENTATION
// ============================================================================

LinearContext* linear_context_create(void) {
    LinearContext* ctx = malloc(sizeof(LinearContext));
    if (!ctx) return NULL;
    
    ctx->var_capacity = 16;
    ctx->vars = malloc(sizeof(LinearVar*) * ctx->var_capacity);
    if (!ctx->vars) {
        free(ctx);
        return NULL;
    }
    
    ctx->var_count = 0;
    ctx->total_moves = 0;
    ctx->total_uses = 0;
    ctx->violations = 0;
    
    return ctx;
}

void linear_context_destroy(LinearContext* ctx) {
    if (!ctx) return;
    
    for (size_t i = 0; i < ctx->var_count; i++) {
        linear_var_free(ctx->vars[i]);
    }
    
    free(ctx->vars);
    free(ctx);
}

LinearVar* linear_var_create(const char* name, TypeLinearity linearity) {
    LinearVar* var = malloc(sizeof(LinearVar));
    if (!var) return NULL;
    
    var->name = strdup(name);
    if (!var->name) {
        free(var);
        return NULL;
    }
    
    var->linearity = linearity;
    var->state = USE_INITIALIZED;
    var->use_count = 0;
    var->is_moved = false;
    var->next = NULL;
    
    return var;
}

void linear_var_free(LinearVar* var) {
    if (!var) return;
    free(var->name);
    free(var);
}

void linear_register(LinearContext* ctx, LinearVar* var) {
    if (!ctx || !var) return;
    
    // Resize if needed
    if (ctx->var_count >= ctx->var_capacity) {
        ctx->var_capacity *= 2;
        ctx->vars = realloc(ctx->vars, sizeof(LinearVar*) * ctx->var_capacity);
    }
    
    ctx->vars[ctx->var_count++] = var;
}

LinearVar* linear_find(LinearContext* ctx, const char* name) {
    if (!ctx || !name) return NULL;
    
    for (size_t i = 0; i < ctx->var_count; i++) {
        if (strcmp(ctx->vars[i]->name, name) == 0) {
            return ctx->vars[i];
        }
    }
    
    return NULL;
}

void linear_var_use(LinearContext* ctx, LinearVar* var) {
    if (!ctx || !var) return;
    
    if (var->state == USE_MOVED) {
        fprintf(stderr, "ERROR: Use of moved value '%s'\n", var->name);
        ctx->violations++;
        return;
    }
    
    if (var->state == USE_CONSUMED) {
        fprintf(stderr, "ERROR: Value '%s' already consumed\n", var->name);
        ctx->violations++;
        return;
    }
    
    var->use_count++;
    ctx->total_uses++;
    
    // Linear types: consume on first use
    if (var->linearity == LINEAR_TYPE) {
        if (var->use_count > 1) {
            fprintf(stderr, "ERROR: Linear value '%s' used more than once\n", var->name);
            ctx->violations++;
        }
        var->state = USE_CONSUMED;
    }
    
    // Affine types: can be used at most once
    if (var->linearity == AFFINE_TYPE && var->use_count > 1) {
        fprintf(stderr, "ERROR: Affine value '%s' used more than once\n", var->name);
        ctx->violations++;
    }
}

void linear_var_move(LinearContext* ctx, LinearVar* var) {
    if (!ctx || !var) return;
    
    if (var->state == USE_MOVED) {
        fprintf(stderr, "ERROR: Value '%s' already moved\n", var->name);
        ctx->violations++;
        return;
    }
    
    if (var->state == USE_CONSUMED) {
        fprintf(stderr, "ERROR: Cannot move consumed value '%s'\n", var->name);
        ctx->violations++;
        return;
    }
    
    var->is_moved = true;
    var->state = USE_MOVED;
    ctx->total_moves++;
}

bool linear_var_check(LinearContext* ctx, LinearVar* var) {
    if (!ctx || !var) return false;
    
    // Linear types must be used exactly once
    if (var->linearity == LINEAR_TYPE) {
        if (var->use_count == 0 && var->state != USE_MOVED) {
            fprintf(stderr, "WARNING: Linear value '%s' never used\n", var->name);
            return false;
        }
        if (var->use_count > 1) {
            return false;
        }
    }
    
    // Affine types can be unused or used once
    if (var->linearity == AFFINE_TYPE) {
        if (var->use_count > 1) {
            return false;
        }
    }
    
    return true;
}

bool linear_validate_all(LinearContext* ctx) {
    if (!ctx) return false;
    
    bool all_valid = true;
    
    for (size_t i = 0; i < ctx->var_count; i++) {
        if (!linear_var_check(ctx, ctx->vars[i])) {
            all_valid = false;
        }
    }
    
    return all_valid;
}

// ============================================================================
// PARSER
// ============================================================================

LinearVar* linear_parse_declaration(const char* line) {
    if (!line) return NULL;
    
    // Expected format:
    // "linear x: i32"  -> LINEAR_TYPE
    // "affine y: i32"  -> AFFINE_TYPE
    // "let z: i32"     -> NORMAL_TYPE
    
    char keyword[32];
    char name[128];
    char type[64];
    
    int result = sscanf(line, "%31s %127[^:]: %63s", keyword, name, type);
    if (result != 3) return NULL;
    
    TypeLinearity linearity = NORMAL_TYPE;
    
    if (strcmp(keyword, "linear") == 0) {
        linearity = LINEAR_TYPE;
    } else if (strcmp(keyword, "affine") == 0) {
        linearity = AFFINE_TYPE;
    }
    
    return linear_var_create(name, linearity);
}

// ============================================================================
// CODE GENERATION
// ============================================================================

char* codegen_linear_check(LinearVar* var) {
    if (!var) return NULL;
    
    char* code = malloc(256);
    if (!code) return NULL;
    
    snprintf(code, 256,
        "    ; Linear check for '%s'\n"
        "    ; linearity=%d, state=%d, uses=%zu\n",
        var->name,
        var->linearity,
        var->state,
        var->use_count
    );
    
    return code;
}
