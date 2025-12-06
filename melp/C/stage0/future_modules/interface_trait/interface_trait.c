#include "interface_trait.h"
#include <stdlib.h>
#include <string.h>

// Trait method oluştur
TraitMethod* trait_method_create(const char* name) {
    TraitMethod* method = malloc(sizeof(TraitMethod));
    method->name = strdup(name);
    method->signature = NULL;
    method->default_impl = NULL;
    method->has_default = 0;
    return method;
}

// Trait definition oluştur
TraitDef* trait_def_create(const char* name) {
    TraitDef* def = malloc(sizeof(TraitDef));
    def->name = strdup(name);
    def->method_count = 0;
    def->methods = malloc(sizeof(TraitMethod*) * 16);
    def->super_count = 0;
    def->super_traits = malloc(sizeof(TraitDef*) * 4);
    def->assoc_type_count = 0;
    def->associated_types = malloc(sizeof(void*) * 4);
    return def;
}

// Trait implementation oluştur
TraitImpl* trait_impl_create(TraitDef* trait_def, const char* type_name) {
    TraitImpl* impl = malloc(sizeof(TraitImpl));
    impl->trait_def = trait_def;
    impl->type_name = strdup(type_name);
    impl->impl_count = 0;
    impl->method_impls = malloc(sizeof(void*) * 16);
    impl->assoc_impl_count = 0;
    impl->assoc_type_impls = malloc(sizeof(void*) * 4);
    return impl;
}

// Trait bound oluştur
TraitBound* trait_bound_create(const char* type_param, TraitDef* trait) {
    TraitBound* bound = malloc(sizeof(TraitBound));
    bound->type_param = strdup(type_param);
    bound->required_trait = trait;
    bound->is_optional = 0;
    return bound;
}

// Trait object oluştur
TraitObject* trait_object_create(TraitDef* trait_def, void* data) {
    TraitObject* obj = malloc(sizeof(TraitObject));
    obj->trait_def = trait_def;
    obj->data = data;
    obj->vtable = NULL;
    return obj;
}

// VTable oluştur
VTable* vtable_create(TraitDef* trait_def) {
    VTable* vtable = malloc(sizeof(VTable));
    vtable->trait_def = trait_def;
    vtable->method_count = 0;
    vtable->methods = malloc(sizeof(void*) * 16);
    return vtable;
}

// Trait method signature ayarla
void trait_method_set_signature(TraitMethod* method, void* signature) {
    if (!method) return;
    method->signature = signature;
}

// Trait method default implementation ayarla
void trait_method_set_default_impl(TraitMethod* method, void* impl) {
    if (!method) return;
    method->default_impl = impl;
    method->has_default = 1;
}

// Trait definition'a method ekle
void trait_def_add_method(TraitDef* def, TraitMethod* method) {
    if (!def || !method) return;
    def->methods[def->method_count] = method;
    def->method_count++;
}

// Trait definition'a super trait ekle
void trait_def_add_super_trait(TraitDef* def, TraitDef* super_trait) {
    if (!def || !super_trait) return;
    def->super_traits[def->super_count] = super_trait;
    def->super_count++;
}

// Trait definition'a associated type ekle
void trait_def_add_associated_type(TraitDef* def, void* assoc_type) {
    if (!def || !assoc_type) return;
    def->associated_types[def->assoc_type_count] = assoc_type;
    def->assoc_type_count++;
}

// Trait definition'da method bul
TraitMethod* trait_def_find_method(TraitDef* def, const char* name) {
    if (!def || !name) return NULL;
    
    for (int i = 0; i < def->method_count; i++) {
        if (strcmp(def->methods[i]->name, name) == 0) {
            return def->methods[i];
        }
    }
    
    // Super trait'lerde ara
    for (int i = 0; i < def->super_count; i++) {
        TraitMethod* method = trait_def_find_method(def->super_traits[i], name);
        if (method) return method;
    }
    
    return NULL;
}

