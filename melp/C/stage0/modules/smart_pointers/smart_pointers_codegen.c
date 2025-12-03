#include "smart_pointers.h"
#include <stdio.h>
#include <string.h>

// Generate assembly for smart pointer declaration
void sp_codegen_declaration(FILE* out, SmartPtrDecl* decl) {
    fprintf(out, "; Smart pointer declaration: %s (type: %d)\n", 
            decl->var_name, decl->type);
    
    switch (decl->type) {
        case SP_BOX:
            fprintf(out, "    ; Box<%s> - unique ownership\n", decl->inner_type);
            fprintf(out, "    mov rdi, 8       ; size for pointer\n");
            fprintf(out, "    call malloc      ; allocate heap memory\n");
            fprintf(out, "    mov [%s], rax    ; store pointer\n", decl->var_name);
            break;
            
        case SP_RC:
            fprintf(out, "    ; Rc<%s> - reference counted\n", decl->inner_type);
            fprintf(out, "    mov rdi, 16      ; size for pointer + refcount\n");
            fprintf(out, "    call malloc\n");
            fprintf(out, "    mov qword [rax], 1   ; initialize refcount to 1\n");
            fprintf(out, "    mov [%s], rax    ; store pointer\n", decl->var_name);
            break;
            
        case SP_ARC:
            fprintf(out, "    ; Arc<%s> - atomic reference counted\n", decl->inner_type);
            fprintf(out, "    mov rdi, 16      ; size for pointer + atomic refcount\n");
            fprintf(out, "    call malloc\n");
            fprintf(out, "    mov qword [rax], 1   ; initialize refcount to 1\n");
            fprintf(out, "    mov [%s], rax    ; store pointer\n", decl->var_name);
            break;
            
        case SP_WEAK:
            fprintf(out, "    ; Weak<%s> - weak reference\n", decl->inner_type);
            fprintf(out, "    mov rdi, 8\n");
            fprintf(out, "    call malloc\n");
            fprintf(out, "    mov [%s], rax    ; store weak pointer\n", decl->var_name);
            break;
            
        default:
            fprintf(out, "    ; Unknown smart pointer type\n");
    }
    
    fprintf(out, "\n");
}

// Generate assembly for smart pointer operation
void sp_codegen_operation(FILE* out, SmartPtrOpNode* op) {
    fprintf(out, "; Smart pointer operation on: %s (op: %d)\n", 
            op->ptr_name, op->op);
    
    switch (op->op) {
        case SP_OP_NEW:
            fprintf(out, "    ; Box/Rc/Arc::new(value)\n");
            fprintf(out, "    mov rdi, 8\n");
            fprintf(out, "    call malloc\n");
            fprintf(out, "    ; Store value in allocated memory\n");
            break;
            
        case SP_OP_CLONE:
            fprintf(out, "    ; Clone reference counted pointer\n");
            fprintf(out, "    mov rax, [%s]    ; load pointer\n", op->ptr_name);
            fprintf(out, "    inc qword [rax]  ; increment refcount\n");
            fprintf(out, "    ; Return cloned pointer in rax\n");
            break;
            
        case SP_OP_DROP:
            fprintf(out, "    ; Drop smart pointer\n");
            fprintf(out, "    mov rax, [%s]    ; load pointer\n", op->ptr_name);
            fprintf(out, "    dec qword [rax]  ; decrement refcount\n");
            fprintf(out, "    jnz .skip_free_%s\n", op->ptr_name);
            fprintf(out, "    mov rdi, rax\n");
            fprintf(out, "    call free        ; free if refcount = 0\n");
            fprintf(out, ".skip_free_%s:\n", op->ptr_name);
            break;
            
        case SP_OP_DEREF:
            fprintf(out, "    ; Dereference smart pointer\n");
            fprintf(out, "    mov rax, [%s]    ; load pointer\n", op->ptr_name);
            fprintf(out, "    mov rax, [rax+8] ; load value\n");
            break;
            
        case SP_OP_STRONG_COUNT:
            fprintf(out, "    ; Get strong reference count\n");
            fprintf(out, "    mov rax, [%s]    ; load pointer\n", op->ptr_name);
            fprintf(out, "    mov rax, [rax]   ; load refcount\n");
            break;
            
        case SP_OP_DOWNGRADE:
            fprintf(out, "    ; Downgrade Rc to Weak\n");
            fprintf(out, "    mov rax, [%s]    ; load strong pointer\n", op->ptr_name);
            fprintf(out, "    ; Create weak reference without incrementing strong count\n");
            break;
            
        case SP_OP_UPGRADE:
            fprintf(out, "    ; Upgrade Weak to Rc\n");
            fprintf(out, "    mov rax, [%s]    ; load weak pointer\n", op->ptr_name);
            fprintf(out, "    cmp qword [rax], 0  ; check if still valid\n");
            fprintf(out, "    je .upgrade_failed_%s\n", op->ptr_name);
            fprintf(out, "    inc qword [rax]  ; increment strong count\n");
            fprintf(out, ".upgrade_failed_%s:\n", op->ptr_name);
            break;
            
        default:
            fprintf(out, "    ; Unknown operation\n");
    }
    
    fprintf(out, "\n");
}

// Generate full assembly for smart pointer module
void sp_codegen_full(SmartPtrContext* ctx, const char* output_file) {
    FILE* out = fopen(output_file, "w");
    if (!out) {
        ctx->error_message = strdup("Failed to open output file");
        return;
    }
    
    fprintf(out, "section .data\n");
    fprintf(out, "    ; Smart pointer data section\n\n");
    
    fprintf(out, "section .bss\n");
    for (int i = 0; i < ctx->decl_count; i++) {
        fprintf(out, "    %s resq 1    ; %s\n", 
                ctx->declarations[i]->var_name,
                ctx->declarations[i]->inner_type);
    }
    fprintf(out, "\n");
    
    fprintf(out, "section .text\n");
    fprintf(out, "    global main\n");
    fprintf(out, "    extern malloc, free, printf\n\n");
    
    fprintf(out, "main:\n");
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n\n");
    
    // Generate code for all declarations
    for (int i = 0; i < ctx->decl_count; i++) {
        sp_codegen_declaration(out, ctx->declarations[i]);
    }
    
    // Generate code for all operations
    for (int i = 0; i < ctx->op_count; i++) {
        sp_codegen_operation(out, ctx->operations[i]);
    }
    
    fprintf(out, "    ; Clean exit\n");
    fprintf(out, "    mov rax, 0\n");
    fprintf(out, "    mov rsp, rbp\n");
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
    
    fclose(out);
}
