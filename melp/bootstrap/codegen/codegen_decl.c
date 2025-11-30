// MELP Modular Codegen - Declaration Generation
// Değişken tanımlama kodu üretimi

#include "codegen.h"

// Forward declarations - diğer modüllerden
extern void codegen_generate_expression_value(Codegen* gen, Expression* expr);
extern int is_double_expression(Codegen* gen, Expression* expr);

// ============================================================================
// Declaration Code Generation
// ============================================================================

void codegen_generate_declaration(Codegen* gen, Declaration* decl) {
    char buffer[256];
    
    // Phase 11: Union types - currently treated as first type
    if (decl->is_union) {
        snprintf(buffer, sizeof(buffer), 
                 "    ; Union type variable '%s' (types: ", decl->name);
        codegen_emit(gen, buffer);
        
        for (int i = 0; i < decl->union_count; i++) {
            const char* type_str = decl->union_types[i] == TYPE_NUMERIC ? "numeric" :
                                   decl->union_types[i] == TYPE_DECIMAL ? "decimal" :
                                   decl->union_types[i] == TYPE_BOOLEAN ? "boolean" : "text";
            snprintf(buffer, sizeof(buffer), "    ;   - %s", type_str);
            codegen_emit(gen, buffer);
        }
        
        snprintf(buffer, sizeof(buffer), 
                 "    ; Note: Union types use first type for storage");
        codegen_emit(gen, buffer);
    }
    
    // Resolve type alias if present
    const char* actual_struct_name = decl->struct_name;
    VarType actual_type = decl->type;
    
    if (decl->struct_name) {
        TypeAlias* alias = codegen_find_type_alias(gen, decl->struct_name);
        if (alias) {
            actual_type = alias->base_type;
            actual_struct_name = alias->struct_name;
        }
    }
    
    // Check if this is a struct instance
    if (actual_struct_name) {
        codegen_generate_struct_declaration(gen, decl, actual_struct_name);
    } else if (decl->is_array) {
        codegen_generate_array_declaration(gen, decl);
    } else if (decl->is_pointer) {
        codegen_generate_pointer_declaration(gen, decl);
    } else {
        codegen_generate_simple_declaration(gen, decl, actual_type);
    }
}

// ============================================================================
// Struct Declaration
// ============================================================================

void codegen_generate_struct_declaration(Codegen* gen, Declaration* decl, const char* struct_name) {
    char buffer[256];
    
    StructInfo* struct_info = codegen_find_struct(gen, struct_name);
    if (!struct_info) {
        fprintf(stderr, "Codegen error: Undefined struct type '%s'\n", struct_name);
        exit(1);
    }
    
    gen->stack_offset += struct_info->total_size;
    codegen_add_struct_variable(gen, decl->name, gen->stack_offset, struct_name);
    
    snprintf(buffer, sizeof(buffer), 
             "    ; Declaration: %s %s (struct, size=%d bytes)", 
             struct_name, decl->name, struct_info->total_size);
    codegen_emit(gen, buffer);
    
    snprintf(buffer, sizeof(buffer),
             "    sub rsp, %d         ; Allocate space for struct %s",
             struct_info->total_size, decl->name);
    codegen_emit(gen, buffer);
    
    if (decl->init_value) {
        codegen_emit(gen, "");
        snprintf(buffer, sizeof(buffer), "    ; Struct initialization: %s = ...", decl->name);
        codegen_emit(gen, buffer);
        codegen_generate_expression_value(gen, decl->init_value);
        
        if (struct_info->field_count >= 1) {
            int field0_offset = gen->stack_offset - struct_info->field_offsets[0];
            snprintf(buffer, sizeof(buffer),
                     "    mov [rbp-%d], rax   ; Store %s.%s from return",
                     field0_offset, decl->name, struct_info->field_names[0]);
            codegen_emit(gen, buffer);
        }
        if (struct_info->field_count >= 2) {
            int field1_offset = gen->stack_offset - struct_info->field_offsets[1];
            snprintf(buffer, sizeof(buffer),
                     "    mov [rbp-%d], rdx   ; Store %s.%s from return",
                     field1_offset, decl->name, struct_info->field_names[1]);
            codegen_emit(gen, buffer);
        }
    } else {
        for (int i = 0; i < struct_info->field_count; i++) {
            int field_offset = gen->stack_offset - struct_info->field_offsets[i];
            snprintf(buffer, sizeof(buffer),
                     "    mov qword [rbp-%d], 0   ; Initialize %s.%s",
                     field_offset, decl->name, struct_info->field_names[i]);
            codegen_emit(gen, buffer);
        }
    }
}

