// Reflection Module - Implementation
// Runtime type information implementasyonu

#include "reflection.h"
#include <stdlib.h>
#include <string.h>

// Global type registry
static TypeRegistry* global_registry = NULL;

// Type info operations
TypeInfo* type_info_create(const char* name, TypeCategory category) {
    TypeInfo* type = (TypeInfo*)malloc(sizeof(TypeInfo));
    
    type->name = strdup(name);
    type->category = category;
    type->size = 0;
    type->alignment = 0;
    type->flags = 0;
    type->attributes = NULL;
    type->attribute_count = 0;
    
    // Category-specific initialization
    if (category == TYPE_STRUCT) {
        type->data.struct_info.fields = NULL;
        type->data.struct_info.field_count = 0;
        type->data.struct_info.methods = NULL;
        type->data.struct_info.method_count = 0;
    } else if (category == TYPE_ENUM) {
        type->data.enum_info.variants = NULL;
        type->data.enum_info.values = NULL;
        type->data.enum_info.variant_count = 0;
    }
    
    return type;
}

void type_info_free(TypeInfo* type_info) {
    if (type_info) {
        if (type_info->name) free(type_info->name);
        
        if (type_info->category == TYPE_STRUCT) {
            if (type_info->data.struct_info.fields) {
                for (size_t i = 0; i < type_info->data.struct_info.field_count; i++) {
                    field_info_free(type_info->data.struct_info.fields[i]);
                }
                free(type_info->data.struct_info.fields);
            }
            if (type_info->data.struct_info.methods) {
                for (size_t i = 0; i < type_info->data.struct_info.method_count; i++) {
                    method_info_free(type_info->data.struct_info.methods[i]);
                }
                free(type_info->data.struct_info.methods);
            }
        } else if (type_info->category == TYPE_ENUM) {
            if (type_info->data.enum_info.variants) {
                for (size_t i = 0; i < type_info->data.enum_info.variant_count; i++) {
                    free(type_info->data.enum_info.variants[i]);
                }
                free(type_info->data.enum_info.variants);
            }
            if (type_info->data.enum_info.values) {
                free(type_info->data.enum_info.values);
            }
        }
        
        if (type_info->attributes) free(type_info->attributes);
        free(type_info);
    }
}

TypeInfo* type_info_get_by_name(const char* name) {
    if (!global_registry) return NULL;
    return type_registry_lookup(global_registry, name);
}

TypeInfo* type_info_get_by_instance(void* instance) {
    // TODO Stage 1: Instance'dan TypeInfo çıkar
    (void)instance;
    return NULL;
}

int type_info_is_compatible(TypeInfo* from, TypeInfo* to) {
    if (!from || !to) return 0;
    
    // Aynı tip
    if (strcmp(from->name, to->name) == 0) return 1;
    
    // TODO Stage 1: Subtype checking, interface implementation vb.
    return 0;
}

int type_info_is_subtype(TypeInfo* derived, TypeInfo* base) {
    if (!derived || !base) return 0;
    
    // TODO Stage 1: Inheritance chain kontrolü
    (void)derived;
    (void)base;
    return 0;
}

// Field operations
FieldInfo* field_info_create(const char* name, TypeInfo* type, size_t offset) {
    FieldInfo* field = (FieldInfo*)malloc(sizeof(FieldInfo));
    
    field->name = strdup(name);
    field->type_info = type;
    field->offset = offset;
    field->size = type ? type->size : 0;
    field->flags = 0;
    field->attributes = NULL;
    field->attribute_count = 0;
    
    return field;
}

void field_info_free(FieldInfo* field) {
    if (field) {
        if (field->name) free(field->name);
        if (field->attributes) free(field->attributes);
        free(field);
    }
}

FieldInfo* type_info_get_field(TypeInfo* type, const char* name) {
    if (!type || type->category != TYPE_STRUCT) return NULL;
    
    for (size_t i = 0; i < type->data.struct_info.field_count; i++) {
        if (strcmp(type->data.struct_info.fields[i]->name, name) == 0) {
            return type->data.struct_info.fields[i];
        }
    }
    
    return NULL;
}

