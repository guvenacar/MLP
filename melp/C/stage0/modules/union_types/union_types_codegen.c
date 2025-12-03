#include "union_types.h"
#include <stdio.h>
#include <string.h>

// Generate assembly for union type definition
void union_codegen_typedef(FILE* out, UnionTypeDef* def) {
    fprintf(out, "; Union type definition: %s (kind: %d)\n", def->name, def->kind);
    fprintf(out, "; Variants:\n");
    
    for (int i = 0; i < def->variant_count; i++) {
        fprintf(out, ";   %s (tag=%d, type=%s)\n", 
                def->variants[i]->name,
                def->variants[i]->tag_value,
                def->variants[i]->type);
    }
    
    fprintf(out, "; Tag size: %d bytes\n", def->tag_size);
    fprintf(out, "\n");
}

// Generate assembly for union instance
void union_codegen_instance(FILE* out, UnionInstance* inst) {
    fprintf(out, "; Union instance: %s of type %s\n", inst->var_name, inst->type_name);
    fprintf(out, "; Active variant: %s\n", inst->active_variant);
    
    fprintf(out, "    ; Allocate union instance\n");
    fprintf(out, "    mov rdi, 16              ; tag (4 bytes) + data (max size)\n");
    fprintf(out, "    call malloc\n");
    fprintf(out, "    mov [%s], rax            ; store pointer\n", inst->var_name);
    fprintf(out, "    \n");
    fprintf(out, "    ; Set variant tag\n");
    fprintf(out, "    mov rbx, [%s]\n", inst->var_name);
    fprintf(out, "    mov dword [rbx], %d      ; write tag for %s\n", 
            0, inst->active_variant);  // Tag value would come from typedef
    fprintf(out, "    \n");
    fprintf(out, "    ; Store variant data\n");
    
    if (inst->value && strlen(inst->value) > 0) {
        fprintf(out, "    mov qword [rbx+8], %s    ; store value\n", inst->value);
    }
    
    fprintf(out, "\n");
}

// Generate assembly for union operation
void union_codegen_operation(FILE* out, UnionOpNode* op) {
    fprintf(out, "; Union operation: %d\n", op->op);
    
    switch (op->op) {
        case UNION_OP_MATCH:
            fprintf(out, "    ; Pattern match on: %s\n", op->union_name);
            fprintf(out, "    mov rax, [%s]        ; load union pointer\n", op->union_name);
            fprintf(out, "    mov edx, [rax]       ; load tag\n");
            fprintf(out, "    \n");
            fprintf(out, "    ; Match arms\n");
            
            for (int i = 0; i < op->case_count; i++) {
                fprintf(out, "    cmp edx, %d          ; check variant %s\n", 
                        i, op->cases[i]->pattern);
                fprintf(out, "    je .match_arm_%d\n", i);
            }
            fprintf(out, "    jmp .match_end\n");
            fprintf(out, "    \n");
            
            for (int i = 0; i < op->case_count; i++) {
                fprintf(out, ".match_arm_%d:\n", i);
                fprintf(out, "    ; %s => %s\n", 
                        op->cases[i]->pattern,
                        op->cases[i]->body);
                fprintf(out, "    jmp .match_end\n");
            }
            fprintf(out, ".match_end:\n");
            break;
            
        case UNION_OP_GET_TAG:
            fprintf(out, "    ; Get active variant tag\n");
            fprintf(out, "    mov rax, [%s]\n", op->union_name);
            fprintf(out, "    mov eax, [rax]       ; load tag\n");
            break;
            
        case UNION_OP_IS_VARIANT:
            fprintf(out, "    ; Check if variant is: %s\n", op->variant_name);
            fprintf(out, "    mov rax, [%s]\n", op->union_name);
            fprintf(out, "    mov edx, [rax]\n");
            fprintf(out, "    cmp edx, %d          ; compare with variant tag\n", 0);
            fprintf(out, "    sete al              ; set AL if equal\n");
            break;
            
        case UNION_OP_UNWRAP:
            fprintf(out, "    ; Unwrap variant data\n");
            fprintf(out, "    mov rax, [%s]\n", op->union_name);
            fprintf(out, "    mov rax, [rax+8]     ; load data\n");
            break;
            
        case UNION_OP_AS_REF:
            fprintf(out, "    ; Get reference to variant data\n");
            fprintf(out, "    mov rax, [%s]\n", op->union_name);
            fprintf(out, "    add rax, 8           ; pointer to data field\n");
            break;
            
        default:
            fprintf(out, "    ; Unknown operation\n");
    }
    
    fprintf(out, "\n");
}

// Generate full assembly for union types module
void union_codegen_full(UnionContext* ctx, const char* output_file) {
    FILE* out = fopen(output_file, "w");
    if (!out) {
        ctx->error_message = strdup("Failed to open output file");
        return;
    }
    
    fprintf(out, "section .data\n");
    fprintf(out, "    ; Union types data section\n\n");
    
    fprintf(out, "section .bss\n");
    for (int i = 0; i < ctx->instance_count; i++) {
        fprintf(out, "    %s resq 1    ; %s instance\n",
                ctx->instances[i]->var_name,
                ctx->instances[i]->type_name);
    }
    fprintf(out, "\n");
    
    fprintf(out, "section .text\n");
    fprintf(out, "    global main\n");
    fprintf(out, "    extern malloc, free, printf\n\n");
    
    // Generate type definitions (as comments)
    for (int i = 0; i < ctx->typedef_count; i++) {
        union_codegen_typedef(out, ctx->type_defs[i]);
    }
    
    fprintf(out, "main:\n");
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n\n");
    
    // Generate code for all instances
    for (int i = 0; i < ctx->instance_count; i++) {
        union_codegen_instance(out, ctx->instances[i]);
    }
    
    // Generate code for all operations
    for (int i = 0; i < ctx->op_count; i++) {
        union_codegen_operation(out, ctx->operations[i]);
    }
    
    fprintf(out, "    ; Clean exit\n");
    fprintf(out, "    mov rax, 0\n");
    fprintf(out, "    mov rsp, rbp\n");
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
    
    fclose(out);
}
