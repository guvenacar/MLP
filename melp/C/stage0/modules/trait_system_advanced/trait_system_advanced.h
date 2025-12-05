#ifndef TRAIT_SYSTEM_ADVANCED_H
#define TRAIT_SYSTEM_ADVANCED_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// MLP ADVANCED TRAIT SYSTEM - MODULE #60
// ============================================================================
// Rust-style advanced trait features:
// - Trait bounds: where T: Display + Clone
// - Associated types: type Item;
// - Default implementations: default fn method() { ... }
// - Trait inheritance: trait Manager: Employee { }
// - Generic constraints: fn<T: Ord>(x: T)

// ============================================================================
// TRAIT BOUND TYPES
// ============================================================================

typedef enum {
    BOUND_SINGLE,        // T: Display
    BOUND_MULTIPLE,      // T: Display + Clone
    BOUND_LIFETIME,      // T: 'static
    BOUND_HIGHER_RANKED  // for<'a> T: Fn(&'a i32)
} TraitBoundType;

typedef enum {
    CONSTRAINT_WHERE,    // where T: Display
    CONSTRAINT_INLINE,   // fn<T: Display>
    CONSTRAINT_IMPL      // impl<T: Display>
} ConstraintContext;

// ============================================================================
// CORE STRUCTURES
// ============================================================================

typedef struct TraitBound {
    char* type_param;            // "T", "U", etc.
    char** trait_names;          // ["Display", "Clone"]
    size_t trait_count;          // Number of traits
    TraitBoundType bound_type;   // Type of bound
    
    char* lifetime;              // For lifetime bounds
    bool is_optional;            // T: ?Sized
    
    struct TraitBound* next;     // Linked list
} TraitBound;

typedef struct AssociatedType {
    char* name;                  // "Item", "Output", etc.
    char* concrete_type;         // Concrete type if known
    TraitBound* bounds;          // Bounds on associated type
    bool has_default;            // Has default type?
    char* default_type;          // Default type if any
} AssociatedType;

typedef struct TraitImpl {
    char* trait_name;            // "Display"
    char* for_type;              // "Point"
    TraitBound* bounds;          // Bounds on impl
    AssociatedType** assoc_types; // Associated type implementations
    size_t assoc_type_count;
    
    char** methods;              // Method implementations
    size_t method_count;
} TraitImpl;

typedef struct TraitDef {
    char* name;                  // Trait name
    char** type_params;          // Generic parameters
    size_t type_param_count;
    
    TraitBound* bounds;          // Trait bounds
    char** super_traits;         // Parent traits (inheritance)
    size_t super_trait_count;
    
    AssociatedType** assoc_types; // Associated types
    size_t assoc_type_count;
    
    char** methods;              // Method signatures
    size_t method_count;
    char** default_impls;        // Default implementations
    size_t default_impl_count;
} TraitDef;

typedef struct GenericConstraint {
    char* function_name;         // Function/method name
    TraitBound* bounds;          // Where clause bounds
    ConstraintContext context;   // Where constraint appears
    
    bool has_where_clause;       // Has explicit where clause
    size_t bound_count;          // Number of bounds
} GenericConstraint;

// ============================================================================
// TRAIT CONTEXT
// ============================================================================

typedef struct TraitContext {
    TraitDef** traits;           // All trait definitions
    size_t trait_count;
    size_t trait_capacity;
    
    TraitImpl** impls;           // All trait implementations
    size_t impl_count;
    size_t impl_capacity;
    
    GenericConstraint** constraints; // Generic constraints
    size_t constraint_count;
    size_t constraint_capacity;
    
    // Statistics
    size_t total_bounds;
    size_t total_assoc_types;
    size_t total_super_traits;
    size_t total_default_impls;
} TraitContext;

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

TraitContext* trait_context_create(void);
void trait_context_destroy(TraitContext* ctx);
void trait_context_reset(TraitContext* ctx);

// ============================================================================
// TRAIT DEFINITION
// ============================================================================

TraitDef* trait_def_create(const char* name);
void trait_def_add_type_param(TraitDef* trait, const char* param);
void trait_def_add_super_trait(TraitDef* trait, const char* super_name);
void trait_def_add_assoc_type(TraitDef* trait, AssociatedType* assoc);
void trait_def_add_method(TraitDef* trait, const char* method);
void trait_def_add_default_impl(TraitDef* trait, const char* impl);
void trait_def_free(TraitDef* trait);

// ============================================================================
// TRAIT BOUNDS
// ============================================================================

TraitBound* trait_bound_create(const char* type_param);
void trait_bound_add_trait(TraitBound* bound, const char* trait_name);
void trait_bound_set_lifetime(TraitBound* bound, const char* lifetime);
void trait_bound_set_optional(TraitBound* bound, bool optional);
void trait_bound_free(TraitBound* bound);

