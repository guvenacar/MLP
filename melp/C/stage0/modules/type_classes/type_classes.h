#ifndef TYPE_CLASSES_H
#define TYPE_CLASSES_H

#include <stddef.h>
#include <stdbool.h>

// ============================================================================
// MLP TYPE CLASS SYSTEM - MODULE #64
// ============================================================================
// Type classes: Haskell-style ad-hoc polymorphism
// Higher-kinded types (HKT): types that take type constructors
// Examples: Functor, Monad, Applicative
// Based on Haskell's type class system

typedef enum {
    KIND_TYPE,              // *
    KIND_TYPE_TO_TYPE,      // * -> *
    KIND_TYPE_TO_TYPE_TO_TYPE  // * -> * -> *
} TypeKind;

typedef struct TypeClass {
    char* name;
    TypeKind kind;
    char** methods;
    size_t method_count;
    char** laws;            // Type class laws
    size_t law_count;
    char** super_classes;   // Superclass constraints
    size_t super_count;
} TypeClass;

typedef struct TypeInstance {
    char* class_name;
    char* type_name;
    char** method_impls;
    size_t impl_count;
    bool valid;
} TypeInstance;

typedef struct TypeClassContext {
    TypeClass** classes;
    size_t class_count;
    size_t class_capacity;
    TypeInstance** instances;
    size_t instance_count;
    size_t instance_capacity;
    size_t total_checks;
    size_t violations;
} TypeClassContext;

// Context management
TypeClassContext* tc_context_create(void);
void tc_context_destroy(TypeClassContext* ctx);

// Type class creation
TypeClass* tc_class_create(const char* name, TypeKind kind);
void tc_class_add_method(TypeClass* tc, const char* method);
void tc_class_add_law(TypeClass* tc, const char* law);
void tc_class_add_super(TypeClass* tc, const char* super);
void tc_class_free(TypeClass* tc);

// Instance creation
TypeInstance* tc_instance_create(const char* class_name, const char* type_name);
void tc_instance_add_impl(TypeInstance* inst, const char* impl);
void tc_instance_free(TypeInstance* inst);

// Registration
void tc_register_class(TypeClassContext* ctx, TypeClass* tc);
void tc_register_instance(TypeClassContext* ctx, TypeInstance* inst);

// Lookup
TypeClass* tc_find_class(TypeClassContext* ctx, const char* name);
TypeInstance* tc_find_instance(TypeClassContext* ctx, const char* class_name, const char* type_name);

// Checking
bool tc_has_instance(TypeClassContext* ctx, const char* class_name, const char* type_name);
bool tc_check_laws(TypeClassContext* ctx, TypeInstance* inst);

// Parser
TypeClass* tc_parse_class(const char* line);
TypeInstance* tc_parse_instance(const char* line);

// Codegen
char* codegen_typeclass_check(TypeClass* tc);

#endif
