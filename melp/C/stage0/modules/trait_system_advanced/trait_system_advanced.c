#include "trait_system_advanced.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Forward declaration for strdup
extern char* strdup(const char*);

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

TraitContext* trait_context_create(void) {
    TraitContext* ctx = (TraitContext*)malloc(sizeof(TraitContext));
    if (!ctx) return NULL;
    
    ctx->traits = (TraitDef**)malloc(sizeof(TraitDef*) * 16);
    ctx->trait_count = 0;
    ctx->trait_capacity = 16;
    
    ctx->impls = (TraitImpl**)malloc(sizeof(TraitImpl*) * 32);
    ctx->impl_count = 0;
    ctx->impl_capacity = 32;
    
    ctx->constraints = (GenericConstraint**)malloc(sizeof(GenericConstraint*) * 16);
    ctx->constraint_count = 0;
    ctx->constraint_capacity = 16;
    
    ctx->total_bounds = 0;
    ctx->total_assoc_types = 0;
    ctx->total_super_traits = 0;
    ctx->total_default_impls = 0;
    
    return ctx;
}

void trait_context_destroy(TraitContext* ctx) {
    if (!ctx) return;
    
    // Free traits
    for (size_t i = 0; i < ctx->trait_count; i++) {
        trait_def_free(ctx->traits[i]);
    }
    free(ctx->traits);
    
    // Free impls
    for (size_t i = 0; i < ctx->impl_count; i++) {
        trait_impl_free(ctx->impls[i]);
    }
    free(ctx->impls);
    
    // Free constraints
    for (size_t i = 0; i < ctx->constraint_count; i++) {
        constraint_free(ctx->constraints[i]);
    }
    free(ctx->constraints);
    
    free(ctx);
}

void trait_context_reset(TraitContext* ctx) {
    if (!ctx) return;
    TraitContext* new_ctx = trait_context_create();
    trait_context_destroy(ctx);
    *ctx = *new_ctx;
    free(new_ctx);
}

// ============================================================================
// TRAIT DEFINITION
// ============================================================================

TraitDef* trait_def_create(const char* name) {
    TraitDef* trait = (TraitDef*)malloc(sizeof(TraitDef));
    if (!trait) return NULL;
    
    trait->name = strdup(name);
    trait->type_params = (char**)malloc(sizeof(char*) * 4);
    trait->type_param_count = 0;
    trait->bounds = NULL;
    trait->super_traits = (char**)malloc(sizeof(char*) * 4);
    trait->super_trait_count = 0;
    trait->assoc_types = (AssociatedType**)malloc(sizeof(AssociatedType*) * 4);
    trait->assoc_type_count = 0;
    trait->methods = (char**)malloc(sizeof(char*) * 8);
    trait->method_count = 0;
    trait->default_impls = (char**)malloc(sizeof(char*) * 8);
    trait->default_impl_count = 0;
    
    return trait;
}

void trait_def_add_type_param(TraitDef* trait, const char* param) {
    if (!trait) return;
    trait->type_params[trait->type_param_count++] = strdup(param);
}

void trait_def_add_super_trait(TraitDef* trait, const char* super_name) {
    if (!trait) return;
    trait->super_traits[trait->super_trait_count++] = strdup(super_name);
}

void trait_def_add_assoc_type(TraitDef* trait, AssociatedType* assoc) {
    if (!trait || !assoc) return;
    trait->assoc_types[trait->assoc_type_count++] = assoc;
}

void trait_def_add_method(TraitDef* trait, const char* method) {
    if (!trait) return;
    trait->methods[trait->method_count++] = strdup(method);
}

void trait_def_add_default_impl(TraitDef* trait, const char* impl) {
    if (!trait) return;
    trait->default_impls[trait->default_impl_count++] = strdup(impl);
}

void trait_def_free(TraitDef* trait) {
    if (!trait) return;
    
    free(trait->name);
    
    for (size_t i = 0; i < trait->type_param_count; i++) {
        free(trait->type_params[i]);
    }
    free(trait->type_params);
    
    trait_bound_free(trait->bounds);
    
    for (size_t i = 0; i < trait->super_trait_count; i++) {
        free(trait->super_traits[i]);
    }
    free(trait->super_traits);
    
    for (size_t i = 0; i < trait->assoc_type_count; i++) {
        assoc_type_free(trait->assoc_types[i]);
    }
    free(trait->assoc_types);
    
    for (size_t i = 0; i < trait->method_count; i++) {
        free(trait->methods[i]);
    }
    free(trait->methods);
    
    for (size_t i = 0; i < trait->default_impl_count; i++) {
        free(trait->default_impls[i]);
    }
    free(trait->default_impls);
    
    free(trait);
}

