#include "smart_pointers.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Declare strdup if not available
#ifndef _GNU_SOURCE
extern char* strdup(const char*);
#endif

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

SmartPointerContext* smart_pointer_context_create(void) {
    SmartPointerContext* ctx = (SmartPointerContext*)malloc(sizeof(SmartPointerContext));
    if (!ctx) return NULL;
    
    ctx->rcs = (Rc**)malloc(sizeof(Rc*) * 16);
    ctx->rc_count = 0;
    ctx->rc_capacity = 16;
    
    ctx->arcs = (Arc**)malloc(sizeof(Arc*) * 16);
    ctx->arc_count = 0;
    ctx->arc_capacity = 16;
    
    ctx->boxes = (Box**)malloc(sizeof(Box*) * 16);
    ctx->box_count = 0;
    ctx->box_capacity = 16;
    
    ctx->weaks = (Weak**)malloc(sizeof(Weak*) * 16);
    ctx->weak_count = 0;
    ctx->weak_capacity = 16;
    
    ctx->total_allocations = 0;
    ctx->total_deallocations = 0;
    ctx->current_memory = 0;
    ctx->peak_memory = 0;
    
    return ctx;
}

void smart_pointer_context_destroy(SmartPointerContext* ctx) {
    if (!ctx) return;
    
    // Don't drop pointers here - they should be dropped manually
    // This just frees the tracking arrays
    free(ctx->rcs);
    free(ctx->arcs);
    free(ctx->boxes);
    free(ctx->weaks);
    
    free(ctx);
}

void smart_pointer_context_reset(SmartPointerContext* ctx) {
    if (!ctx) return;
    
    smart_pointer_context_destroy(ctx);
    *ctx = *smart_pointer_context_create();
}

// ============================================================================
// Rc<T> OPERATIONS
// ============================================================================

Rc* rc_new(SmartPointerContext* ctx, const char* type_name, void* data, size_t data_size) {
    // Allocate Rc
    Rc* rc = (Rc*)malloc(sizeof(Rc));
    if (!rc) return NULL;
    
    // Allocate inner
    RcInner* inner = (RcInner*)malloc(sizeof(RcInner));
    if (!inner) {
        free(rc);
        return NULL;
    }
    
    // Allocate data
    inner->data = malloc(data_size);
    if (!inner->data) {
        free(inner);
        free(rc);
        return NULL;
    }
    
    // Copy data
    memcpy(inner->data, data, data_size);
    
    // Initialize inner
    inner->strong_count = 1;
    inner->weak_count = 0;
    inner->data_size = data_size;
    inner->type_name = strdup(type_name);
    inner->drop_fn = NULL;
    
    // Initialize Rc
    rc->inner = inner;
    rc->mangled_name = smart_pointer_mangle_name(SP_RC, type_name);
    
    // Update memory tracking
    ctx->total_allocations++;
    ctx->current_memory += data_size + sizeof(Rc) + sizeof(RcInner);
    if (ctx->current_memory > ctx->peak_memory) {
        ctx->peak_memory = ctx->current_memory;
    }
    
    // Register in context
    smart_pointer_register_rc(ctx, rc);
    
    return rc;
}

Rc* rc_clone(SmartPointerContext* ctx, Rc* rc) {
    if (!rc || !rc->inner) return NULL;
    
    // Increment strong count
    rc->inner->strong_count++;
    
    // Allocate new Rc pointing to same inner
    Rc* cloned = (Rc*)malloc(sizeof(Rc));
    if (!cloned) return NULL;
    
    cloned->inner = rc->inner;
    cloned->mangled_name = strdup(rc->mangled_name);
    
    // Register in context
    smart_pointer_register_rc(ctx, cloned);
    
    return cloned;
}

void rc_drop(SmartPointerContext* ctx, Rc* rc) {
    if (!rc) return;
    
    if (rc->inner) {
        // Decrement strong count
        rc->inner->strong_count--;
        
        // If last strong reference, drop data
        if (rc->inner->strong_count == 0) {
            // Call custom drop function if exists
            if (rc->inner->drop_fn) {
                rc->inner->drop_fn(rc->inner->data);
            }
            
            // Free data
            free(rc->inner->data);
            ctx->total_deallocations++;
            ctx->current_memory -= rc->inner->data_size;
            
            // If no weak references, free inner
            if (rc->inner->weak_count == 0) {
                free(rc->inner->type_name);
                free(rc->inner);
                ctx->current_memory -= sizeof(RcInner);
            }
        }
    }
    
    // Free Rc
    free(rc->mangled_name);
    free(rc);
    ctx->current_memory -= sizeof(Rc);
}