void* field_info_get_value(FieldInfo* field, void* instance) {
    if (!field || !instance) return NULL;
    
    // Instance + offset ile field'a eriş
    return (char*)instance + field->offset;
}

void field_info_set_value(FieldInfo* field, void* instance, void* value) {
    if (!field || !instance || !value) return;
    
    // Instance + offset ile field'a yaz
    void* field_ptr = (char*)instance + field->offset;
    memcpy(field_ptr, value, field->size);
}

size_t type_info_get_field_count(TypeInfo* type) {
    if (!type || type->category != TYPE_STRUCT) return 0;
    return type->data.struct_info.field_count;
}

FieldInfo** type_info_get_fields(TypeInfo* type) {
    if (!type || type->category != TYPE_STRUCT) return NULL;
    return type->data.struct_info.fields;
}

// Method operations
MethodInfo* method_info_create(const char* name, TypeInfo* return_type) {
    MethodInfo* method = (MethodInfo*)malloc(sizeof(MethodInfo));
    
    method->name = strdup(name);
    method->return_type = return_type;
    method->param_types = NULL;
    method->param_count = 0;
    method->function_ptr = NULL;
    method->flags = 0;
    method->attributes = NULL;
    method->attribute_count = 0;
    
    return method;
}

void method_info_free(MethodInfo* method) {
    if (method) {
        if (method->name) free(method->name);
        if (method->param_types) free(method->param_types);
        if (method->attributes) free(method->attributes);
        free(method);
    }
}

MethodInfo* type_info_get_method(TypeInfo* type, const char* name) {
    if (!type || type->category != TYPE_STRUCT) return NULL;
    
    for (size_t i = 0; i < type->data.struct_info.method_count; i++) {
        if (strcmp(type->data.struct_info.methods[i]->name, name) == 0) {
            return type->data.struct_info.methods[i];
        }
    }
    
    return NULL;
}

void* method_info_invoke(MethodInfo* method, void* instance, void** args) {
    if (!method || !method->function_ptr) return NULL;
    
    // TODO Stage 1: Dynamic function invocation
    (void)instance;
    (void)args;
    return NULL;
}

size_t type_info_get_method_count(TypeInfo* type) {
    if (!type || type->category != TYPE_STRUCT) return 0;
    return type->data.struct_info.method_count;
}

MethodInfo** type_info_get_methods(TypeInfo* type) {
    if (!type || type->category != TYPE_STRUCT) return NULL;
    return type->data.struct_info.methods;
}

// Attribute operations
AttributeInfo* attribute_info_create(const char* name) {
    AttributeInfo* attr = (AttributeInfo*)malloc(sizeof(AttributeInfo));
    
    attr->name = strdup(name);
    attr->values = NULL;
    attr->value_count = 0;
    
    return attr;
}

void attribute_info_free(AttributeInfo* attr) {
    if (attr) {
        if (attr->name) free(attr->name);
        if (attr->values) free(attr->values);
        free(attr);
    }
}

void attribute_info_add_value(AttributeInfo* attr, void* value) {
    if (!attr) return;
    
    attr->values = (void**)realloc(attr->values, sizeof(void*) * (attr->value_count + 1));
    attr->values[attr->value_count++] = value;
}

AttributeInfo* type_info_get_attribute(TypeInfo* type, const char* name) {
    if (!type) return NULL;
    
    AttributeInfo** attrs = (AttributeInfo**)type->attributes;
    for (size_t i = 0; i < type->attribute_count; i++) {
        if (strcmp(attrs[i]->name, name) == 0) {
            return attrs[i];
        }
    }
    
    return NULL;
}

int type_info_has_attribute(TypeInfo* type, const char* name) {
    return type_info_get_attribute(type, name) != NULL;
}

// Registry operations
TypeRegistry* type_registry_create(void) {
    TypeRegistry* registry = (TypeRegistry*)malloc(sizeof(TypeRegistry));
    
    registry->types = NULL;
    registry->type_count = 0;
    registry->type_capacity = 0;
    registry->type_map = NULL;
    
    if (!global_registry) {
        global_registry = registry;
    }
    
    return registry;
}

