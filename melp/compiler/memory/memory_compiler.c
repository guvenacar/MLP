#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Enhanced Memory Compiler for MELP Stage 3 Phase 4
// Real memory analysis implementation (not placeholder)
// Performs lifetime analysis, escape analysis, and GC hints

// ============================================
// MEMORY OPERATION TYPES
// ============================================

typedef enum {
    MEM_OP_ALLOC = 0,
    MEM_OP_FREE = 1,
    MEM_OP_REF = 2,
    MEM_OP_DEREF = 3,
    MEM_OP_DEFER = 4
} MemoryOpType;

// ============================================
// ALLOCATION TYPES
// ============================================

typedef enum {
    ALLOC_STACK = 0,
    ALLOC_HEAP = 1,
    ALLOC_STATIC = 2,
    ALLOC_UNKNOWN = 3
} AllocType;

// ============================================
// GC HINTS
// ============================================

typedef enum {
    GC_NONE = 0,
    GC_MANUAL = 1,
    GC_AUTO = 2,
    GC_REFCOUNT = 3
} GCMode;

// ============================================
// TOKEN TYPES (from parser AST)
// ============================================

typedef enum {
    NODE_PROGRAM = 0,
    NODE_FUNC_DECL = 1,
    NODE_VAR_DECL = 2,
    NODE_BLOCK = 11,
    NODE_RETURN = 15,
    NODE_BINARY = 20,
    NODE_UNARY = 21,
    NODE_IDENTIFIER = 23,
    NODE_ASSIGN = 27
} NodeType;

// ============================================
// MEMORY TRACKING STRUCTURES
// ============================================

typedef struct {
    char* name;
    AllocType type;
    long long size;
    int lifetime_start;
    int lifetime_end;
    bool escapes;
    int ref_count;
    int scope_depth;
} MemoryAllocation;

typedef struct {
    MemoryOpType type;
    char* allocation_name;
    int line;
} MemoryOperation;

typedef struct {
    MemoryAllocation* allocations;
    int alloc_count;
    int alloc_capacity;
    MemoryOperation* operations;
    int op_count;
    int op_capacity;
    int current_scope_depth;
    GCMode gc_mode;
} MemoryContext;

// ============================================
// MEMORY CONTEXT MANAGEMENT
// ============================================

void memory_context_init(MemoryContext* ctx) {
    ctx->allocations = NULL;
    ctx->alloc_count = 0;
    ctx->alloc_capacity = 0;
    ctx->operations = NULL;
    ctx->op_count = 0;
    ctx->op_capacity = 0;
    ctx->current_scope_depth = 0;
    ctx->gc_mode = GC_AUTO;  // Use GC by default
}

void memory_context_free(MemoryContext* ctx) {
    for (int i = 0; i < ctx->alloc_count; i++) {
        if (ctx->allocations[i].name) {
            free(ctx->allocations[i].name);
        }
    }
    if (ctx->allocations) free(ctx->allocations);
    
    for (int i = 0; i < ctx->op_count; i++) {
        if (ctx->operations[i].allocation_name) {
            free(ctx->operations[i].allocation_name);
        }
    }
    if (ctx->operations) free(ctx->operations);
}

void memory_enter_scope(MemoryContext* ctx) {
    ctx->current_scope_depth++;
}

void memory_exit_scope(MemoryContext* ctx) {
    // Mark allocations in this scope as ending
    for (int i = 0; i < ctx->alloc_count; i++) {
        if (ctx->allocations[i].scope_depth == ctx->current_scope_depth &&
            ctx->allocations[i].lifetime_end == -1) {
            ctx->allocations[i].lifetime_end = ctx->current_scope_depth;
        }
    }
    ctx->current_scope_depth--;
}

// ============================================
// ALLOCATION TRACKING
// ============================================