// ============================================================================
// TRAIT BOUNDS
// ============================================================================

TraitBound* trait_bound_create(const char* type_param) {
    TraitBound* bound = (TraitBound*)malloc(sizeof(TraitBound));
    if (!bound) return NULL;
    
    bound->type_param = strdup(type_param);
    bound->trait_names = (char**)malloc(sizeof(char*) * 4);
    bound->trait_count = 0;
    bound->bound_type = BOUND_SINGLE;
    bound->lifetime = NULL;
    bound->is_optional = false;
    bound->next = NULL;
    
    return bound;
}

void trait_bound_add_trait(TraitBound* bound, const char* trait_name) {
    if (!bound) return;
    bound->trait_names[bound->trait_count++] = strdup(trait_name);
    if (bound->trait_count > 1) {
        bound->bound_type = BOUND_MULTIPLE;
    }
}

void trait_bound_set_lifetime(TraitBound* bound, const char* lifetime) {
    if (!bound) return;
    bound->lifetime = strdup(lifetime);
    bound->bound_type = BOUND_LIFETIME;
}

void trait_bound_set_optional(TraitBound* bound, bool optional) {
    if (!bound) return;
    bound->is_optional = optional;
}

void trait_bound_free(TraitBound* bound) {
    while (bound) {
        TraitBound* next = bound->next;
        
        free(bound->type_param);
        
        for (size_t i = 0; i < bound->trait_count; i++) {
            free(bound->trait_names[i]);
        }
        free(bound->trait_names);
        
        if (bound->lifetime) free(bound->lifetime);
        
        free(bound);
        bound = next;
    }
}

bool trait_bound_check(TraitContext* ctx, TraitBound* bound, const char* type_name) {
    if (!ctx || !bound || !type_name) return false;
    
    // Check each trait in the bound
    for (size_t i = 0; i < bound->trait_count; i++) {
        if (!trait_is_implemented(ctx, bound->trait_names[i], type_name)) {
            return false;
        }
    }
    
    return true;
}

bool trait_bounds_check_all(TraitContext* ctx, TraitBound* bounds, const char* type_name) {
    if (!ctx || !type_name) return true;  // No bounds = always satisfied
    
    TraitBound* current = bounds;
    while (current) {
        if (!trait_bound_check(ctx, current, type_name)) {
            return false;
        }
        current = current->next;
    }
    
    return true;
}

// ============================================================================
// ASSOCIATED TYPES
// ============================================================================

AssociatedType* assoc_type_create(const char* name) {
    AssociatedType* assoc = (AssociatedType*)malloc(sizeof(AssociatedType));
    if (!assoc) return NULL;
    
    assoc->name = strdup(name);
    assoc->concrete_type = NULL;
    assoc->bounds = NULL;
    assoc->has_default = false;
    assoc->default_type = NULL;
    
    return assoc;
}

void assoc_type_set_concrete(AssociatedType* assoc, const char* type_name) {
    if (!assoc) return;
    assoc->concrete_type = strdup(type_name);
}

void assoc_type_add_bound(AssociatedType* assoc, TraitBound* bound) {
    if (!assoc || !bound) return;
    
    if (!assoc->bounds) {
        assoc->bounds = bound;
    } else {
        TraitBound* current = assoc->bounds;
        while (current->next) {
            current = current->next;
        }
        current->next = bound;
    }
}

void assoc_type_set_default(AssociatedType* assoc, const char* default_type) {
    if (!assoc) return;
    assoc->has_default = true;
    assoc->default_type = strdup(default_type);
}

void assoc_type_free(AssociatedType* assoc) {
    if (!assoc) return;
    
    free(assoc->name);
    if (assoc->concrete_type) free(assoc->concrete_type);
    trait_bound_free(assoc->bounds);
    if (assoc->default_type) free(assoc->default_type);
    
    free(assoc);
}

char* assoc_type_resolve(TraitContext* ctx, const char* trait_name, 
                         const char* assoc_name, const char* impl_type) {
    if (!ctx || !trait_name || !assoc_name || !impl_type) return NULL;
    
    // Find the trait implementation
    TraitImpl* impl = impl_find(ctx, trait_name, impl_type);
    if (!impl) return NULL;
    
    // Find the associated type in the impl
    for (size_t i = 0; i < impl->assoc_type_count; i++) {
        if (strcmp(impl->assoc_types[i]->name, assoc_name) == 0) {
            if (impl->assoc_types[i]->concrete_type) {
                return strdup(impl->assoc_types[i]->concrete_type);
            }
        }
    }
    
    // Check trait definition for default
    TraitDef* trait = trait_find(ctx, trait_name);
    if (trait) {
        for (size_t i = 0; i < trait->assoc_type_count; i++) {
            if (strcmp(trait->assoc_types[i]->name, assoc_name) == 0) {
                if (trait->assoc_types[i]->has_default) {
                    return strdup(trait->assoc_types[i]->default_type);
                }
            }
        }
    }
    
    return NULL;
}

