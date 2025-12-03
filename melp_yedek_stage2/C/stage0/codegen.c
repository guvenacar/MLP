#include "codegen.h"
#include <stdio.h>
#include <string.h>

void codegen_generate(Program* program, const char* output_file) {
    FILE* f = fopen(output_file, "w");
    if (!f) {
        fprintf(stderr, "Error: Cannot open output file\n");
        return;
    }
    
    fprintf(f, "section .data\n");
    
    // String constants
    for (int i = 0; i < program->count; i++) {
        Declaration* decl = program->declarations[i];
        if (decl->type == VAR_TEXT) {
            fprintf(f, "    str_%s db \"%s\", 0\n", decl->name, decl->value);
        }
    }
    
    fprintf(f, "\nsection .bss\n");
    
    // Variables
    for (int i = 0; i < program->count; i++) {
        Declaration* decl = program->declarations[i];
        if (decl->type == VAR_NUMERIC) {
            fprintf(f, "    var_%s resq 1\n", decl->name);
        } else if (decl->type == VAR_BOOLEAN) {
            fprintf(f, "    var_%s resb 1\n", decl->name);
        } else if (decl->type == VAR_TEXT) {
            fprintf(f, "    var_%s resq 1\n", decl->name);
        }
    }
    
    fprintf(f, "\nsection .text\n");
    fprintf(f, "    global _start\n\n");
    fprintf(f, "_start:\n");
    
    // Initialize variables
    for (int i = 0; i < program->count; i++) {
        Declaration* decl = program->declarations[i];
        if (decl->type == VAR_NUMERIC) {
            fprintf(f, "    mov rax, %s\n", decl->value);
            fprintf(f, "    mov [var_%s], rax\n", decl->name);
        } else if (decl->type == VAR_BOOLEAN) {
            int val = (strcmp(decl->value, "true") == 0) ? 1 : 0;
            fprintf(f, "    mov byte [var_%s], %d\n", decl->name, val);
        } else if (decl->type == VAR_TEXT) {
            fprintf(f, "    mov rax, str_%s\n", decl->name);
            fprintf(f, "    mov [var_%s], rax\n", decl->name);
        }
    }
    
    // Exit
    fprintf(f, "\n    mov rax, 60\n");
    fprintf(f, "    xor rdi, rdi\n");
    fprintf(f, "    syscall\n");
    
    fclose(f);
}