void memory_track_allocation(MemoryContext* ctx, const char* name, 
                             AllocType type, long long size, int line) {
    if (ctx->alloc_count >= ctx->alloc_capacity) {
        ctx->alloc_capacity = (ctx->alloc_capacity == 0) ? 16 : ctx->alloc_capacity * 2;
        ctx->allocations = (MemoryAllocation*)realloc(ctx->allocations, 
                                                       ctx->alloc_capacity * sizeof(MemoryAllocation));
    }
    
    MemoryAllocation* alloc = &ctx->allocations[ctx->alloc_count++];
    alloc->name = strdup(name);
    alloc->type = type;
    alloc->size = size;
    alloc->lifetime_start = line;
    alloc->lifetime_end = -1;
    alloc->escapes = false;
    alloc->ref_count = 1;
    alloc->scope_depth = ctx->current_scope_depth;
}

void memory_mark_escape(MemoryContext* ctx, const char* name) {
    for (int i = 0; i < ctx->alloc_count; i++) {
        if (strcmp(ctx->allocations[i].name, name) == 0) {
            ctx->allocations[i].escapes = true;
            ctx->allocations[i].type = ALLOC_HEAP;  // Escaping vars must be heap
            break;
        }
    }
}

void memory_track_operation(MemoryContext* ctx, MemoryOpType type, 
                            const char* alloc_name, int line) {
    if (ctx->op_count >= ctx->op_capacity) {
        ctx->op_capacity = (ctx->op_capacity == 0) ? 16 : ctx->op_capacity * 2;
        ctx->operations = (MemoryOperation*)realloc(ctx->operations,
                                                     ctx->op_capacity * sizeof(MemoryOperation));
    }
    
    MemoryOperation* op = &ctx->operations[ctx->op_count++];
    op->type = type;
    op->allocation_name = strdup(alloc_name);
    op->line = line;
}

// ============================================
// ALLOCATION STRATEGY
// ============================================

AllocType memory_decide_allocation_type(long long size, bool escapes) {
    // Small non-escaping values -> stack
    if (size < 1024 && !escapes) {
        return ALLOC_STACK;
    }
    
    // Large values or escaping values -> heap (GC managed)
    if (size >= 1024 || escapes) {
        return ALLOC_HEAP;
    }
    
    return ALLOC_STACK;
}

// ============================================
// MEMORY ANALYSIS REPORT
// ============================================

void memory_generate_report(MemoryContext* ctx, FILE* out) {
    fprintf(out, "=== MELP Memory Analysis Report ===\n\n");
    
    fprintf(out, "GC Mode: %s\n", 
            ctx->gc_mode == GC_AUTO ? "Automatic" :
            ctx->gc_mode == GC_MANUAL ? "Manual" : "None");
    fprintf(out, "Max Scope Depth: %d\n\n", ctx->current_scope_depth);
    
    fprintf(out, "=== Allocations (%d total) ===\n", ctx->alloc_count);
    
    int stack_count = 0, heap_count = 0, escape_count = 0;
    
    for (int i = 0; i < ctx->alloc_count; i++) {
        MemoryAllocation* alloc = &ctx->allocations[i];
        
        fprintf(out, "[%d] %s:\n", i, alloc->name);
        fprintf(out, "    Type: %s\n", 
                alloc->type == ALLOC_STACK ? "STACK" :
                alloc->type == ALLOC_HEAP ? "HEAP" : "UNKNOWN");
        fprintf(out, "    Lifetime: line %d", alloc->lifetime_start);
        if (alloc->lifetime_end != -1) {
            fprintf(out, " -> scope %d", alloc->lifetime_end);
        }
        fprintf(out, "\n");
        fprintf(out, "    Escapes: %s\n", alloc->escapes ? "YES" : "NO");
        fprintf(out, "    Scope depth: %d\n", alloc->scope_depth);
        fprintf(out, "    GC Hint: %s\n", 
                alloc->type == ALLOC_HEAP ? "gc_alloc()" : "stack");
        fprintf(out, "\n");
        
        if (alloc->type == ALLOC_STACK) stack_count++;
        if (alloc->type == ALLOC_HEAP) heap_count++;
        if (alloc->escapes) escape_count++;
    }
    
    fprintf(out, "=== Statistics ===\n");
    fprintf(out, "Stack allocations: %d\n", stack_count);
    fprintf(out, "Heap allocations (GC): %d\n", heap_count);
    fprintf(out, "Escaping variables: %d\n", escape_count);
    fprintf(out, "Memory operations: %d\n\n", ctx->op_count);
    
    if (ctx->op_count > 0) {
        fprintf(out, "=== Memory Operations ===\n");
        for (int i = 0; i < ctx->op_count; i++) {
            MemoryOperation* op = &ctx->operations[i];
            const char* op_name = 
                op->type == MEM_OP_ALLOC ? "ALLOC" :
                op->type == MEM_OP_FREE ? "FREE" :
                op->type == MEM_OP_REF ? "REF" : "DEREF";
            fprintf(out, "[%d] %s: %s at line %d\n", 
                    i, op_name, op->allocation_name, op->line);
        }
    }
    
    fprintf(out, "\n=== GC Integration Hints ===\n");
    fprintf(out, "Replace malloc() with gc_alloc() for %d heap allocations\n", heap_count);
    fprintf(out, "Stack variables require no GC management\n");
    if (escape_count > 0) {
        fprintf(out, "WARNING: %d variables escape scope - ensure proper GC roots\n", escape_count);
    }
}