// ============================================================================
// TRAIT IMPLEMENTATION
// ============================================================================

TraitImpl* trait_impl_create(const char* trait_name, const char* for_type) {
    TraitImpl* impl = (TraitImpl*)malloc(sizeof(TraitImpl));
    if (!impl) return NULL;
    
    impl->trait_name = strdup(trait_name);
    impl->for_type = strdup(for_type);
    impl->bounds = NULL;
    impl->assoc_types = (AssociatedType**)malloc(sizeof(AssociatedType*) * 4);
    impl->assoc_type_count = 0;
    impl->methods = (char**)malloc(sizeof(char*) * 8);
    impl->method_count = 0;
    
    return impl;
}

void trait_impl_add_bound(TraitImpl* impl, TraitBound* bound) {
    if (!impl || !bound) return;
    
    if (!impl->bounds) {
        impl->bounds = bound;
    } else {
        TraitBound* current = impl->bounds;
        while (current->next) {
            current = current->next;
        }
        current->next = bound;
    }
}

void trait_impl_add_assoc_type(TraitImpl* impl, AssociatedType* assoc) {
    if (!impl || !assoc) return;
    impl->assoc_types[impl->assoc_type_count++] = assoc;
}

void trait_impl_add_method(TraitImpl* impl, const char* method) {
    if (!impl) return;
    impl->methods[impl->method_count++] = strdup(method);
}

void trait_impl_free(TraitImpl* impl) {
    if (!impl) return;
    
    free(impl->trait_name);
    free(impl->for_type);
    trait_bound_free(impl->bounds);
    
    for (size_t i = 0; i < impl->assoc_type_count; i++) {
        assoc_type_free(impl->assoc_types[i]);
    }
    free(impl->assoc_types);
    
    for (size_t i = 0; i < impl->method_count; i++) {
        free(impl->methods[i]);
    }
    free(impl->methods);
    
    free(impl);
}

bool trait_is_implemented(TraitContext* ctx, const char* trait_name, const char* type_name) {
    if (!ctx || !trait_name || !type_name) return false;
    
    // Search for implementation
    for (size_t i = 0; i < ctx->impl_count; i++) {
        TraitImpl* impl = ctx->impls[i];
        if (strcmp(impl->trait_name, trait_name) == 0 &&
            strcmp(impl->for_type, type_name) == 0) {
            return true;
        }
    }
    
    return false;
}

// ============================================================================
// GENERIC CONSTRAINTS
// ============================================================================

GenericConstraint* constraint_create(const char* func_name, ConstraintContext context) {
    GenericConstraint* constraint = (GenericConstraint*)malloc(sizeof(GenericConstraint));
    if (!constraint) return NULL;
    
    constraint->function_name = strdup(func_name);
    constraint->bounds = NULL;
    constraint->context = context;
    constraint->has_where_clause = false;
    constraint->bound_count = 0;
    
    return constraint;
}

void constraint_add_bound(GenericConstraint* constraint, TraitBound* bound) {
    if (!constraint || !bound) return;
    
    if (!constraint->bounds) {
        constraint->bounds = bound;
    } else {
        TraitBound* current = constraint->bounds;
        while (current->next) {
            current = current->next;
        }
        current->next = bound;
    }
    
    constraint->bound_count++;
    constraint->has_where_clause = true;
}

void constraint_free(GenericConstraint* constraint) {
    if (!constraint) return;
    
    free(constraint->function_name);
    trait_bound_free(constraint->bounds);
    
    free(constraint);
}

bool constraint_validate(TraitContext* ctx, GenericConstraint* constraint, 
                        const char** type_args, size_t arg_count) {
    if (!ctx || !constraint) return true;
    if (!type_args || arg_count == 0) return true;
    
    // Check each bound against corresponding type argument
    TraitBound* current = constraint->bounds;
    size_t idx = 0;
    
    while (current && idx < arg_count) {
        if (!trait_bound_check(ctx, current, type_args[idx])) {
            return false;
        }
        current = current->next;
        idx++;
    }
    
    return true;
}

// ============================================================================
// TRAIT INHERITANCE
// ============================================================================

