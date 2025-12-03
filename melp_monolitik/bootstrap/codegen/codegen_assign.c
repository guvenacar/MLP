// ============================================================================
// MELP Modular Codegen - codegen_assign.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// İçerik: codegen_generate_assignment
// Atama ifadelerini işler (basit, struct field, array index, pointer deref)

void codegen_generate_assignment(Codegen* gen, char* var_name, char* field_name, Expression* array_index, Expression* dereference_target, Expression* value) {
    char buffer[256];
    
    if (dereference_target != NULL) {
        // Phase 10: Pointer dereference assignment: *ptr = value
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Pointer dereference assignment: *ptr = ...");
        
        // Evaluate value first
        codegen_generate_expression_value(gen, value);
        codegen_emit(gen, "    push rax            ; Save value");
        
        // Evaluate pointer expression (get address)
        codegen_generate_expression_value(gen, dereference_target);
        codegen_emit(gen, "    mov rbx, rax        ; Move address to rbx");
        codegen_emit(gen, "    pop rax             ; Restore value");
        codegen_emit(gen, "    mov [rbx], rax      ; Store value at address");
    } else if (array_index != NULL) {
        // Array assignment: arr[index] = value
        VarInfo* var_info = codegen_get_variable(gen, var_name);
        
        codegen_emit(gen, "");
        snprintf(buffer, sizeof(buffer), "    ; Array assignment: %s[...] = ...", var_name);
        codegen_emit(gen, buffer);
        
        // Evaluate index expression
        codegen_generate_expression_value(gen, array_index);
        codegen_emit(gen, "    push rax            ; Save index");
        
        // Evaluate value expression
        codegen_generate_expression_value(gen, value);
        codegen_emit(gen, "    mov rbx, rax        ; Save value in rbx");
        codegen_emit(gen, "    pop rax             ; Restore index");
        
        if (var_info->is_dynamic_array) {
            // Dynamic array: load pointer, then store at offset
            codegen_emit(gen, "    push rbx            ; Save value");
            snprintf(buffer, sizeof(buffer), "    mov rcx, [rbp-%d]   ; Load array pointer", var_info->stack_offset);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    pop rbx             ; Restore value");
            codegen_emit(gen, "    mov rdx, 8          ; Element size");
            codegen_emit(gen, "    imul rax, rdx       ; index * 8");
            codegen_emit(gen, "    add rcx, rax        ; ptr + (index * 8)");
            codegen_emit(gen, "    mov [rcx], rbx      ; Store value to array[index]");
        } else {
            // Fixed-size array: stack-based addressing
            int base_offset = var_info->stack_offset;
            
            // Calculate element address: rbp - (base - index*8)
            codegen_emit(gen, "    mov rcx, 8          ; Element size");
            codegen_emit(gen, "    imul rax, rcx       ; index * 8");
            snprintf(buffer, sizeof(buffer), "    mov rcx, %d         ; Base offset", base_offset);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    sub rcx, rax        ; base - (index * 8)");
            codegen_emit(gen, "    neg rcx             ; -(base - index*8)");
            codegen_emit(gen, "    mov [rbp + rcx], rbx ; Store value to array[index]");
        }
    } else if (field_name == NULL) {
        // Regular variable assignment
        codegen_emit(gen, "");
        snprintf(buffer, sizeof(buffer), "    ; Assignment: %s = ...", var_name);
        codegen_emit(gen, buffer);
        
        // Phase 18: Check if this is a state variable
        int is_state_var = 0;
        for (int i = 0; i < gen->global_var_count; i++) {
            if (strcmp(gen->global_vars[i].name, var_name) == 0) {
                is_state_var = 1;
                // Evaluate expression and store to state
                codegen_generate_expression_value(gen, value);
                snprintf(buffer, sizeof(buffer), "    mov [state_%s], rax   ; Store to state %s",
                         var_name, var_name);
                codegen_emit(gen, buffer);
                break;
            }
        }
        
        if (!is_state_var) {
            int offset = codegen_find_variable(gen, var_name);
            
            // Evaluate expression and store result
            codegen_generate_expression_value(gen, value);
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store to %s",
                     offset, var_name);
            codegen_emit(gen, buffer);
        }
    } else {
        // Field assignment: object.field = value
        int offset = codegen_find_variable(gen, var_name);
        
        codegen_emit(gen, "");
        snprintf(buffer, sizeof(buffer), "    ; Field assignment: %s.%s = ...", var_name, field_name);
        codegen_emit(gen, buffer);
        
        // Find variable info to get struct name
        VarInfo* var_info = codegen_get_variable(gen, var_name);
        
        if (!var_info || !var_info->struct_name) {
            fprintf(stderr, "Codegen error: Variable '%s' is not a struct\n", var_name);
            exit(1);
        }
        
        // Find struct definition
        StructInfo* struct_info = codegen_find_struct(gen, var_info->struct_name);
        if (!struct_info) {
            fprintf(stderr, "Codegen error: Struct '%s' not found\n", var_info->struct_name);
            exit(1);
        }
        
        // Find field offset
        int field_offset = -1;
        for (int i = 0; i < struct_info->field_count; i++) {
            if (strcmp(struct_info->field_names[i], field_name) == 0) {
                field_offset = struct_info->field_offsets[i];
                break;
            }
        }
        
        if (field_offset < 0) {
            fprintf(stderr, "Codegen error: Field '%s' not found in struct '%s'\n", 
                    field_name, var_info->struct_name);
            exit(1);
        }
        
        // Evaluate expression and store to field
        codegen_generate_expression_value(gen, value);
        int actual_offset = offset - field_offset;
        snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store to %s.%s",
                 actual_offset, var_name, field_name);
        codegen_emit(gen, buffer);
    }
}