// Check if type satisfies bound
bool trait_bound_check(TraitContext* ctx, TraitBound* bound, const char* type_name);

// Check if all bounds in list are satisfied
bool trait_bounds_check_all(TraitContext* ctx, TraitBound* bounds, const char* type_name);

// ============================================================================
// ASSOCIATED TYPES
// ============================================================================

AssociatedType* assoc_type_create(const char* name);
void assoc_type_set_concrete(AssociatedType* assoc, const char* type_name);
void assoc_type_add_bound(AssociatedType* assoc, TraitBound* bound);
void assoc_type_set_default(AssociatedType* assoc, const char* default_type);
void assoc_type_free(AssociatedType* assoc);

// Resolve associated type
char* assoc_type_resolve(TraitContext* ctx, const char* trait_name, 
                         const char* assoc_name, const char* impl_type);

// ============================================================================
// TRAIT IMPLEMENTATION
// ============================================================================

TraitImpl* trait_impl_create(const char* trait_name, const char* for_type);
void trait_impl_add_bound(TraitImpl* impl, TraitBound* bound);
void trait_impl_add_assoc_type(TraitImpl* impl, AssociatedType* assoc);
void trait_impl_add_method(TraitImpl* impl, const char* method);
void trait_impl_free(TraitImpl* impl);

// Check if type implements trait
bool trait_is_implemented(TraitContext* ctx, const char* trait_name, const char* type_name);

// ============================================================================
// GENERIC CONSTRAINTS
// ============================================================================

GenericConstraint* constraint_create(const char* func_name, ConstraintContext context);
void constraint_add_bound(GenericConstraint* constraint, TraitBound* bound);
void constraint_free(GenericConstraint* constraint);

// Validate generic constraints
bool constraint_validate(TraitContext* ctx, GenericConstraint* constraint, 
                        const char** type_args, size_t arg_count);

// ============================================================================
// TRAIT INHERITANCE
// ============================================================================

// Check if trait inherits from another
bool trait_inherits_from(TraitContext* ctx, const char* child_trait, const char* parent_trait);

// Get all parent traits (transitive)
char** trait_get_all_parents(TraitContext* ctx, const char* trait_name, size_t* count);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

// Register trait in context
void trait_register(TraitContext* ctx, TraitDef* trait);

// Register impl in context
void impl_register(TraitContext* ctx, TraitImpl* impl);

// Find trait definition
TraitDef* trait_find(TraitContext* ctx, const char* name);

// Find trait implementation
TraitImpl* impl_find(TraitContext* ctx, const char* trait_name, const char* type_name);

// Mangle trait name
char* trait_mangle_name(const char* trait_name, const char* type_name);

// Print trait bounds (for debugging)
void trait_bounds_print(TraitBound* bounds);

// Get trait statistics
void trait_stats(TraitContext* ctx, size_t* traits, size_t* impls, 
                size_t* bounds, size_t* assoc_types);

// ============================================================================
// PARSER STRUCTURES
// ============================================================================

typedef enum {
    TRAIT_DECL,      // trait Display { }
    TRAIT_IMPL,      // impl Display for Point { }
    TRAIT_BOUND,     // where T: Display
    ASSOC_TYPE_DECL  // type Item = i32;
} TraitStmtType;

typedef struct TraitStmt {
    TraitStmtType stmt_type;
    union {
        TraitDef* trait_def;
        TraitImpl* trait_impl;
        TraitBound* trait_bound;
        AssociatedType* assoc_type;
    } data;
} TraitStmt;

// Parser functions
TraitStmt* trait_parse_statement(const char* line);
TraitDef* trait_parse_definition(const char** lines, size_t line_count);
TraitImpl* trait_parse_implementation(const char** lines, size_t line_count);
TraitBound* trait_parse_bound(const char* bound_str);
AssociatedType* trait_parse_assoc_type(const char* line);
void trait_stmt_free(TraitStmt* stmt);

// Helper parsers
char** parse_trait_list(const char* str, size_t* count);  // "Display + Clone"
bool is_trait_keyword(const char* str);

// ============================================================================
// CODEGEN FUNCTIONS
// ============================================================================

char* codegen_trait_definition(TraitDef* trait);
char* codegen_trait_impl(TraitImpl* impl);
char* codegen_trait_bound_check(TraitBound* bound, const char* type_name);
char* codegen_assoc_type_access(const char* trait_name, const char* assoc_name);
char* codegen_constraint_check(GenericConstraint* constraint);

#endif // TRAIT_SYSTEM_ADVANCED_H
