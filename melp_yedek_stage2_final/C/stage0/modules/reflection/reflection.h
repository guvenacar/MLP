// Reflection Module - Header
// Runtime type information ve metadata access

#ifndef REFLECTION_H
#define REFLECTION_H

#include <stddef.h>
#include <stdint.h>

// Type categories
typedef enum {
    TYPE_PRIMITIVE,    // int, float, bool, vb.
    TYPE_STRUCT,       // Struct types
    TYPE_ENUM,         // Enum types
    TYPE_ARRAY,        // Array types
    TYPE_POINTER,      // Pointer types
    TYPE_FUNCTION,     // Function types
    TYPE_INTERFACE,    // Interface/Trait types
    TYPE_GENERIC       // Generic types
} TypeCategory;

// Primitive types
typedef enum {
    PRIM_INT8,
    PRIM_INT16,
    PRIM_INT32,
    PRIM_INT64,
    PRIM_UINT8,
    PRIM_UINT16,
    PRIM_UINT32,
    PRIM_UINT64,
    PRIM_FLOAT32,
    PRIM_FLOAT64,
    PRIM_BOOL,
    PRIM_CHAR,
    PRIM_STRING,
    PRIM_VOID
} PrimitiveType;

// Type flags
typedef enum {
    TYPE_FLAG_CONST = 1 << 0,      // Const qualifier
    TYPE_FLAG_VOLATILE = 1 << 1,   // Volatile qualifier
    TYPE_FLAG_MUTABLE = 1 << 2,    // Mutable
    TYPE_FLAG_PUBLIC = 1 << 3,     // Public visibility
    TYPE_FLAG_PRIVATE = 1 << 4     // Private visibility
} TypeFlags;

// Field metadata
typedef struct {
    char* name;            // Field adı
    void* type_info;       // TypeInfo pointer
    size_t offset;         // Struct içindeki offset
    size_t size;           // Field boyutu
    uint32_t flags;        // Field flags
    void* attributes;      // Attributes/annotations
    size_t attribute_count;
} FieldInfo;

// Method metadata
typedef struct {
    char* name;            // Method adı
    void* return_type;     // Dönüş tipi
    void** param_types;    // Parametre tipleri
    size_t param_count;    // Parametre sayısı
    void* function_ptr;    // Function pointer
    uint32_t flags;        // Method flags
    void* attributes;      // Attributes
    size_t attribute_count;
} MethodInfo;

// Type metadata
typedef struct {
    char* name;            // Tip adı
    TypeCategory category; // Tip kategorisi
    size_t size;           // Boyut (bytes)
    size_t alignment;      // Alignment
    uint32_t flags;        // Type flags
    
    // Category-specific data
    union {
        PrimitiveType primitive;
        struct {
            FieldInfo** fields;
            size_t field_count;
            MethodInfo** methods;
            size_t method_count;
        } struct_info;
        struct {
            char** variants;
            int* values;
            size_t variant_count;
        } enum_info;
        struct {
            void* element_type;
            size_t length;
        } array_info;
        struct {
            void* pointee_type;
        } pointer_info;
        struct {
            void* return_type;
            void** param_types;
            size_t param_count;
        } function_info;
    } data;
    
    void* attributes;      // Type attributes
    size_t attribute_count;
} TypeInfo;

// Attribute/Annotation
typedef struct {
    char* name;            // Attribute adı
    void** values;         // Attribute değerleri
    size_t value_count;    // Değer sayısı
} AttributeInfo;

// Runtime type registry
typedef struct {
    TypeInfo** types;      // Kayıtlı tipler
    size_t type_count;
    size_t type_capacity;
    void* type_map;        // Name -> TypeInfo map
} TypeRegistry;

// Instance metadata
typedef struct {
    void* instance;        // Instance pointer
    TypeInfo* type_info;   // Type information
    size_t ref_count;      // Reference count
} InstanceInfo;

// Type query operations
TypeInfo* type_info_create(const char* name, TypeCategory category);
void type_info_free(TypeInfo* type_info);
TypeInfo* type_info_get_by_name(const char* name);
TypeInfo* type_info_get_by_instance(void* instance);
int type_info_is_compatible(TypeInfo* from, TypeInfo* to);
int type_info_is_subtype(TypeInfo* derived, TypeInfo* base);

// Field operations
FieldInfo* field_info_create(const char* name, TypeInfo* type, size_t offset);
void field_info_free(FieldInfo* field);
FieldInfo* type_info_get_field(TypeInfo* type, const char* name);
void* field_info_get_value(FieldInfo* field, void* instance);
void field_info_set_value(FieldInfo* field, void* instance, void* value);
size_t type_info_get_field_count(TypeInfo* type);
FieldInfo** type_info_get_fields(TypeInfo* type);

// Method operations
MethodInfo* method_info_create(const char* name, TypeInfo* return_type);
void method_info_free(MethodInfo* method);
MethodInfo* type_info_get_method(TypeInfo* type, const char* name);
void* method_info_invoke(MethodInfo* method, void* instance, void** args);
size_t type_info_get_method_count(TypeInfo* type);
MethodInfo** type_info_get_methods(TypeInfo* type);

// Attribute operations
AttributeInfo* attribute_info_create(const char* name);
void attribute_info_free(AttributeInfo* attr);
void attribute_info_add_value(AttributeInfo* attr, void* value);
AttributeInfo* type_info_get_attribute(TypeInfo* type, const char* name);
int type_info_has_attribute(TypeInfo* type, const char* name);

// Registry operations
TypeRegistry* type_registry_create(void);
void type_registry_free(TypeRegistry* registry);
void type_registry_register(TypeRegistry* registry, TypeInfo* type_info);
TypeInfo* type_registry_lookup(TypeRegistry* registry, const char* name);
size_t type_registry_get_count(TypeRegistry* registry);
TypeInfo** type_registry_get_all(TypeRegistry* registry);

// Instance operations
InstanceInfo* instance_info_create(void* instance, TypeInfo* type_info);
void instance_info_free(InstanceInfo* info);
TypeInfo* instance_get_type(void* instance);
void* instance_clone(void* instance);
int instance_equals(void* a, void* b);
size_t instance_hash(void* instance);

// Type conversion
void* type_cast(void* value, TypeInfo* from_type, TypeInfo* to_type);
int type_can_cast(TypeInfo* from_type, TypeInfo* to_type);

// Serialization helpers
char* type_to_string(TypeInfo* type);
void* type_from_string(const char* str);

#endif // REFLECTION_H
