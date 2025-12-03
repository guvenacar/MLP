#include "variable_codegen.h"
#include <stdlib.h>
#include <string.h>

// Create codegen
VariableCodegen* variable_codegen_create(FILE* output) {
    VariableCodegen* codegen = malloc(sizeof(VariableCodegen));
    codegen->output = output;
    codegen->data_section_active = 0;
    codegen->bss_section_active = 0;
    return codegen;
}

// Free codegen
void variable_codegen_free(VariableCodegen* codegen) {
    free(codegen);
}

// Ensure .data section is active
void variable_codegen_data_section(VariableCodegen* codegen) {
    if (!codegen->data_section_active) {
        fprintf(codegen->output, "\nsection .data\n");
        codegen->data_section_active = 1;
        codegen->bss_section_active = 0;
    }
}

// Ensure .bss section is active
void variable_codegen_bss_section(VariableCodegen* codegen) {
    if (!codegen->bss_section_active) {
        fprintf(codegen->output, "\nsection .bss\n");
        codegen->bss_section_active = 1;
        codegen->data_section_active = 0;
    }
}

// Generate assembly for variable declaration
void variable_codegen_declaration(VariableCodegen* codegen, VariableDeclaration* decl) {
    if (!codegen || !decl) return;
    
    FILE* f = codegen->output;
    
    // Type description for comment
    const char* type_desc;
    if (decl->is_pointer) {
        type_desc = "pointer";
    } else if (decl->is_array) {
        type_desc = "array";
    } else if (decl->type == VAR_NUMERIC) {
        type_desc = "numeric";
    } else if (decl->type == VAR_STRING) {
        type_desc = "text";
    } else {
        type_desc = "boolean";
    }
    
    fprintf(f, "    ; Variable: %s (type: %s)\n", decl->name, type_desc);
    
    // Handle pointer types
    if (decl->is_pointer) {
        fprintf(f, "    ; Pointer to %s\n", 
                decl->base_type == VAR_NUMERIC ? "numeric" :
                decl->base_type == VAR_STRING ? "text" : "boolean");
        variable_codegen_bss_section(codegen);
        fprintf(f, "    var_%s: resq 1  ; 64-bit pointer\n", decl->name);
        
        // If initialized, set pointer value
        if (decl->value) {
            fprintf(f, "\nsection .text\n");
            codegen->data_section_active = 0;
            codegen->bss_section_active = 0;
            fprintf(f, "    mov rax, %s\n", decl->value);
            fprintf(f, "    mov [var_%s], rax\n", decl->name);
        }
        return;
    }
    
    // Handle array types
    if (decl->is_array) {
        fprintf(f, "    ; Array of %s", 
                decl->base_type == VAR_NUMERIC ? "numeric" :
                decl->base_type == VAR_STRING ? "text" : "boolean");
        
        if (decl->array_size > 0) {
            fprintf(f, " [size: %d]\n", decl->array_size);
        } else {
            fprintf(f, " [dynamic]\n");
        }
        
        variable_codegen_bss_section(codegen);
        
        if (decl->array_size > 0) {
            // Fixed-size array
            int element_size = (decl->base_type == VAR_NUMERIC) ? 8 : 8;  // 8 bytes per element
            fprintf(f, "    var_%s: resq %d  ; Array of %d elements\n", 
                    decl->name, decl->array_size, decl->array_size);
        } else {
            // Dynamic array (just a pointer)
            fprintf(f, "    var_%s: resq 1  ; Pointer to dynamic array\n", decl->name);
        }
        return;
    }
    
    if (decl->type == VAR_NUMERIC) {
        // Numeric variable
        if (decl->internal_num_type == INTERNAL_INT64) {
            fprintf(f, "    ; TTO: INT64 optimization\n");
            variable_codegen_bss_section(codegen);
            fprintf(f, "    var_%s: resq 1  ; 64-bit integer\n", decl->name);
            
            // Initialization in .text section
            fprintf(f, "\nsection .text\n");
            codegen->data_section_active = 0;
            codegen->bss_section_active = 0;
            fprintf(f, "    mov rax, %s\n", decl->value);
            fprintf(f, "    mov [var_%s], rax\n", decl->name);
            
        } else if (decl->internal_num_type == INTERNAL_DOUBLE) {
            fprintf(f, "    ; TTO: DOUBLE optimization\n");
            variable_codegen_data_section(codegen);
            fprintf(f, "    var_%s: dq %s  ; 64-bit float\n", decl->name, decl->value);
            
        } else {  // INTERNAL_BIGDECIMAL
            fprintf(f, "    ; TTO: BIGDECIMAL (large number)\n");
            variable_codegen_bss_section(codegen);
            fprintf(f, "    var_%s: resq 2  ; 128-bit for big decimal\n", decl->name);
        }
        
    } else if (decl->type == VAR_STRING) {
        // String variable
        if (decl->internal_str_type == INTERNAL_SSO) {
            fprintf(f, "    ; TTO: SSO optimization (≤23 chars)\n");
            variable_codegen_data_section(codegen);
            fprintf(f, "    var_%s: db \"%s\", 0  ; Small string inline\n", 
                    decl->name, decl->value);
            
        } else if (decl->internal_str_type == INTERNAL_RODATA) {
            fprintf(f, "    ; TTO: RODATA optimization (constant)\n");
            variable_codegen_data_section(codegen);
            fprintf(f, "    var_%s: db \"%s\", 0  ; Read-only data\n", 
                    decl->name, decl->value);
            
        } else {  // INTERNAL_HEAP
            fprintf(f, "    ; TTO: HEAP allocation (>23 chars)\n");
            variable_codegen_data_section(codegen);
            fprintf(f, "    str_%s_data: db \"%s\", 0\n", decl->name, decl->value);
            variable_codegen_bss_section(codegen);
            fprintf(f, "    var_%s: resq 1  ; Pointer to heap string\n", decl->name);
        }
        
    } else if (decl->type == VAR_BOOLEAN) {
        // Boolean variable
        fprintf(f, "    ; Boolean: %s\n", decl->value);
        variable_codegen_bss_section(codegen);
        fprintf(f, "    var_%s: resb 1  ; 1 byte for boolean\n", decl->name);
        
        fprintf(f, "\nsection .text\n");
        codegen->data_section_active = 0;
        codegen->bss_section_active = 0;
        
        int bool_val = strcmp(decl->value, "true") == 0 ? 1 : 0;
        fprintf(f, "    mov byte [var_%s], %d\n", decl->name, bool_val);
    }
}
