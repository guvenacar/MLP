#ifndef GENERIC_TYPES_H
#define GENERIC_TYPES_H

#include <stddef.h>
#include "../../lexer.h"

// Generic type parameter
typedef struct TypeParameter {
    char* name;                 // T, U, V, etc.
    void* constraint;           // Constraint (trait/interface)
    int has_constraint;         // Constraint var mı?
    void* default_type;         // Default type
} TypeParameter;

// Generic type definition
typedef struct GenericType {
    char* name;                     // GenericList<T>, HashMap<K,V>
    TypeParameter** type_params;    // Type parameters
    int param_count;                // Parameter sayısı
    void* body;                     // Type body
    int is_specialized;             // Specialized instance mı?
} GenericType;

// Type instantiation (örneğin List<int>)
typedef struct TypeInstantiation {
    GenericType* generic_type;      // Generic type definition
    void** type_args;               // Type arguments (int, string, etc.)
    int arg_count;                  // Argument sayısı
    char* mangled_name;             // Mangled name (List_int)
    void* specialized_body;         // Specialized implementation
} TypeInstantiation;

// Generic function
typedef struct GenericFunction {
    char* name;                     // map<T>, filter<T>, etc.
    TypeParameter** type_params;    // Type parameters
    int param_count;                // Parameter sayısı
    void* body;                     // Function body
    void** instantiations;          // Instantiation cache
    int inst_count;                 // Instantiation sayısı
} GenericFunction;

// Type constraint (trait bound)
typedef struct TypeConstraint {
    char* param_name;               // T
    char* trait_name;               // Comparable, Printable
    void* trait_def;                // Trait definition
} TypeConstraint;

// Type substitution (T -> int)
typedef struct TypeSubstitution {
    char* from;                     // T
    void* to;                       // int
} TypeSubstitution;

// Monomorphization context (compile-time specialization)
typedef struct MonomorphContext {
    TypeSubstitution** substitutions;   // Type substitutions
    int subst_count;                    // Substitution sayısı
    void* current_instance;             // Current instantiation
} MonomorphContext;

// API fonksiyonları
TypeParameter* type_parameter_create(const char* name);
GenericType* generic_type_create(const char* name);
TypeInstantiation* type_instantiation_create(GenericType* generic_type);
GenericFunction* generic_function_create(const char* name);
TypeConstraint* type_constraint_create(const char* param_name, const char* trait_name);
TypeSubstitution* type_substitution_create(const char* from, void* to);
MonomorphContext* monomorph_context_create(void);

void type_parameter_free(TypeParameter* param);
void generic_type_free(GenericType* type);
void type_instantiation_free(TypeInstantiation* inst);
void generic_function_free(GenericFunction* func);
void type_constraint_free(TypeConstraint* constraint);
void type_substitution_free(TypeSubstitution* subst);
void monomorph_context_free(MonomorphContext* ctx);

// Type parameter operasyonları
void type_parameter_set_constraint(TypeParameter* param, void* constraint);
void type_parameter_set_default(TypeParameter* param, void* default_type);

// Generic type operasyonları
void generic_type_add_param(GenericType* type, TypeParameter* param);
TypeInstantiation* generic_type_instantiate(GenericType* type, void** type_args, int arg_count);

// Type instantiation operasyonları
void type_instantiation_specialize(TypeInstantiation* inst);
char* type_instantiation_mangle_name(TypeInstantiation* inst);

// Generic function operasyonları
void generic_function_add_param(GenericFunction* func, TypeParameter* param);
void* generic_function_instantiate(GenericFunction* func, void** type_args, int arg_count);

// Type constraint operasyonları
int type_constraint_check(TypeConstraint* constraint, void* type);

// Monomorphization
void monomorph_add_substitution(MonomorphContext* ctx, TypeSubstitution* subst);
void* monomorph_apply_substitution(MonomorphContext* ctx, void* type);
void* monomorph_specialize(MonomorphContext* ctx, void* generic_item);

// Type utilities
int types_equal(void* type1, void* type2);
char* generic_type_to_string(void* type);

#endif // GENERIC_TYPES_H