void type_registry_free(TypeRegistry* registry) {
    if (registry) {
        if (registry->types) {
            for (size_t i = 0; i < registry->type_count; i++) {
                type_info_free(registry->types[i]);
            }
            free(registry->types);
        }
        if (registry->type_map) free(registry->type_map);
        free(registry);
        
        if (global_registry == registry) {
            global_registry = NULL;
        }
    }
}

void type_registry_register(TypeRegistry* registry, TypeInfo* type_info) {
    if (!registry || !type_info) return;
    
    if (registry->type_count >= registry->type_capacity) {
        size_t new_capacity = registry->type_capacity == 0 ? 16 : registry->type_capacity * 2;
        registry->types = (TypeInfo**)realloc(registry->types, sizeof(TypeInfo*) * new_capacity);
        registry->type_capacity = new_capacity;
    }
    
    registry->types[registry->type_count++] = type_info;
    
    // TODO Stage 1: type_map'e de ekle
}

TypeInfo* type_registry_lookup(TypeRegistry* registry, const char* name) {
    if (!registry || !name) return NULL;
    
    for (size_t i = 0; i < registry->type_count; i++) {
        if (strcmp(registry->types[i]->name, name) == 0) {
            return registry->types[i];
        }
    }
    
    return NULL;
}

size_t type_registry_get_count(TypeRegistry* registry) {
    return registry ? registry->type_count : 0;
}

TypeInfo** type_registry_get_all(TypeRegistry* registry) {
    return registry ? registry->types : NULL;
}

// Instance operations
InstanceInfo* instance_info_create(void* instance, TypeInfo* type_info) {
    InstanceInfo* info = (InstanceInfo*)malloc(sizeof(InstanceInfo));
    
    info->instance = instance;
    info->type_info = type_info;
    info->ref_count = 1;
    
    return info;
}

void instance_info_free(InstanceInfo* info) {
    if (info) {
        free(info);
    }
}

TypeInfo* instance_get_type(void* instance) {
    // TODO Stage 1: Instance'dan TypeInfo çıkar
    (void)instance;
    return NULL;
}

void* instance_clone(void* instance) {
    if (!instance) return NULL;
    
    TypeInfo* type = instance_get_type(instance);
    if (!type) return NULL;
    
    void* clone = malloc(type->size);
    memcpy(clone, instance, type->size);
    
    return clone;
}

int instance_equals(void* a, void* b) {
    if (a == b) return 1;
    if (!a || !b) return 0;
    
    TypeInfo* type_a = instance_get_type(a);
    TypeInfo* type_b = instance_get_type(b);
    
    if (!type_a || !type_b || type_a != type_b) return 0;
    
    // TODO Stage 1: Deep equality check
    return memcmp(a, b, type_a->size) == 0;
}

size_t instance_hash(void* instance) {
    if (!instance) return 0;
    
    TypeInfo* type = instance_get_type(instance);
    if (!type) return 0;
    
    // Basit hash
    size_t hash = 0;
    unsigned char* ptr = (unsigned char*)instance;
    for (size_t i = 0; i < type->size; i++) {
        hash = hash * 31 + ptr[i];
    }
    
    return hash;
}

// Type conversion
void* type_cast(void* value, TypeInfo* from_type, TypeInfo* to_type) {
    if (!value || !from_type || !to_type) return NULL;
    
    if (!type_info_is_compatible(from_type, to_type)) {
        return NULL;
    }
    
    // TODO Stage 1: Gerçek type casting
    return value;
}

int type_can_cast(TypeInfo* from_type, TypeInfo* to_type) {
    return type_info_is_compatible(from_type, to_type);
}

// Serialization helpers
char* type_to_string(TypeInfo* type) {
    if (!type) return NULL;
    
    // Basit string representation
    return strdup(type->name);
}

void* type_from_string(const char* str) {
    if (!str) return NULL;
    
    // TODO Stage 1: String'den TypeInfo parse et
    return type_info_get_by_name(str);
}