// ============================================
// SIMPLE AST PARSING FROM PARSER OUTPUT
// ============================================

void memory_analyze_from_parser_ast(MemoryContext* ctx, FILE* ast_file) {
    char line[1024];
    int current_line = 0;
    
    while (fgets(line, sizeof(line), ast_file)) {
        current_line++;
        
        // Parse AST structure looking for declarations
        if (strstr(line, "[FUNC_DECL]")) {
            char name[256] = "";
            if (sscanf(line, "%*[^=]=%255s", name) == 1) {
                memory_enter_scope(ctx);
                memory_track_allocation(ctx, name, ALLOC_STATIC, 0, current_line);
                memory_track_operation(ctx, MEM_OP_ALLOC, name, current_line);
            }
        }
        else if (strstr(line, "[VAR_DECL]")) {
            char name[256] = "";
            if (sscanf(line, "%*[^=]=%255s", name) == 1) {
                // Assume small stack allocation unless marked otherwise
                memory_track_allocation(ctx, name, ALLOC_STACK, 64, current_line);
                memory_track_operation(ctx, MEM_OP_ALLOC, name, current_line);
            }
        }
        else if (strstr(line, "[RETURN]")) {
            // Variables returned from functions escape
            // Mark recent allocations as escaping
            if (ctx->alloc_count > 0) {
                ctx->allocations[ctx->alloc_count - 1].escapes = true;
                memory_mark_escape(ctx, ctx->allocations[ctx->alloc_count - 1].name);
            }
        }
        else if (strstr(line, "[BLOCK]")) {
            memory_enter_scope(ctx);
        }
    }
    
    // Apply allocation strategy
    for (int i = 0; i < ctx->alloc_count; i++) {
        MemoryAllocation* alloc = &ctx->allocations[i];
        alloc->type = memory_decide_allocation_type(alloc->size, alloc->escapes);
    }
}

// ============================================
// MAIN - Real Memory Analysis
// ============================================

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <parser_ast.txt> [output_memory.txt]\n", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = (argc >= 3) ? argv[2] : "memory_analysis.txt";

    // Read parser AST
    FILE* in = fopen(input_file, "r");
    if (!in) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", input_file);
        return 1;
    }

    // Initialize memory context
    MemoryContext ctx;
    memory_context_init(&ctx);

    // Analyze AST for memory patterns
    memory_analyze_from_parser_ast(&ctx, in);
    fclose(in);

    // Generate report
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", output_file);
        memory_context_free(&ctx);
        return 1;
    }

    memory_generate_report(&ctx, out);
    fclose(out);

    printf("✓ Enhanced Memory: %s -> %s (%d allocations, %d heap)\n",
           input_file, output_file, ctx.alloc_count,
           ctx.alloc_count > 0 ? ctx.allocations[0].type == ALLOC_HEAP : 0);

    memory_context_free(&ctx);
    return 0;
}