// ============================================================================
// Array Declaration
// ============================================================================

void codegen_generate_array_declaration(Codegen* gen, Declaration* decl) {
    char buffer[256];
    int element_size = 8;
    
    if (decl->array_size == 0) {
        // Dynamic array
        gen->stack_offset += 8;
        codegen_add_dynamic_array(gen, decl->name, gen->stack_offset, decl->type);
        
        const char* type_str = decl->type == TYPE_NUMERIC ? "numeric" :
                               decl->type == TYPE_DECIMAL ? "decimal" :
                               decl->type == TYPE_BOOLEAN ? "boolean" : "text";
        
        snprintf(buffer, sizeof(buffer), 
                 "    ; Declaration: %s[] %s (dynamic array, pointer)", 
                 type_str, decl->name);
        codegen_emit(gen, buffer);
        
        snprintf(buffer, sizeof(buffer),
                 "    sub rsp, 8         ; Allocate space for pointer %s",
                 decl->name);
        codegen_emit(gen, buffer);
        
        if (decl->init_value) {
            codegen_generate_expression_value(gen, decl->init_value);
            snprintf(buffer, sizeof(buffer),
                     "    mov [rbp-%d], rax   ; Store array pointer to %s",
                     gen->stack_offset, decl->name);
            codegen_emit(gen, buffer);
        } else {
            snprintf(buffer, sizeof(buffer),
                     "    mov qword [rbp-%d], 0   ; Initialize %s to NULL",
                     gen->stack_offset, decl->name);
            codegen_emit(gen, buffer);
        }
    } else {
        // Fixed-size array
        int total_size = decl->array_size * element_size;
        
        gen->stack_offset += total_size;
        codegen_add_stack_array(gen, decl->name, gen->stack_offset, decl->type, decl->array_size);
        
        const char* type_str = decl->type == TYPE_NUMERIC ? "numeric" :
                               decl->type == TYPE_DECIMAL ? "decimal" :
                               decl->type == TYPE_BOOLEAN ? "boolean" : "text";
        
        snprintf(buffer, sizeof(buffer), 
                 "    ; Declaration: %s[] %s (array, size=%d, total=%d bytes)", 
                 type_str, decl->name, decl->array_size, total_size);
        codegen_emit(gen, buffer);
        
        snprintf(buffer, sizeof(buffer),
                 "    sub rsp, %d         ; Allocate space for array %s",
                 total_size, decl->name);
        codegen_emit(gen, buffer);
        
        if (decl->array_init) {
            for (int i = 0; i < decl->array_init_count; i++) {
                codegen_generate_expression_value(gen, decl->array_init[i]);
                int elem_offset = gen->stack_offset - (i * element_size);
                snprintf(buffer, sizeof(buffer),
                         "    mov [rbp-%d], rax   ; Initialize %s[%d]",
                         elem_offset, decl->name, i);
                codegen_emit(gen, buffer);
            }
            
            for (int i = decl->array_init_count; i < decl->array_size; i++) {
                int elem_offset = gen->stack_offset - (i * element_size);
                snprintf(buffer, sizeof(buffer),
                         "    mov qword [rbp-%d], 0   ; Initialize %s[%d]",
                         elem_offset, decl->name, i);
                codegen_emit(gen, buffer);
            }
        } else {
            for (int i = 0; i < decl->array_size; i++) {
                int elem_offset = gen->stack_offset - (i * element_size);
                snprintf(buffer, sizeof(buffer),
                         "    mov qword [rbp-%d], 0   ; Initialize %s[%d]",
                         elem_offset, decl->name, i);
                codegen_emit(gen, buffer);
            }
        }
    }
}

// ============================================================================
// Pointer Declaration
// ============================================================================

