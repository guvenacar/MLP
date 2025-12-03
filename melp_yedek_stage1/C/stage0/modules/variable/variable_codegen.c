#include "variable_codegen.h"
#include <stdio.h>
#include <string.h>

void codegen_generate(Program* program, const char* output_file) {
    FILE* f = fopen(output_file, "w");
    if (!f) {
        fprintf(stderr, "Error: Cannot open output file\n");
        return;
    }
    
    fprintf(f, "section .data\n");
    
    // String constants (INTERNAL_RODATA and INTERNAL_SSO both go to .data for now)
    for (int i = 0; i < program->count; i++) {
        Declaration* decl = program->declarations[i];
        if (decl->type == VAR_STRING) {
            fprintf(f, "    str_%s db \"%s\", 0  ; %s\n", 
                    decl->name, decl->value,
                    decl->internal_str_type == INTERNAL_SSO ? "SSO (≤23 bytes)" : "HEAP");
        }
    }
    
    fprintf(f, "\nsection .bss\n");
    
    // Variables with smart storage comments
    for (int i = 0; i < program->count; i++) {
        Declaration* decl = program->declarations[i];
        if (decl->type == VAR_NUMERIC) {
            const char* type_comment = "";
            if (decl->internal_num_type == INTERNAL_INT64) {
                type_comment = " ; INT64 (small integer)";
            } else if (decl->internal_num_type == INTERNAL_DOUBLE) {
                type_comment = " ; DOUBLE (floating point)";
            } else if (decl->internal_num_type == INTERNAL_BIGDECIMAL) {
                type_comment = " ; BIGDECIMAL (large number)";
            }
            fprintf(f, "    var_%s resq 1%s\n", decl->name, type_comment);
        } else if (decl->type == VAR_BOOLEAN) {
            fprintf(f, "    var_%s resb 1  ; BOOLEAN\n", decl->name);
        } else if (decl->type == VAR_STRING) {
            fprintf(f, "    var_%s resq 1  ; STRING pointer\n", decl->name);
        }
    }
    
    fprintf(f, "\nsection .text\n");
    fprintf(f, "    global _start\n\n");
    fprintf(f, "_start:\n");
    
    // Initialize variables with optimized code
    for (int i = 0; i < program->count; i++) {
        Declaration* decl = program->declarations[i];
        if (decl->type == VAR_NUMERIC) {
            fprintf(f, "    ; %s = %s (%s)\n", 
                    decl->name, decl->value,
                    decl->internal_num_type == INTERNAL_INT64 ? "INT64" : 
                    decl->internal_num_type == INTERNAL_DOUBLE ? "DOUBLE" : "BIGDECIMAL");
            
            if (decl->internal_num_type == INTERNAL_INT64) {
                // Small integer: direct immediate value
                fprintf(f, "    mov rax, %s\n", decl->value);
                fprintf(f, "    mov [var_%s], rax\n", decl->name);
            } else if (decl->internal_num_type == INTERNAL_DOUBLE) {
                // TODO: Floating point support (Phase 2)
                fprintf(f, "    ; TODO: DOUBLE support\n");
                fprintf(f, "    mov rax, 0  ; placeholder\n");
                fprintf(f, "    mov [var_%s], rax\n", decl->name);
            } else {
                // TODO: BigDecimal support (Phase 3)
                fprintf(f, "    ; TODO: BIGDECIMAL support\n");
                fprintf(f, "    mov rax, 0  ; placeholder\n");
                fprintf(f, "    mov [var_%s], rax\n", decl->name);
            }
        } else if (decl->type == VAR_BOOLEAN) {
            int val = (strcmp(decl->value, "true") == 0) ? 1 : 0;
            fprintf(f, "    ; %s = %s\n", decl->name, decl->value);
            fprintf(f, "    mov byte [var_%s], %d\n", decl->name, val);
        } else if (decl->type == VAR_STRING) {
            fprintf(f, "    ; %s = \"%s\" (%s)\n", 
                    decl->name, decl->value,
                    decl->internal_str_type == INTERNAL_SSO ? "SSO" : "HEAP");
            fprintf(f, "    mov rax, str_%s\n", decl->name);
            fprintf(f, "    mov [var_%s], rax\n", decl->name);
        }
    }
    
    // Exit
    fprintf(f, "\n    ; Exit program\n");
    fprintf(f, "    mov rax, 60\n");
    fprintf(f, "    xor rdi, rdi\n");
    fprintf(f, "    syscall\n");
    
    fclose(f);
}
