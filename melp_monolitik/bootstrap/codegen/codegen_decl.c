// ============================================================================
// MELP Modular Codegen - codegen_decl.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// İçerik: codegen_generate_declaration
// Değişken bildirimlerini işler

void codegen_generate_declaration(Codegen* gen, Declaration* decl) {
    char buffer[256];
    
    // Phase 11: Union types - currently treated as first type (no runtime type checking)
    if (decl->is_union) {
        snprintf(buffer, sizeof(buffer), 
                 "    ; Union type variable '%s' (types: ", decl->name);
        codegen_emit(gen, buffer);
        
        for (int i = 0; i < decl->union_count; i++) {
            const char* type_str = decl->union_types[i] == TYPE_NUMERIC ? "numeric" :
                                   decl->union_types[i] == TYPE_DECIMAL ? "decimal" :
                                   decl->union_types[i] == TYPE_BOOLEAN ? "boolean" : "string";
            snprintf(buffer, sizeof(buffer), "    ;   - %s", type_str);
            codegen_emit(gen, buffer);
        }
        
        snprintf(buffer, sizeof(buffer), 
                 "    ; Note: Union types use first type for storage (runtime checking not implemented)");
        codegen_emit(gen, buffer);
    }
    
    // Resolve type alias if present
    const char* actual_struct_name = decl->struct_name;
    VarType actual_type = decl->type;
    
    if (decl->struct_name) {
        // Check if this is a type alias
        TypeAlias* alias = codegen_find_type_alias(gen, decl->struct_name);
        if (alias) {
            // Resolve alias to actual type
            actual_type = alias->base_type;
            actual_struct_name = alias->struct_name; // May be NULL (primitive) or another struct name
        }
    }
    
    // Check if this is a struct instance (after alias resolution)
    if (actual_struct_name) {
        // Struct instance declaration
        StructInfo* struct_info = codegen_find_struct(gen, actual_struct_name);
        if (!struct_info) {
            fprintf(stderr, "Codegen error: Undefined struct type '%s'\n", actual_struct_name);
            exit(1);
        }
        
        // Allocate space for entire struct
        gen->stack_offset += struct_info->total_size;
        codegen_add_struct_variable(gen, decl->name, gen->stack_offset, actual_struct_name);
        
        snprintf(buffer, sizeof(buffer), 
                 "    ; Declaration: %s %s (struct, size=%d bytes)", 
                 actual_struct_name, decl->name, struct_info->total_size);
        codegen_emit(gen, buffer);
        
        snprintf(buffer, sizeof(buffer),
                 "    sub rsp, %d         ; Allocate space for struct %s",
                 struct_info->total_size, decl->name);
        codegen_emit(gen, buffer);
        
        // Check if struct has initialization (e.g., Point p = createPoint(x, y))
        if (decl->init_value) {
            // Generate the function call - result in rax (field 0), rdx (field 1)
            codegen_emit(gen, "");
            snprintf(buffer, sizeof(buffer), "    ; Struct initialization: %s = ...", decl->name);
            codegen_emit(gen, buffer);
            codegen_generate_expression_value(gen, decl->init_value);
            
            // Store returned values into struct fields
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
            // Initialize fields to zero
            for (int i = 0; i < struct_info->field_count; i++) {
                int field_offset = gen->stack_offset - struct_info->field_offsets[i];
                snprintf(buffer, sizeof(buffer),
                         "    mov qword [rbp-%d], 0   ; Initialize %s.%s",
                         field_offset, decl->name, struct_info->field_names[i]);
                codegen_emit(gen, buffer);
            }
        }
    } else if (decl->is_array) {
        // Array declaration
        int element_size = 8; // All types are 8 bytes for simplicity
        
        if (decl->array_size == 0) {
            // Dynamic array - store pointer (8 bytes)
            gen->stack_offset += 8;
            codegen_add_dynamic_array(gen, decl->name, gen->stack_offset, decl->type);
            
            const char* type_str = decl->type == TYPE_NUMERIC ? "numeric" :
                                   decl->type == TYPE_DECIMAL ? "decimal" :
                                   decl->type == TYPE_BOOLEAN ? "boolean" : "string";
            
            snprintf(buffer, sizeof(buffer), 
                     "    ; Declaration: %s[] %s (dynamic array, pointer)", 
                     type_str, decl->name);
            codegen_emit(gen, buffer);
            
            snprintf(buffer, sizeof(buffer),
                     "    sub rsp, 8         ; Allocate space for pointer %s",
                     decl->name);
            codegen_emit(gen, buffer);
            
            // Initialize pointer
            if (decl->init_value) {
                // init_value should be malloc(size) call
                codegen_generate_expression_value(gen, decl->init_value);
                snprintf(buffer, sizeof(buffer),
                         "    mov [rbp-%d], rax   ; Store array pointer to %s",
                         gen->stack_offset, decl->name);
                codegen_emit(gen, buffer);
            } else {
                // Initialize to NULL
                snprintf(buffer, sizeof(buffer),
                         "    mov qword [rbp-%d], 0   ; Initialize %s to NULL",
                         gen->stack_offset, decl->name);
                codegen_emit(gen, buffer);
            }
        } else {
            // Fixed-size array - allocate on stack
            int total_size = decl->array_size * element_size;
            
            gen->stack_offset += total_size;
            // Phase 14: Use codegen_add_stack_array to track array info for iterators
            codegen_add_stack_array(gen, decl->name, gen->stack_offset, decl->type, decl->array_size);
            
            const char* type_str = decl->type == TYPE_NUMERIC ? "numeric" :
                                   decl->type == TYPE_DECIMAL ? "decimal" :
                                   decl->type == TYPE_BOOLEAN ? "boolean" : "string";
            
            snprintf(buffer, sizeof(buffer), 
                     "    ; Declaration: %s[] %s (array, size=%d, total=%d bytes)", 
                     type_str, decl->name, decl->array_size, total_size);
            codegen_emit(gen, buffer);
            
            snprintf(buffer, sizeof(buffer),
                     "    sub rsp, %d         ; Allocate space for array %s",
                     total_size, decl->name);
            codegen_emit(gen, buffer);
            
            // Initialize array elements
            if (decl->array_init) {
                // Initialize from array literal
                for (int i = 0; i < decl->array_init_count; i++) {
                    codegen_generate_expression_value(gen, decl->array_init[i]);
                    int elem_offset = gen->stack_offset - (i * element_size);
                    snprintf(buffer, sizeof(buffer),
                             "    mov [rbp-%d], rax   ; Initialize %s[%d]",
                             elem_offset, decl->name, i);
                    codegen_emit(gen, buffer);
                }
                
                // Zero-initialize remaining elements
                for (int i = decl->array_init_count; i < decl->array_size; i++) {
                    int elem_offset = gen->stack_offset - (i * element_size);
                    snprintf(buffer, sizeof(buffer),
                             "    mov qword [rbp-%d], 0   ; Initialize %s[%d]",
                             elem_offset, decl->name, i);
                    codegen_emit(gen, buffer);
                }
            } else {
                // Zero-initialize all elements
                for (int i = 0; i < decl->array_size; i++) {
                    int elem_offset = gen->stack_offset - (i * element_size);
                    snprintf(buffer, sizeof(buffer),
                             "    mov qword [rbp-%d], 0   ; Initialize %s[%d]",
                             elem_offset, decl->name, i);
                    codegen_emit(gen, buffer);
                }
            }
        }
    } else if (decl->is_pointer) {
        // Phase 10: Pointer variable declaration
        gen->stack_offset += 8;  // Pointers are 8 bytes (64-bit addresses)
        codegen_add_pointer_variable(gen, decl->name, gen->stack_offset, decl->type);
        
        const char* type_str = decl->type == TYPE_NUMERIC ? "numeric" :
                               decl->type == TYPE_DECIMAL ? "decimal" :
                               decl->type == TYPE_BOOLEAN ? "boolean" : "string";
        
        snprintf(buffer, sizeof(buffer), 
                 "    ; Declaration: %s* %s (pointer)", 
                 type_str, decl->name);
        codegen_emit(gen, buffer);
        
        snprintf(buffer, sizeof(buffer),
                 "    sub rsp, 8         ; Allocate space for pointer %s",
                 decl->name);
        codegen_emit(gen, buffer);
        
        // Initialize pointer
        if (decl->init_value) {
            codegen_generate_expression_value(gen, decl->init_value);
            snprintf(buffer, sizeof(buffer),
                     "    mov [rbp-%d], rax   ; Initialize %s",
                     gen->stack_offset, decl->name);
            codegen_emit(gen, buffer);
        } else {
            // Initialize to NULL (0)
            snprintf(buffer, sizeof(buffer),
                     "    mov qword [rbp-%d], 0   ; Initialize %s to NULL",
                     gen->stack_offset, decl->name);
            codegen_emit(gen, buffer);
        }
    } else {
        // Regular variable declaration (may be through type alias)
        gen->stack_offset += 8;
        
        // TTO: Determine internal type from init_value
        InternalNumericType internal_num_type = INTERNAL_INT64;  // Default
        InternalStringType internal_str_type = INTERNAL_RODATA;  // Default
        
        if (actual_type == TYPE_NUMERIC && decl->init_value) {
            if (decl->init_value->type == EXPR_NUMBER) {
                internal_num_type = decl->init_value->internal_numeric_type;
            } else if (decl->init_value->type == EXPR_BINARY_OP) {
                // TTO: Check if binary op result would be double
                if (is_double_expression(gen, decl->init_value)) {
                    internal_num_type = INTERNAL_DOUBLE;
                }
            } else if (decl->init_value->type == EXPR_VARIABLE) {
                // TTO: Inherit type from source variable
                if (is_double_expression(gen, decl->init_value)) {
                    internal_num_type = INTERNAL_DOUBLE;
                }
            }
        } else if (actual_type == TYPE_STRING && decl->init_value) {
            if (decl->init_value->type == EXPR_STRING) {
                internal_str_type = decl->init_value->internal_string_type;
            }
        }
        
        // TTO: Use appropriate add_variable function based on type
        if (actual_type == TYPE_NUMERIC) {
            codegen_add_variable_tto(gen, decl->name, gen->stack_offset, actual_type, internal_num_type);
        } else if (actual_type == TYPE_STRING) {
            codegen_add_variable_tto_string(gen, decl->name, gen->stack_offset, actual_type, internal_str_type);
        } else {
            codegen_add_variable(gen, decl->name, gen->stack_offset, actual_type, NULL);
        }
        
        // TTO: Add type info to comment
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
                 actual_type == TYPE_BOOLEAN ? "boolean" : "string",
                 decl->name,
                 tto_info);
        codegen_emit(gen, buffer);
        
        snprintf(buffer, sizeof(buffer),
                 "    sub rsp, 8         ; Allocate space for %s",
                 decl->name);
        codegen_emit(gen, buffer);
        
        // Initialize if value provided
        if (decl->init_value) {
            codegen_generate_expression_value(gen, decl->init_value);
            snprintf(buffer, sizeof(buffer),
                     "    mov [rbp-%d], rax   ; Initialize %s",
                     gen->stack_offset, decl->name);
            codegen_emit(gen, buffer);
            
            // Check if this is a lambda with closures
            if (decl->init_value->type == EXPR_LAMBDA && decl->init_value->lambda.captured_count > 0) {
                // Mark variable as closure
                VarInfo* var = gen->variables;
                if (var && strcmp(var->name, decl->name) == 0) {
                    var->is_closure = 1;
                }
            }
        }
    }
}

