// Reflection Codegen - Header
// Runtime type information için assembly generation

#ifndef REFLECTION_CODEGEN_H
#define REFLECTION_CODEGEN_H

#include "reflection.h"

// Type metadata codegen
void codegen_type_info(TypeInfo* type);
void codegen_field_info(FieldInfo* field);
void codegen_method_info(MethodInfo* method);
void codegen_attribute_info(AttributeInfo* attr);

// Type registry codegen
void codegen_type_registry_init(TypeRegistry* registry);
void codegen_type_registry_register(TypeInfo* type);

// Reflection operation codegen
void codegen_typeof(void* expression);
void codegen_type_assertion(void* expression, TypeInfo* target_type);
void codegen_type_check(void* expression, TypeInfo* target_type);

// Field/method access codegen
void codegen_get_field(TypeInfo* type, const char* field_name);
void codegen_set_field(TypeInfo* type, const char* field_name);
void codegen_invoke_method(TypeInfo* type, const char* method_name);

// Instance operations codegen
void codegen_instance_clone(void* instance);
void codegen_instance_equals(void* a, void* b);
void codegen_instance_hash(void* instance);

#endif // REFLECTION_CODEGEN_H