size_t rc_strong_count(Rc* rc) {
    if (!rc || !rc->inner) return 0;
    return rc->inner->strong_count;
}

size_t rc_weak_count(Rc* rc) {
    if (!rc || !rc->inner) return 0;
    return rc->inner->weak_count;
}

void* rc_get(Rc* rc) {
    if (!rc || !rc->inner) return NULL;
    return rc->inner->data;
}

Weak* rc_downgrade(SmartPointerContext* ctx, Rc* rc) {
    if (!rc || !rc->inner) return NULL;
    
    // Increment weak count
    rc->inner->weak_count++;
    
    // Create Weak
    Weak* weak = (Weak*)malloc(sizeof(Weak));
    if (!weak) return NULL;
    
    weak->rc_inner = rc->inner;
    weak->arc_inner = NULL;
    weak->parent_kind = SP_RC;
    weak->mangled_name = smart_pointer_mangle_name(SP_WEAK, rc->inner->type_name);
    
    // Register in context
    smart_pointer_register_weak(ctx, weak);
    
    return weak;
}

// ============================================================================
// Arc<T> OPERATIONS
// ============================================================================

Arc* arc_new(SmartPointerContext* ctx, const char* type_name, void* data, size_t data_size) {
    // Allocate Arc
    Arc* arc = (Arc*)malloc(sizeof(Arc));
    if (!arc) return NULL;
    
    // Allocate inner
    ArcInner* inner = (ArcInner*)malloc(sizeof(ArcInner));
    if (!inner) {
        free(arc);
        return NULL;
    }
    
    // Allocate data
    inner->data = malloc(data_size);
    if (!inner->data) {
        free(inner);
        free(arc);
        return NULL;
    }
    
    // Copy data
    memcpy(inner->data, data, data_size);
    
    // Initialize inner (atomic)
    atomic_init(&inner->strong_count, 1);
    atomic_init(&inner->weak_count, 0);
    inner->data_size = data_size;
    inner->type_name = strdup(type_name);
    inner->drop_fn = NULL;
    
    // Initialize Arc
    arc->inner = inner;
    arc->mangled_name = smart_pointer_mangle_name(SP_ARC, type_name);
    
    // Update memory tracking
    ctx->total_allocations++;
    ctx->current_memory += data_size + sizeof(Arc) + sizeof(ArcInner);
    if (ctx->current_memory > ctx->peak_memory) {
        ctx->peak_memory = ctx->current_memory;
    }
    
    // Register in context
    smart_pointer_register_arc(ctx, arc);
    
    return arc;
}

Arc* arc_clone(SmartPointerContext* ctx, Arc* arc) {
    if (!arc || !arc->inner) return NULL;
    
    // Atomic increment strong count
    atomic_fetch_add(&arc->inner->strong_count, 1);
    
    // Allocate new Arc pointing to same inner
    Arc* cloned = (Arc*)malloc(sizeof(Arc));
    if (!cloned) return NULL;
    
    cloned->inner = arc->inner;
    cloned->mangled_name = strdup(arc->mangled_name);
    
    // Register in context
    smart_pointer_register_arc(ctx, cloned);
    
    return cloned;
}

void arc_drop(SmartPointerContext* ctx, Arc* arc) {
    if (!arc) return;
    
    if (arc->inner) {
        // Atomic decrement strong count
        size_t old_count = atomic_fetch_sub(&arc->inner->strong_count, 1);
        
        // If last strong reference, drop data
        if (old_count == 1) {
            // Call custom drop function if exists
            if (arc->inner->drop_fn) {
                arc->inner->drop_fn(arc->inner->data);
            }
            
            // Free data
            free(arc->inner->data);
            ctx->total_deallocations++;
            ctx->current_memory -= arc->inner->data_size;
            
            // If no weak references, free inner
            size_t weak_count = atomic_load(&arc->inner->weak_count);
            if (weak_count == 0) {
                free(arc->inner->type_name);
                free(arc->inner);
                ctx->current_memory -= sizeof(ArcInner);
            }
        }
    }
    
    // Free Arc
    free(arc->mangled_name);
    free(arc);
    ctx->current_memory -= sizeof(Arc);
}

size_t arc_strong_count(Arc* arc) {
    if (!arc || !arc->inner) return 0;
    return atomic_load(&arc->inner->strong_count);
}

size_t arc_weak_count(Arc* arc) {
    if (!arc || !arc->inner) return 0;
    return atomic_load(&arc->inner->weak_count);
}

void* arc_get(Arc* arc) {
    if (!arc || !arc->inner) return NULL;
    return arc->inner->data;
}

