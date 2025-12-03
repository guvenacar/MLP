// Reflection Parser - Header
// Type metadata ve reflection parsing

#ifndef REFLECTION_PARSER_H
#define REFLECTION_PARSER_H

#include "reflection.h"

// Parser fonksiyonları (Stage 1 için placeholder)
TypeInfo* parse_type_declaration(void* tokens);
FieldInfo* parse_field_declaration(void* tokens);
MethodInfo* parse_method_declaration(void* tokens);
AttributeInfo* parse_attribute(void* tokens);

// Type query parsing
void* parse_typeof_expression(void* tokens);
void* parse_type_assertion(void* tokens);
void* parse_type_check(void* tokens);

// Reflection operation parsing
void* parse_field_access(void* tokens);
void* parse_method_invocation(void* tokens);
void* parse_attribute_query(void* tokens);

#endif // REFLECTION_PARSER_H
