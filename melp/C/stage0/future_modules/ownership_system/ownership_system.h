#ifndef OWNERSHIP_SYSTEM_H
#define OWNERSHIP_SYSTEM_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// OWNERSHIP TYPES
// ============================================================================

// Ownership transfer types
typedef enum {
    OWNER_OWNED,      // Full ownership (default)
    OWNER_BORROWED,   // Borrowed reference (&T)
    OWNER_MUT_BORROW, // Mutable borrow (&mut T)
    OWNER_MOVED,      // Moved ownership
    OWNER_UNKNOWN
} OwnershipType;

// Lifetime scope
typedef enum {
    LIFETIME_STATIC,     // 'static - program lifetime
    LIFETIME_NAMED,      // 'a, 'b - named lifetime
    LIFETIME_ELIDED,     // Compiler-inferred lifetime
    LIFETIME_TEMPORARY,  // Temporary lifetime
    LIFETIME_UNKNOWN
} LifetimeType;

// Borrow state
typedef enum {
    BORROW_NONE,         // Not borrowed
    BORROW_SHARED,       // Multiple immutable borrows allowed
    BORROW_EXCLUSIVE,    // Single mutable borrow
    BORROW_CONFLICT      // Error state
} BorrowState;

// Move semantics
typedef enum {
    MOVE_COPY,           // Copy semantics (Copy trait)
    MOVE_MOVE,           // Move semantics (moved)
    MOVE_CLONE,          // Explicit clone
    MOVE_BORROW          // Borrowed (no move)
} MoveSemantics;

// ============================================================================
// DATA STRUCTURES
// ============================================================================

// Lifetime annotation
typedef struct {
    char name[64];           // 'a, 'b, 'static
    LifetimeType type;
    int scope_start;         // Line number
    int scope_end;
    bool is_active;
} Lifetime;

// Variable ownership info
typedef struct {
    char var_name[128];
    char type_name[128];
    OwnershipType ownership;
    MoveSemantics semantics;
    BorrowState borrow_state;
    
    Lifetime* lifetime;      // Associated lifetime
    int borrow_count;        // Number of active borrows
    bool is_moved;           // Has been moved
    int declaration_line;
    int last_use_line;
} OwnershipInfo;

// Borrow operation
typedef struct {
    char var_name[128];
    char borrowed_as[128];   // New reference name
    bool is_mutable;
    Lifetime* lifetime;
    int line;
} BorrowOp;

// Move operation
typedef struct {
    char from_var[128];
    char to_var[128];
    MoveSemantics semantics;
    int line;
} MoveOp;

// Ownership rule violation
typedef struct {
    char message[256];
    char var_name[128];
    int line;
    enum {
        VIOLATION_DOUBLE_BORROW,      // Multiple mutable borrows
        VIOLATION_USE_AFTER_MOVE,     // Use after move
        VIOLATION_LIFETIME_OUTLIVE,   // Reference outlives owner
        VIOLATION_DANGLING_REF,       // Dangling reference
        VIOLATION_IMMUT_THROUGH_MUT   // Immutable borrow with active mutable
    } type;
} OwnershipViolation;

// ============================================================================
// CONTEXT
// ============================================================================

typedef struct {
    // Ownership tracking
    OwnershipInfo owners[256];
    int owner_count;
    
    // Lifetime tracking
    Lifetime lifetimes[64];
    int lifetime_count;
    
    // Operations
    BorrowOp borrows[128];
    int borrow_count;
    
    MoveOp moves[128];
    int move_count;
    
    // Violations
    OwnershipViolation violations[64];
    int violation_count;
    
    // Current scope
    int current_scope_depth;
    int current_line;
} OwnershipCtx;

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

OwnershipCtx* ownership_ctx_create(void);
void ownership_ctx_free(OwnershipCtx* ctx);

// ============================================================================
// OWNERSHIP OPERATIONS
// ============================================================================

// Add new owned variable
void ownership_add_owner(OwnershipCtx* ctx, const char* var_name, 
                         const char* type_name, int line);

// Create borrow
bool ownership_create_borrow(OwnershipCtx* ctx, const char* var_name,
                             const char* borrowed_as, bool is_mutable, 
                             const char* lifetime_name, int line);

// Execute move
bool ownership_execute_move(OwnershipCtx* ctx, const char* from_var,
                            const char* to_var, MoveSemantics semantics, int line);

// Check if variable can be used
bool ownership_can_use(OwnershipCtx* ctx, const char* var_name, int line);

// End borrow scope
void ownership_end_borrow(OwnershipCtx* ctx, const char* borrowed_name, int line);

// ============================================================================
// LIFETIME MANAGEMENT
// ============================================================================

// Create lifetime annotation
Lifetime* ownership_create_lifetime(OwnershipCtx* ctx, const char* name, 
                                     LifetimeType type, int start_line);

// End lifetime scope
void ownership_end_lifetime(OwnershipCtx* ctx, const char* name, int end_line);

// Check lifetime validity
bool ownership_check_lifetime(OwnershipCtx* ctx, const char* ref_var, 
                              const char* owner_var, int line);

// ============================================================================
// BORROW CHECKER
// ============================================================================

// Check borrow rules (multiple readers OR single writer)
bool ownership_check_borrow_rules(OwnershipCtx* ctx, const char* var_name, 
                                  bool is_mutable, int line);

// Detect use-after-move
bool ownership_check_use_after_move(OwnershipCtx* ctx, const char* var_name, int line);

// Validate no dangling references
bool ownership_check_dangling(OwnershipCtx* ctx);

// ============================================================================
// PARSER FUNCTIONS
// ============================================================================

void ownership_parser_init(const char* source);
bool ownership_parse_file(OwnershipCtx* ctx);
bool ownership_parse_borrow(OwnershipCtx* ctx, const char* line);
bool ownership_parse_move(OwnershipCtx* ctx, const char* line);
bool ownership_parse_lifetime(OwnershipCtx* ctx, const char* line);

// ============================================================================
// CODEGEN FUNCTIONS
// ============================================================================

void ownership_generate_asm(const char* output_file, OwnershipCtx* ctx);
void ownership_codegen_borrow_check(FILE* out, BorrowOp* borrow);
void ownership_codegen_move(FILE* out, MoveOp* move);
void ownership_codegen_lifetime(FILE* out, Lifetime* lifetime);

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

OwnershipInfo* ownership_find_var(OwnershipCtx* ctx, const char* var_name);
void ownership_report_violations(OwnershipCtx* ctx);
const char* ownership_type_str(OwnershipType type);
const char* lifetime_type_str(LifetimeType type);

#endif // OWNERSHIP_SYSTEM_H
