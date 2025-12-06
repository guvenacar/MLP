// Reflection Codegen - Implementation
// Runtime type information için assembly generation

#include "reflection_codegen.h"
#include <stdio.h>
#include <string.h>

// Type metadata codegen
void codegen_type_info(TypeInfo* type) {
    if (!type) return;
    
    printf("; TypeInfo: %s\n", type->name);
    printf("section .data\n");
    printf("    type_%s:\n", type->name);
    printf("        dq type_name_%s      ; name pointer\n", type->name);
    printf("        dd %d                 ; category\n", type->category);
    printf("        dq %zu                ; size\n", type->size);
    printf("        dq %zu                ; alignment\n", type->alignment);
    printf("        dd %u                 ; flags\n", type->flags);
    printf("\n");
    printf("    type_name_%s: db \"%s\", 0\n", type->name, type->name);
    printf("\n");
}

void codegen_field_info(FieldInfo* field) {
    if (!field) return;
    
    printf("; FieldInfo: %s\n", field->name);
    printf("    field_%s:\n", field->name);
    printf("        dq field_name_%s     ; name\n", field->name);
    printf("        dq 0                  ; type_info\n");
    printf("        dq %zu                ; offset\n", field->offset);
    printf("        dq %zu                ; size\n", field->size);
    printf("        dd %u                 ; flags\n", field->flags);
    printf("\n");
    printf("    field_name_%s: db \"%s\", 0\n", field->name, field->name);
    printf("\n");
}

void codegen_method_info(MethodInfo* method) {
    if (!method) return;
    
    printf("; MethodInfo: %s\n", method->name);
    printf("    method_%s:\n", method->name);
    printf("        dq method_name_%s    ; name\n", method->name);
    printf("        dq 0                  ; return_type\n");
    printf("        dq 0                  ; param_types\n");
    printf("        dq %zu                ; param_count\n", method->param_count);
    printf("        dq 0                  ; function_ptr\n");
    printf("\n");
    printf("    method_name_%s: db \"%s\", 0\n", method->name, method->name);
    printf("\n");
}

void codegen_attribute_info(AttributeInfo* attr) {
    if (!attr) return;
    
    printf("; AttributeInfo: %s\n", attr->name);
    printf("    attribute_%s:\n", attr->name);
    printf("        dq attr_name_%s      ; name\n", attr->name);
    printf("        dq 0                  ; values\n");
    printf("        dq %zu                ; value_count\n", attr->value_count);
    printf("\n");
    printf("    attr_name_%s: db \"%s\", 0\n", attr->name, attr->name);
    printf("\n");
}

// Type registry codegen
void codegen_type_registry_init(TypeRegistry* registry) {
    if (!registry) return;
    
    printf("; Type Registry Initialization\n");
    printf("section .bss\n");
    printf("    global_type_registry: resq 1\n");
    printf("\n");
    printf("section .text\n");
    printf("init_type_registry:\n");
    printf("    call type_registry_create\n");
    printf("    mov [global_type_registry], rax\n");
    printf("    ret\n");
    printf("\n");
}

void codegen_type_registry_register(TypeInfo* type) {
    if (!type) return;
    
    printf("; Register Type: %s\n", type->name);
    printf("    mov rdi, [global_type_registry]\n");
    printf("    lea rsi, [type_%s]\n", type->name);
    printf("    call type_registry_register\n");
    printf("\n");
}

// Reflection operation codegen
void codegen_typeof(void* expression) {
    (void)expression;
    
    printf("; typeof() operation\n");
    printf("    mov rdi, [expression_ptr]\n");
    printf("    call instance_get_type\n");
    printf("    ; rax = TypeInfo*\n");
    printf("\n");
}

void codegen_type_assertion(void* expression, TypeInfo* target_type) {
    (void)expression;
    if (!target_type) return;
    
    printf("; Type assertion: as %s\n", target_type->name);
    printf("    mov rdi, [expression_ptr]\n");
    printf("    mov rsi, [source_type]\n");
    printf("    lea rdx, [type_%s]\n", target_type->name);
    printf("    call type_cast\n");
    printf("    test rax, rax\n");
    printf("    jz .cast_failed\n");
    printf("\n");
}

void codegen_type_check(void* expression, TypeInfo* target_type) {
    (void)expression;
    if (!target_type) return;
    
    printf("; Type check: is %s\n", target_type->name);
    printf("    mov rdi, [expression_ptr]\n");
    printf("    call instance_get_type\n");
    printf("    lea rsi, [type_%s]\n", target_type->name);
    printf("    call type_info_is_compatible\n");
    printf("    ; rax = 1 if compatible\n");
    printf("\n");
}

// Field/method access codegen
void codegen_get_field(TypeInfo* type, const char* field_name) {
    if (!type || !field_name) return;
    
    printf("; Get field: %s.%s\n", type->name, field_name);
    printf("    lea rdi, [type_%s]\n", type->name);
    printf("    lea rsi, [field_name_%s]\n", field_name);
    printf("    call type_info_get_field\n");
    printf("    ; rax = FieldInfo*\n");
    printf("    mov rdi, rax\n");
    printf("    mov rsi, [instance_ptr]\n");
    printf("    call field_info_get_value\n");
    printf("    ; rax = field value pointer\n");
    printf("\n");
}

void codegen_set_field(TypeInfo* type, const char* field_name) {
    if (!type || !field_name) return;
    
    printf("; Set field: %s.%s\n", type->name, field_name);
    printf("    lea rdi, [type_%s]\n", type->name);
    printf("    lea rsi, [field_name_%s]\n", field_name);
    printf("    call type_info_get_field\n");
    printf("    mov rdi, rax\n");
    printf("    mov rsi, [instance_ptr]\n");
    printf("    mov rdx, [new_value]\n");
    printf("    call field_info_set_value\n");
    printf("\n");
}

void codegen_invoke_method(TypeInfo* type, const char* method_name) {
    if (!type || !method_name) return;
    
    printf("; Invoke method: %s.%s()\n", type->name, method_name);
    printf("    lea rdi, [type_%s]\n", type->name);
    printf("    lea rsi, [method_name_%s]\n", method_name);
    printf("    call type_info_get_method\n");
    printf("    ; rax = MethodInfo*\n");
    printf("    mov rdi, rax\n");
    printf("    mov rsi, [instance_ptr]\n");
    printf("    lea rdx, [arguments]\n");
    printf("    call method_info_invoke\n");
    printf("    ; rax = return value\n");
    printf("\n");
}

// Instance operations codegen
void codegen_instance_clone(void* instance) {
    (void)instance;
    
    printf("; Instance clone\n");
    printf("    mov rdi, [instance_ptr]\n");
    printf("    call instance_clone\n");
    printf("    ; rax = cloned instance\n");
    printf("\n");
}

void codegen_instance_equals(void* a, void* b) {
    (void)a;
    (void)b;
    
    printf("; Instance equality check\n");
    printf("    mov rdi, [instance_a]\n");
    printf("    mov rsi, [instance_b]\n");
    printf("    call instance_equals\n");
    printf("    ; rax = 1 if equal\n");
    printf("\n");
}

void codegen_instance_hash(void* instance) {
    (void)instance;
    
    printf("; Instance hash\n");
    printf("    mov rdi, [instance_ptr]\n");
    printf("    call instance_hash\n");
    printf("    ; rax = hash value\n");
    printf("\n");
}
