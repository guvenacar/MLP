/**
 * MLP Ownership System Module - Core Implementation
 * 
 * Implements ownership rules, borrow checker, and lifetime management
 */

#include "ownership_system.h"

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

OwnershipCtx* ownership_ctx_create(void) {
    OwnershipCtx* ctx = (OwnershipCtx*)calloc(1, sizeof(OwnershipCtx));
    if (!ctx) {
        fprintf(stderr, "Error: Failed to allocate OwnershipCtx\n");
        exit(1);
    }
    return ctx;
}

void ownership_ctx_free(OwnershipCtx* ctx) {
    if (ctx) {
        free(ctx);
    }
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

OwnershipInfo* ownership_find_var(OwnershipCtx* ctx, const char* var_name) {
    for (int i = 0; i < ctx->owner_count; i++) {
        if (strcmp(ctx->owners[i].var_name, var_name) == 0) {
            return &ctx->owners[i];
        }
    }
    return NULL;
}

static void add_violation(OwnershipCtx* ctx, const char* message, 
                         const char* var_name, int line, int type) {
    if (ctx->violation_count >= 64) return;
    
    OwnershipViolation* v = &ctx->violations[ctx->violation_count++];
    strncpy(v->message, message, sizeof(v->message) - 1);
    strncpy(v->var_name, var_name, sizeof(v->var_name) - 1);
    v->line = line;
    v->type = type;
}

const char* ownership_type_str(OwnershipType type) {
    switch (type) {
        case OWNER_OWNED: return "owned";
        case OWNER_BORROWED: return "borrowed (&T)";
        case OWNER_MUT_BORROW: return "mutable borrow (&mut T)";
        case OWNER_MOVED: return "moved";
        default: return "unknown";
    }
}

const char* lifetime_type_str(LifetimeType type) {
    switch (type) {
        case LIFETIME_STATIC: return "static";
        case LIFETIME_NAMED: return "named";
        case LIFETIME_ELIDED: return "elided";
        case LIFETIME_TEMPORARY: return "temporary";
        default: return "unknown";
    }
}

// ============================================================================
// OWNERSHIP OPERATIONS
// ============================================================================

void ownership_add_owner(OwnershipCtx* ctx, const char* var_name, 
                         const char* type_name, int line) {
    if (ctx->owner_count >= 256) return;
    
    OwnershipInfo* info = &ctx->owners[ctx->owner_count++];
    strncpy(info->var_name, var_name, sizeof(info->var_name) - 1);
    strncpy(info->type_name, type_name, sizeof(info->type_name) - 1);
    info->ownership = OWNER_OWNED;
    info->semantics = MOVE_MOVE;  // Default: move semantics
    info->borrow_state = BORROW_NONE;
    info->lifetime = NULL;
    info->borrow_count = 0;
    info->is_moved = false;
    info->declaration_line = line;
    info->last_use_line = line;
}

bool ownership_create_borrow(OwnershipCtx* ctx, const char* var_name,
                             const char* borrowed_as, bool is_mutable, 
                             const char* lifetime_name, int line) {
    // Find the variable
    OwnershipInfo* owner = ownership_find_var(ctx, var_name);
    if (!owner) {
        add_violation(ctx, "Cannot borrow undefined variable", var_name, line,
                     VIOLATION_USE_AFTER_MOVE);
        return false;
    }
    
    // Check if moved
    if (owner->is_moved) {
        add_violation(ctx, "Cannot borrow moved value", var_name, line,
                     VIOLATION_USE_AFTER_MOVE);
        return false;
    }
    
    // Check borrow rules
    if (!ownership_check_borrow_rules(ctx, var_name, is_mutable, line)) {
        return false;
    }
    
    // Create borrow operation
    if (ctx->borrow_count >= 128) return false;
    
    BorrowOp* borrow = &ctx->borrows[ctx->borrow_count++];
    strncpy(borrow->var_name, var_name, sizeof(borrow->var_name) - 1);
    strncpy(borrow->borrowed_as, borrowed_as, sizeof(borrow->borrowed_as) - 1);
    borrow->is_mutable = is_mutable;
    borrow->line = line;
    
    // Find or create lifetime
    for (int i = 0; i < ctx->lifetime_count; i++) {
        if (strcmp(ctx->lifetimes[i].name, lifetime_name) == 0) {
            borrow->lifetime = &ctx->lifetimes[i];
            break;
        }
    }
    
    // Update owner state
    if (is_mutable) {
        owner->borrow_state = BORROW_EXCLUSIVE;
    } else {
        owner->borrow_state = BORROW_SHARED;
        owner->borrow_count++;
    }
    
    return true;
}

bool ownership_execute_move(OwnershipCtx* ctx, const char* from_var,
                            const char* to_var, MoveSemantics semantics, int line) {
    // Find source variable
    OwnershipInfo* from = ownership_find_var(ctx, from_var);
    if (!from) {
        add_violation(ctx, "Cannot move undefined variable", from_var, line,
                     VIOLATION_USE_AFTER_MOVE);
        return false;
    }
    
    // Check if already moved
    if (from->is_moved && semantics == MOVE_MOVE) {
        add_violation(ctx, "Cannot move already moved value", from_var, line,
                     VIOLATION_USE_AFTER_MOVE);
        return false;
    }
    
    // Check if borrowed
    if (from->borrow_state != BORROW_NONE && semantics == MOVE_MOVE) {
        add_violation(ctx, "Cannot move borrowed value", from_var, line,
                     VIOLATION_DOUBLE_BORROW);
        return false;
    }
    
    // Create move operation
    if (ctx->move_count >= 128) return false;
    
    MoveOp* move = &ctx->moves[ctx->move_count++];
    strncpy(move->from_var, from_var, sizeof(move->from_var) - 1);
    strncpy(move->to_var, to_var, sizeof(move->to_var) - 1);
    move->semantics = semantics;
    move->line = line;
    
    // Update ownership
    if (semantics == MOVE_MOVE) {
        from->is_moved = true;
        from->ownership = OWNER_MOVED;
    }
    from->last_use_line = line;
    
    // Create new owner for destination
    OwnershipInfo* to = ownership_find_var(ctx, to_var);
    if (!to) {
        ownership_add_owner(ctx, to_var, from->type_name, line);
    }
    
    return true;
}

bool ownership_can_use(OwnershipCtx* ctx, const char* var_name, int line) {
    OwnershipInfo* info = ownership_find_var(ctx, var_name);
    if (!info) {
        return false;
    }
    
    if (info->is_moved) {
        add_violation(ctx, "Use after move", var_name, line,
                     VIOLATION_USE_AFTER_MOVE);
        return false;
    }
    
    info->last_use_line = line;
    return true;
}

void ownership_end_borrow(OwnershipCtx* ctx, const char* borrowed_name, int line) {
    // Find the borrow operation
    for (int i = 0; i < ctx->borrow_count; i++) {
        if (strcmp(ctx->borrows[i].borrowed_as, borrowed_name) == 0) {
            BorrowOp* borrow = &ctx->borrows[i];
            
            // Find original owner
            OwnershipInfo* owner = ownership_find_var(ctx, borrow->var_name);
            if (owner) {
                if (borrow->is_mutable) {
                    owner->borrow_state = BORROW_NONE;
                } else {
                    owner->borrow_count--;
                    if (owner->borrow_count == 0) {
                        owner->borrow_state = BORROW_NONE;
                    }
                }
            }
            return;
        }
    }
}

// ============================================================================
// LIFETIME MANAGEMENT
// ============================================================================

Lifetime* ownership_create_lifetime(OwnershipCtx* ctx, const char* name, 
                                     LifetimeType type, int start_line) {
    if (ctx->lifetime_count >= 64) return NULL;
    
    Lifetime* lt = &ctx->lifetimes[ctx->lifetime_count++];
    strncpy(lt->name, name, sizeof(lt->name) - 1);
    lt->type = type;
    lt->scope_start = start_line;
    lt->scope_end = -1;  // Unknown yet
    lt->is_active = true;
    
    return lt;
}

void ownership_end_lifetime(OwnershipCtx* ctx, const char* name, int end_line) {
    for (int i = 0; i < ctx->lifetime_count; i++) {
        if (strcmp(ctx->lifetimes[i].name, name) == 0) {
            ctx->lifetimes[i].scope_end = end_line;
            ctx->lifetimes[i].is_active = false;
            return;
        }
    }
}

bool ownership_check_lifetime(OwnershipCtx* ctx, const char* ref_var, 
                              const char* owner_var, int line) {
    OwnershipInfo* owner = ownership_find_var(ctx, owner_var);
    if (!owner) return false;
    
    // Reference cannot outlive owner
    if (owner->last_use_line < line) {
        add_violation(ctx, "Reference outlives owner", ref_var, line,
                     VIOLATION_LIFETIME_OUTLIVE);
        return false;
    }
    
    return true;
}

// ============================================================================
// BORROW CHECKER
// ============================================================================

bool ownership_check_borrow_rules(OwnershipCtx* ctx, const char* var_name, 
                                  bool is_mutable, int line) {
    OwnershipInfo* owner = ownership_find_var(ctx, var_name);
    if (!owner) return false;
    
    // Rule: Multiple immutable borrows OR single mutable borrow
    if (is_mutable) {
        // Requesting mutable borrow
        if (owner->borrow_state != BORROW_NONE) {
            add_violation(ctx, "Cannot borrow as mutable while already borrowed",
                         var_name, line, VIOLATION_DOUBLE_BORROW);
            return false;
        }
    } else {
        // Requesting immutable borrow
        if (owner->borrow_state == BORROW_EXCLUSIVE) {
            add_violation(ctx, "Cannot borrow as immutable while mutably borrowed",
                         var_name, line, VIOLATION_IMMUT_THROUGH_MUT);
            return false;
        }
    }
    
    return true;
}

bool ownership_check_use_after_move(OwnershipCtx* ctx, const char* var_name, int line) {
    return ownership_can_use(ctx, var_name, line);
}

bool ownership_check_dangling(OwnershipCtx* ctx) {
    // Check for dangling references (references that outlive their owners)
    for (int i = 0; i < ctx->borrow_count; i++) {
        BorrowOp* borrow = &ctx->borrows[i];
        OwnershipInfo* owner = ownership_find_var(ctx, borrow->var_name);
        
        if (owner && owner->is_moved) {
            add_violation(ctx, "Dangling reference detected", borrow->borrowed_as,
                         borrow->line, VIOLATION_DANGLING_REF);
        }
    }
    
    return ctx->violation_count == 0;
}

void ownership_report_violations(OwnershipCtx* ctx) {
    if (ctx->violation_count == 0) {
        printf("✅ No ownership violations detected\n");
        return;
    }
    
    printf("❌ OWNERSHIP VIOLATIONS DETECTED: %d\n", ctx->violation_count);
    printf("═══════════════════════════════════════════════════\n");
    
    for (int i = 0; i < ctx->violation_count; i++) {
        OwnershipViolation* v = &ctx->violations[i];
        printf("Line %d: %s\n", v->line, v->message);
        printf("  Variable: %s\n", v->var_name);
        printf("  Type: ");
        
        switch (v->type) {
            case VIOLATION_DOUBLE_BORROW:
                printf("Double borrow (mutable + any)\n");
                break;
            case VIOLATION_USE_AFTER_MOVE:
                printf("Use after move\n");
                break;
            case VIOLATION_LIFETIME_OUTLIVE:
                printf("Lifetime violation (reference outlives owner)\n");
                break;
            case VIOLATION_DANGLING_REF:
                printf("Dangling reference\n");
                break;
            case VIOLATION_IMMUT_THROUGH_MUT:
                printf("Immutable borrow conflicts with mutable borrow\n");
                break;
        }
        printf("\n");
    }
}
