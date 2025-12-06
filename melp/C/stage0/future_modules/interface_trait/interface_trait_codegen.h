#ifndef INTERFACE_TRAIT_CODEGEN_H
#define INTERFACE_TRAIT_CODEGEN_H

#include "interface_trait.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_trait_def(FILE* out, TraitDef* def);
void codegen_trait_method(FILE* out, TraitMethod* method);
void codegen_trait_impl(FILE* out, TraitImpl* impl);
void codegen_trait_object(FILE* out, TraitObject* obj);
void codegen_vtable(FILE* out, VTable* vtable);

// Dynamic dispatch
void codegen_dynamic_dispatch(FILE* out, TraitObject* obj, const char* method_name);
void codegen_vtable_lookup(FILE* out, const char* method_name);

// Static dispatch (monomorphization)
void codegen_static_dispatch(FILE* out, TraitImpl* impl, const char* method_name);

// Trait bounds
void codegen_trait_bound_check(FILE* out, TraitBound* bound);

// Associated types
void codegen_associated_type(FILE* out, void* assoc_type);

// Super traits
void codegen_super_trait_check(FILE* out, TraitDef* def);

#endif // INTERFACE_TRAIT_CODEGEN_H