bool trait_inherits_from(TraitContext* ctx, const char* child_trait, const char* parent_trait) {
    if (!ctx || !child_trait || !parent_trait) return false;
    
    TraitDef* child = trait_find(ctx, child_trait);
    if (!child) return false;
    
    // Direct parent check
    for (size_t i = 0; i < child->super_trait_count; i++) {
        if (strcmp(child->super_traits[i], parent_trait) == 0) {
            return true;
        }
        
        // Transitive check
        if (trait_inherits_from(ctx, child->super_traits[i], parent_trait)) {
            return true;
        }
    }
    
    return false;
}

char** trait_get_all_parents(TraitContext* ctx, const char* trait_name, size_t* count) {
    if (!ctx || !trait_name || !count) return NULL;
    
    *count = 0;
    char** parents = (char**)malloc(sizeof(char*) * 16);
    
    TraitDef* trait = trait_find(ctx, trait_name);
    if (!trait) return parents;
    
    // Collect direct parents
    for (size_t i = 0; i < trait->super_trait_count; i++) {
        parents[(*count)++] = strdup(trait->super_traits[i]);
        
        // Recursively collect transitive parents
        size_t sub_count;
        char** sub_parents = trait_get_all_parents(ctx, trait->super_traits[i], &sub_count);
        
        for (size_t j = 0; j < sub_count; j++) {
            parents[(*count)++] = sub_parents[j];
        }
        free(sub_parents);
    }
    
    return parents;
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void trait_register(TraitContext* ctx, TraitDef* trait) {
    if (!ctx || !trait) return;
    
    if (ctx->trait_count >= ctx->trait_capacity) {
        ctx->trait_capacity *= 2;
        ctx->traits = (TraitDef**)realloc(ctx->traits, 
                                          sizeof(TraitDef*) * ctx->trait_capacity);
    }
    
    ctx->traits[ctx->trait_count++] = trait;
    ctx->total_assoc_types += trait->assoc_type_count;
    ctx->total_super_traits += trait->super_trait_count;
    ctx->total_default_impls += trait->default_impl_count;
}

void impl_register(TraitContext* ctx, TraitImpl* impl) {
    if (!ctx || !impl) return;
    
    if (ctx->impl_count >= ctx->impl_capacity) {
        ctx->impl_capacity *= 2;
        ctx->impls = (TraitImpl**)realloc(ctx->impls, 
                                          sizeof(TraitImpl*) * ctx->impl_capacity);
    }
    
    ctx->impls[ctx->impl_count++] = impl;
}

TraitDef* trait_find(TraitContext* ctx, const char* name) {
    if (!ctx || !name) return NULL;
    
    for (size_t i = 0; i < ctx->trait_count; i++) {
        if (strcmp(ctx->traits[i]->name, name) == 0) {
            return ctx->traits[i];
        }
    }
    
    return NULL;
}

TraitImpl* impl_find(TraitContext* ctx, const char* trait_name, const char* type_name) {
    if (!ctx || !trait_name || !type_name) return NULL;
    
    for (size_t i = 0; i < ctx->impl_count; i++) {
        TraitImpl* impl = ctx->impls[i];
        if (strcmp(impl->trait_name, trait_name) == 0 &&
            strcmp(impl->for_type, type_name) == 0) {
            return impl;
        }
    }
    
    return NULL;
}

char* trait_mangle_name(const char* trait_name, const char* type_name) {
    size_t len = strlen(trait_name) + strlen(type_name) + 10;
    char* mangled = (char*)malloc(len);
    snprintf(mangled, len, "%s_for_%s", trait_name, type_name);
    return mangled;
}

void trait_bounds_print(TraitBound* bounds) {
    if (!bounds) {
        printf("(no bounds)\n");
        return;
    }
    
    TraitBound* current = bounds;
    while (current) {
        printf("%s: ", current->type_param);
        
        for (size_t i = 0; i < current->trait_count; i++) {
            printf("%s", current->trait_names[i]);
            if (i < current->trait_count - 1) {
                printf(" + ");
            }
        }
        
        if (current->lifetime) {
            printf(" + '%s", current->lifetime);
        }
        
        if (current->is_optional) {
            printf(" (optional)");
        }
        
        printf("\n");
        current = current->next;
    }
}

void trait_stats(TraitContext* ctx, size_t* traits, size_t* impls, 
                size_t* bounds, size_t* assoc_types) {
    if (!ctx) return;
    
    if (traits) *traits = ctx->trait_count;
    if (impls) *impls = ctx->impl_count;
    if (bounds) *bounds = ctx->total_bounds;
    if (assoc_types) *assoc_types = ctx->total_assoc_types;
}
