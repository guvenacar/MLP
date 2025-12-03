#ifndef INTERFACE_TRAIT_H
#define INTERFACE_TRAIT_H

#include <stddef.h>
#include "../../lexer.h"

// Trait method (interface method)
typedef struct TraitMethod {
    char* name;                 // Method adı
    void* signature;            // Method signature (params, return)
    void* default_impl;         // Default implementation (optional)
    int has_default;            // Default impl var mı?
} TraitMethod;

// Forward declaration
typedef struct TraitDef TraitDef;

// Trait definition (interface)
struct TraitDef {
    char* name;                 // Trait adı
    TraitMethod** methods;      // Method listesi
    int method_count;           // Method sayısı
    TraitDef** super_traits;    // Parent traits (inheritance)
    int super_count;            // Parent sayısı
    void** associated_types;    // Associated types
    int assoc_type_count;       // Associated type sayısı
};

// Trait implementation (impl Trait for Type)
typedef struct TraitImpl {
    TraitDef* trait_def;        // Implement edilen trait
    char* type_name;            // Hangi type için
    void** method_impls;        // Method implementations
    int impl_count;             // Implementation sayısı
    void** assoc_type_impls;    // Associated type implementations
    int assoc_impl_count;       // Associated type impl sayısı
} TraitImpl;

// Trait bound (generic constraint)
typedef struct TraitBound {
    char* type_param;           // T
    TraitDef* required_trait;   // Required trait
    int is_optional;            // Optional bound mı?
} TraitBound;

// Trait object (dynamic dispatch)
typedef struct TraitObject {
    void* vtable;               // Virtual method table
    void* data;                 // Actual data pointer
    TraitDef* trait_def;        // Trait definition
} TraitObject;

// Virtual method table
typedef struct VTable {
    TraitDef* trait_def;        // Trait
    void** methods;             // Method pointers
    int method_count;           // Method sayısı
} VTable;

// API fonksiyonları
TraitMethod* trait_method_create(const char* name);
TraitDef* trait_def_create(const char* name);
TraitImpl* trait_impl_create(TraitDef* trait_def, const char* type_name);
TraitBound* trait_bound_create(const char* type_param, TraitDef* trait);
TraitObject* trait_object_create(TraitDef* trait_def, void* data);
VTable* vtable_create(TraitDef* trait_def);

void trait_method_free(TraitMethod* method);
void trait_def_free(TraitDef* def);
void trait_impl_free(TraitImpl* impl);
void trait_bound_free(TraitBound* bound);
void trait_object_free(TraitObject* obj);
void vtable_free(VTable* vtable);

// Trait method operasyonları
void trait_method_set_signature(TraitMethod* method, void* signature);
void trait_method_set_default_impl(TraitMethod* method, void* impl);

// Trait definition operasyonları
void trait_def_add_method(TraitDef* def, TraitMethod* method);
void trait_def_add_super_trait(TraitDef* def, TraitDef* super_trait);
void trait_def_add_associated_type(TraitDef* def, void* assoc_type);
TraitMethod* trait_def_find_method(TraitDef* def, const char* name);

// Trait implementation operasyonları
void trait_impl_add_method(TraitImpl* impl, void* method_impl);
void trait_impl_add_associated_type(TraitImpl* impl, void* assoc_type_impl);
int trait_impl_is_complete(TraitImpl* impl);

// Trait bound operasyonları
int trait_bound_check(TraitBound* bound, void* type);

// Trait object operasyonları (dynamic dispatch)
void* trait_object_call_method(TraitObject* obj, const char* method_name, void** args);
void trait_object_set_vtable(TraitObject* obj, VTable* vtable);

// VTable operasyonları
void vtable_add_method(VTable* vtable, void* method_ptr);
void* vtable_lookup_method(VTable* vtable, const char* method_name);

// Utility
int trait_is_implemented(TraitDef* trait, const char* type_name);
int type_implements_trait(const char* type_name, TraitDef* trait);

#endif // INTERFACE_TRAIT_H