void codegen_generate_pointer_declaration(Codegen* gen, Declaration* decl) {
    char buffer[256];
    
    gen->stack_offset += 8;
    codegen_add_pointer_variable(gen, decl->name, gen->stack_offset, decl->type);
    
    const char* type_str = decl->type == TYPE_NUMERIC ? "numeric" :
                           decl->type == TYPE_DECIMAL ? "decimal" :
                           decl->type == TYPE_BOOLEAN ? "boolean" : "text";
    
    snprintf(buffer, sizeof(buffer), 
             "    ; Declaration: %s* %s (pointer)", 
             type_str, decl->name);
    codegen_emit(gen, buffer);
    
    snprintf(buffer, sizeof(buffer),
             "    sub rsp, 8         ; Allocate space for pointer %s",
             decl->name);
    codegen_emit(gen, buffer);
    
    if (decl->init_value) {
        codegen_generate_expression_value(gen, decl->init_value);
        snprintf(buffer, sizeof(buffer),
                 "    mov [rbp-%d], rax   ; Initialize %s",
                 gen->stack_offset, decl->name);
        codegen_emit(gen, buffer);
    } else {
        snprintf(buffer, sizeof(buffer),
                 "    mov qword [rbp-%d], 0   ; Initialize %s to NULL",
                 gen->stack_offset, decl->name);
        codegen_emit(gen, buffer);
    }
}

// ============================================================================
// Simple Variable Declaration (numeric, text, boolean, decimal)
// ============================================================================

void codegen_generate_simple_declaration(Codegen* gen, Declaration* decl, VarType actual_type) {
    char buffer[256];
    
    gen->stack_offset += 8;
    
    // TTO: Determine internal type from init_value
    InternalNumericType internal_num_type = INTERNAL_INT64;
    InternalStringType internal_str_type = INTERNAL_RODATA;
    
    if (actual_type == TYPE_NUMERIC && decl->init_value) {
        if (decl->init_value->type == EXPR_NUMBER) {
            internal_num_type = decl->init_value->internal_numeric_type;
        } else if (decl->init_value->type == EXPR_BINARY_OP) {
            if (is_double_expression(gen, decl->init_value)) {
                internal_num_type = INTERNAL_DOUBLE;
            }
        } else if (decl->init_value->type == EXPR_VARIABLE) {
            if (is_double_expression(gen, decl->init_value)) {
                internal_num_type = INTERNAL_DOUBLE;
            }
        }
    } else if (actual_type == TYPE_STRING && decl->init_value) {
        if (decl->init_value->type == EXPR_STRING) {
            internal_str_type = decl->init_value->internal_string_type;
        }
    }
    
    // TTO: Use appropriate add_variable function
    if (actual_type == TYPE_NUMERIC) {
        codegen_add_variable_tto(gen, decl->name, gen->stack_offset, actual_type, internal_num_type);
    } else if (actual_type == TYPE_STRING) {
        codegen_add_variable_tto_string(gen, decl->name, gen->stack_offset, actual_type, internal_str_type);
    } else {
        codegen_add_variable(gen, decl->name, gen->stack_offset, actual_type, NULL);
    }
    
    // TTO info for comment
    const char* tto_info = "";
    if (actual_type == TYPE_NUMERIC && internal_num_type == INTERNAL_DOUBLE) {
        tto_info = " (TTO:double)";
    } else if (actual_type == TYPE_STRING && internal_str_type == INTERNAL_SSO) {
        tto_info = " (TTO:SSO)";
    }
    
    snprintf(buffer, sizeof(buffer), 
             "    ; Declaration: %s %s%s", 
             actual_type == TYPE_NUMERIC ? "numeric" :
             actual_type == TYPE_DECIMAL ? "decimal" : 
             actual_type == TYPE_BOOLEAN ? "boolean" : "text",
             decl->name,
             tto_info);
    codegen_emit(gen, buffer);
    
    snprintf(buffer, sizeof(buffer),
             "    sub rsp, 8         ; Allocate space for %s",
             decl->name);
    codegen_emit(gen, buffer);
    
    if (decl->init_value) {
        codegen_generate_expression_value(gen, decl->init_value);
        snprintf(buffer, sizeof(buffer),
                 "    mov [rbp-%d], rax   ; Initialize %s",
                 gen->stack_offset, decl->name);
        codegen_emit(gen, buffer);
        
        // Check if this is a lambda with closures
        if (decl->init_value->type == EXPR_LAMBDA && decl->init_value->lambda.captured_count > 0) {
            VarInfo* var = gen->variables;
            if (var && strcmp(var->name, decl->name) == 0) {
                var->is_closure = 1;
            }
        }
    }
}