Weak* arc_downgrade(SmartPointerContext* ctx, Arc* arc) {
    if (!arc || !arc->inner) return NULL;
    
    // Atomic increment weak count
    atomic_fetch_add(&arc->inner->weak_count, 1);
    
    // Create Weak
    Weak* weak = (Weak*)malloc(sizeof(Weak));
    if (!weak) return NULL;
    
    weak->rc_inner = NULL;
    weak->arc_inner = arc->inner;
    weak->parent_kind = SP_ARC;
    weak->mangled_name = smart_pointer_mangle_name(SP_WEAK, arc->inner->type_name);
    
    // Register in context
    smart_pointer_register_weak(ctx, weak);
    
    return weak;
}

// ============================================================================
// Box<T> OPERATIONS
// ============================================================================

Box* box_new(SmartPointerContext* ctx, const char* type_name, void* data, size_t data_size) {
    // Allocate Box
    Box* box = (Box*)malloc(sizeof(Box));
    if (!box) return NULL;
    
    // Allocate data
    box->data = malloc(data_size);
    if (!box->data) {
        free(box);
        return NULL;
    }
    
    // Copy data
    memcpy(box->data, data, data_size);
    
    // Initialize Box
    box->data_size = data_size;
    box->type_name = strdup(type_name);
    box->mangled_name = smart_pointer_mangle_name(SP_BOX, type_name);
    box->drop_fn = NULL;
    box->moved = false;
    
    // Update memory tracking
    ctx->total_allocations++;
    ctx->current_memory += data_size + sizeof(Box);
    if (ctx->current_memory > ctx->peak_memory) {
        ctx->peak_memory = ctx->current_memory;
    }
    
    // Register in context
    smart_pointer_register_box(ctx, box);
    
    return box;
}

void box_drop(SmartPointerContext* ctx, Box* box) {
    if (!box) return;
    
    // Don't drop if moved
    if (box->moved) {
        free(box);
        return;
    }
    
    // Call custom drop function if exists
    if (box->drop_fn) {
        box->drop_fn(box->data);
    }
    
    // Free data
    free(box->data);
    free(box->type_name);
    free(box->mangled_name);
    
    ctx->total_deallocations++;
    ctx->current_memory -= (box->data_size + sizeof(Box));
    
    free(box);
}

void* box_get(Box* box) {
    if (!box || box->moved) return NULL;
    return box->data;
}

Box* box_move(Box* box) {
    if (!box) return NULL;
    box->moved = true;
    return box;
}

// ============================================================================
// Weak<T> OPERATIONS
// ============================================================================

Weak* weak_from_rc(SmartPointerContext* ctx, Rc* rc) {
    return rc_downgrade(ctx, rc);
}

Weak* weak_from_arc(SmartPointerContext* ctx, Arc* arc) {
    return arc_downgrade(ctx, arc);
}

Rc* weak_upgrade_rc(SmartPointerContext* ctx, Weak* weak) {
    if (!weak || weak->parent_kind != SP_RC || !weak->rc_inner) return NULL;
    
    // Check if still valid
    if (weak->rc_inner->strong_count == 0) return NULL;
    
    // Create new Rc
    Rc* rc = (Rc*)malloc(sizeof(Rc));
    if (!rc) return NULL;
    
    rc->inner = weak->rc_inner;
    rc->mangled_name = strdup(weak->mangled_name);
    
    // Increment strong count
    weak->rc_inner->strong_count++;
    
    // Register in context
    smart_pointer_register_rc(ctx, rc);
    
    return rc;
}

Arc* weak_upgrade_arc(SmartPointerContext* ctx, Weak* weak) {
    if (!weak || weak->parent_kind != SP_ARC || !weak->arc_inner) return NULL;
    
    // Check if still valid (atomic load)
    size_t strong_count = atomic_load(&weak->arc_inner->strong_count);
    if (strong_count == 0) return NULL;
    
    // Create new Arc
    Arc* arc = (Arc*)malloc(sizeof(Arc));
    if (!arc) return NULL;
    
    arc->inner = weak->arc_inner;
    arc->mangled_name = strdup(weak->mangled_name);
    
    // Atomic increment strong count
    atomic_fetch_add(&weak->arc_inner->strong_count, 1);
    
    // Register in context
    smart_pointer_register_arc(ctx, arc);
    
    return arc;
}