// Trait implementation'a method ekle
void trait_impl_add_method(TraitImpl* impl, void* method_impl) {
    if (!impl || !method_impl) return;
    impl->method_impls[impl->impl_count] = method_impl;
    impl->impl_count++;
}

// Trait implementation'a associated type ekle
void trait_impl_add_associated_type(TraitImpl* impl, void* assoc_type_impl) {
    if (!impl || !assoc_type_impl) return;
    impl->assoc_type_impls[impl->assoc_impl_count] = assoc_type_impl;
    impl->assoc_impl_count++;
}

// Trait implementation tamamlanmış mı?
int trait_impl_is_complete(TraitImpl* impl) {
    if (!impl) return 0;
    
    // Tüm required method'lar implement edilmiş mi?
    int required_count = 0;
    
    for (int i = 0; i < impl->trait_def->method_count; i++) {
        TraitMethod* method = impl->trait_def->methods[i];
        if (!method->has_default) {
            required_count++;
        }
    }
    
    return impl->impl_count >= required_count;
}

// Trait bound kontrolü
int trait_bound_check(TraitBound* bound, void* type) {
    if (!bound) return 1;
    
    // Type trait'i implement ediyor mu?
    // Stage 0: Her zaman true
    return 1;
}

// Trait object method çağır (dynamic dispatch)
void* trait_object_call_method(TraitObject* obj, const char* method_name, void** args) {
    if (!obj || !method_name) return NULL;
    
    // VTable'dan method bul
    void* method_ptr = vtable_lookup_method((VTable*)obj->vtable, method_name);
    
    if (!method_ptr) return NULL;
    
    // Method call (şimdilik NULL dön)
    // Gerçek implementasyonda: function pointer call
    
    return NULL;
}

// Trait object vtable ayarla
void trait_object_set_vtable(TraitObject* obj, VTable* vtable) {
    if (!obj) return;
    obj->vtable = vtable;
}

// VTable'a method ekle
void vtable_add_method(VTable* vtable, void* method_ptr) {
    if (!vtable || !method_ptr) return;
    vtable->methods[vtable->method_count] = method_ptr;
    vtable->method_count++;
}

// VTable'da method lookup
void* vtable_lookup_method(VTable* vtable, const char* method_name) {
    if (!vtable || !method_name) return NULL;
    
    // Method name ile lookup (şimdilik index-based)
    TraitMethod* method = trait_def_find_method(vtable->trait_def, method_name);
    
    if (!method) return NULL;
    
    // Method index bul
    for (int i = 0; i < vtable->trait_def->method_count; i++) {
        if (vtable->trait_def->methods[i] == method) {
            if (i < vtable->method_count) {
                return vtable->methods[i];
            }
        }
    }
    
    return NULL;
}

// Trait implemented mı?
int trait_is_implemented(TraitDef* trait, const char* type_name) {
    // Stage 0: Basit check
    return trait && type_name ? 1 : 0;
}

// Type trait implement ediyor mu?
int type_implements_trait(const char* type_name, TraitDef* trait) {
    // Stage 0: Basit check
    return type_name && trait ? 1 : 0;
}

// Bellek temizleme
void trait_method_free(TraitMethod* method) {
    if (!method) return;
    free(method->name);
    free(method);
}

void trait_def_free(TraitDef* def) {
    if (!def) return;
    free(def->name);
    
    for (int i = 0; i < def->method_count; i++) {
        trait_method_free(def->methods[i]);
    }
    
    free(def->methods);
    free(def->super_traits);
    free(def->associated_types);
    free(def);
}

void trait_impl_free(TraitImpl* impl) {
    if (!impl) return;
    free(impl->type_name);
    free(impl->method_impls);
    free(impl->assoc_type_impls);
    free(impl);
}

void trait_bound_free(TraitBound* bound) {
    if (!bound) return;
    free(bound->type_param);
    free(bound);
}

void trait_object_free(TraitObject* obj) {
    if (!obj) return;
    free(obj);
}

void vtable_free(VTable* vtable) {
    if (!vtable) return;
    free(vtable->methods);
    free(vtable);
}
