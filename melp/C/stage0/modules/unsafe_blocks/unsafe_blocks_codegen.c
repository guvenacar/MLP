#include "unsafe_blocks.h"
#include <stdio.h>
#include <string.h>

// Generate assembly for unsafe context
void unsafe_codegen_context(FILE* out, UnsafeContext* ctx) {
    fprintf(out, "; Unsafe context: %s (type: %d)\n", ctx->name, ctx->type);
    
    switch (ctx->type) {
        case UNSAFE_BLOCK:
            fprintf(out, "    ; Begin unsafe block\n");
            fprintf(out, "    ; Safety checks disabled within this scope\n");
            break;
            
        case UNSAFE_FUNCTION:
            fprintf(out, "    ; Unsafe function: %s\n", ctx->name);
            fprintf(out, "%s:\n", ctx->name);
            fprintf(out, "    push rbp\n");
            fprintf(out, "    mov rbp, rsp\n");
            break;
            
        case UNSAFE_TRAIT:
            fprintf(out, "    ; Unsafe trait implementation\n");
            break;
            
        default:
            fprintf(out, "    ; Unknown unsafe context\n");
    }
    
    fprintf(out, "\n");
}

// Generate assembly for unsafe operation
void unsafe_codegen_operation(FILE* out, UnsafeOpNode* op) {
    fprintf(out, "; Unsafe operation: %d\n", op->op);
    fprintf(out, "; Code: %s\n", op->code);
    
    switch (op->op) {
        case UNSAFE_OP_DEREF_RAW:
            fprintf(out, "    ; Dereference raw pointer: %s\n", 
                    op->target ? op->target : "unknown");
            fprintf(out, "    mov rax, [%s]        ; load raw pointer\n",
                    op->target ? op->target : "rbx");
            fprintf(out, "    mov rax, [rax]       ; dereference (UNSAFE)\n");
            break;
            
        case UNSAFE_OP_RAW_PTR_CREATE:
            fprintf(out, "    ; Create raw pointer\n");
            fprintf(out, "    lea rax, [variable]  ; get address\n");
            fprintf(out, "    ; Now rax holds raw pointer\n");
            break;
            
        case UNSAFE_OP_RAW_PTR_CAST:
            fprintf(out, "    ; Cast raw pointer\n");
            fprintf(out, "    mov rax, [ptr]       ; load pointer\n");
            fprintf(out, "    ; Cast to different type (UNSAFE)\n");
            break;
            
        case UNSAFE_OP_TRANSMUTE:
            fprintf(out, "    ; Type transmutation (UNSAFE)\n");
            fprintf(out, "    ; Reinterpret bits as different type\n");
            fprintf(out, "    mov rax, [source]    ; load source value\n");
            fprintf(out, "    ; Now treat as target type\n");
            break;
            
        case UNSAFE_OP_INLINE_ASM:
            fprintf(out, "    ; Inline assembly (UNSAFE)\n");
            fprintf(out, "    ; Direct machine code execution\n");
            fprintf(out, "    ; asm! { ... }\n");
            break;
            
        case UNSAFE_OP_FFI_CALL:
            fprintf(out, "    ; Foreign function call (UNSAFE)\n");
            fprintf(out, "    ; Call external C function\n");
            fprintf(out, "    call external_function\n");
            break;
            
        case UNSAFE_OP_ACCESS_UNION:
            fprintf(out, "    ; Access union field (UNSAFE)\n");
            fprintf(out, "    mov rax, [union_ptr]\n");
            fprintf(out, "    mov rax, [rax+8]     ; access field\n");
            break;
            
        case UNSAFE_OP_STATIC_MUT:
            fprintf(out, "    ; Access mutable static (UNSAFE)\n");
            fprintf(out, "    mov rax, [static_var]\n");
            fprintf(out, "    inc qword [static_var]\n");
            break;
            
        default:
            fprintf(out, "    ; Unknown unsafe operation\n");
    }
    
    fprintf(out, "\n");
}

// Generate full assembly for unsafe blocks module
void unsafe_codegen_full(UnsafeModuleContext* ctx, const char* output_file) {
    FILE* out = fopen(output_file, "w");
    if (!out) {
        ctx->error_message = strdup("Failed to open output file");
        return;
    }
    
    fprintf(out, "section .data\n");
    fprintf(out, "    ; Unsafe module data section\n");
    fprintf(out, "    warning_msg db 'UNSAFE: Undefined behavior possible', 10, 0\n\n");
    
    fprintf(out, "section .bss\n");
    for (int i = 0; i < ctx->pointer_count; i++) {
        fprintf(out, "    %s resq 1    ; %s pointer (%s)\n",
                ctx->raw_pointers[i]->name,
                ctx->raw_pointers[i]->is_const ? "const" : "mut",
                ctx->raw_pointers[i]->pointee_type);
    }
    fprintf(out, "\n");
    
    fprintf(out, "section .text\n");
    fprintf(out, "    global main\n");
    fprintf(out, "    extern malloc, free, printf\n\n");
    
    // Generate unsafe function contexts
    for (int i = 0; i < ctx->context_count; i++) {
        if (ctx->contexts[i]->type == UNSAFE_FUNCTION) {
            unsafe_codegen_context(out, ctx->contexts[i]);
        }
    }
    
    fprintf(out, "main:\n");
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n\n");
    
    // Generate unsafe blocks
    for (int i = 0; i < ctx->context_count; i++) {
        if (ctx->contexts[i]->type == UNSAFE_BLOCK) {
            unsafe_codegen_context(out, ctx->contexts[i]);
        }
    }
    
    // Generate all unsafe operations
    for (int i = 0; i < ctx->op_count; i++) {
        unsafe_codegen_operation(out, ctx->operations[i]);
    }
    
    fprintf(out, "    ; End unsafe operations\n");
    fprintf(out, "    mov rax, 0\n");
    fprintf(out, "    mov rsp, rbp\n");
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
    
    // Generate unsafe function epilogues
    for (int i = 0; i < ctx->context_count; i++) {
        if (ctx->contexts[i]->type == UNSAFE_FUNCTION) {
            fprintf(out, "\n; End %s\n", ctx->contexts[i]->name);
            fprintf(out, "    mov rsp, rbp\n");
            fprintf(out, "    pop rbp\n");
            fprintf(out, "    ret\n");
        }
    }
    
    fclose(out);
}