void weak_drop(SmartPointerContext* ctx, Weak* weak) {
    if (!weak) return;
    
    // Decrement weak count
    if (weak->parent_kind == SP_RC && weak->rc_inner) {
        weak->rc_inner->weak_count--;
        
        // If no strong and no weak, free inner
        if (weak->rc_inner->strong_count == 0 && weak->rc_inner->weak_count == 0) {
            free(weak->rc_inner->type_name);
            free(weak->rc_inner);
            ctx->current_memory -= sizeof(RcInner);
        }
    } else if (weak->parent_kind == SP_ARC && weak->arc_inner) {
        size_t old_weak = atomic_fetch_sub(&weak->arc_inner->weak_count, 1);
        
        // If no strong and no weak, free inner
        size_t strong_count = atomic_load(&weak->arc_inner->strong_count);
        if (strong_count == 0 && old_weak == 1) {
            free(weak->arc_inner->type_name);
            free(weak->arc_inner);
            ctx->current_memory -= sizeof(ArcInner);
        }
    }
    
    // Free Weak
    free(weak->mangled_name);
    free(weak);
}

bool weak_is_valid(Weak* weak) {
    if (!weak) return false;
    
    if (weak->parent_kind == SP_RC && weak->rc_inner) {
        return weak->rc_inner->strong_count > 0;
    } else if (weak->parent_kind == SP_ARC && weak->arc_inner) {
        size_t strong_count = atomic_load(&weak->arc_inner->strong_count);
        return strong_count > 0;
    }
    
    return false;
}

// ============================================================================
// MEMORY TRACKING
// ============================================================================

size_t smart_pointer_memory_usage(SmartPointerContext* ctx) {
    if (!ctx) return 0;
    return ctx->current_memory;
}

size_t smart_pointer_peak_memory(SmartPointerContext* ctx) {
    if (!ctx) return 0;
    return ctx->peak_memory;
}

void smart_pointer_stats(SmartPointerContext* ctx, 
                         size_t* total_allocs, 
                         size_t* total_deallocs,
                         size_t* current_memory) {
    if (!ctx) return;
    
    if (total_allocs) *total_allocs = ctx->total_allocations;
    if (total_deallocs) *total_deallocs = ctx->total_deallocations;
    if (current_memory) *current_memory = ctx->current_memory;
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

char* smart_pointer_mangle_name(SmartPointerKind kind, const char* type_name) {
    const char* prefix;
    switch (kind) {
        case SP_RC:   prefix = "Rc_"; break;
        case SP_ARC:  prefix = "Arc_"; break;
        case SP_BOX:  prefix = "Box_"; break;
        case SP_WEAK: prefix = "Weak_"; break;
        default:      prefix = "Unknown_"; break;
    }
    
    size_t len = strlen(prefix) + strlen(type_name) + 1;
    char* mangled = (char*)malloc(len);
    if (!mangled) return NULL;
    
    snprintf(mangled, len, "%s%s", prefix, type_name);
    return mangled;
}

void smart_pointer_register_rc(SmartPointerContext* ctx, Rc* rc) {
    if (!ctx || !rc) return;
    
    // Expand if needed
    if (ctx->rc_count >= ctx->rc_capacity) {
        ctx->rc_capacity *= 2;
        ctx->rcs = (Rc**)realloc(ctx->rcs, sizeof(Rc*) * ctx->rc_capacity);
    }
    
    ctx->rcs[ctx->rc_count++] = rc;
}

void smart_pointer_register_arc(SmartPointerContext* ctx, Arc* arc) {
    if (!ctx || !arc) return;
    
    // Expand if needed
    if (ctx->arc_count >= ctx->arc_capacity) {
        ctx->arc_capacity *= 2;
        ctx->arcs = (Arc**)realloc(ctx->arcs, sizeof(Arc*) * ctx->arc_capacity);
    }
    
    ctx->arcs[ctx->arc_count++] = arc;
}

void smart_pointer_register_box(SmartPointerContext* ctx, Box* box) {
    if (!ctx || !box) return;
    
    // Expand if needed
    if (ctx->box_count >= ctx->box_capacity) {
        ctx->box_capacity *= 2;
        ctx->boxes = (Box**)realloc(ctx->boxes, sizeof(Box*) * ctx->box_capacity);
    }
    
    ctx->boxes[ctx->box_count++] = box;
}

void smart_pointer_register_weak(SmartPointerContext* ctx, Weak* weak) {
    if (!ctx || !weak) return;
    
    // Expand if needed
    if (ctx->weak_count >= ctx->weak_capacity) {
        ctx->weak_capacity *= 2;
        ctx->weaks = (Weak**)realloc(ctx->weaks, sizeof(Weak*) * ctx->weak_capacity);
    }
    
    ctx->weaks[ctx->weak_count++] = weak;
}
