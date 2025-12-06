#include "iterator_system.h"
#include <stdio.h>
#include <string.h>

// Generate assembly for iterator declaration
void iter_codegen_declaration(FILE* out, IteratorDecl* decl) {
    fprintf(out, "; Iterator declaration: %s (type: %d)\n", decl->var_name, decl->type);
    
    switch (decl->type) {
        case ITER_ARRAY:
            fprintf(out, "    ; Array iterator for: %s\n", decl->source);
            fprintf(out, "    mov rax, [%s]     ; load array pointer\n", decl->source);
            fprintf(out, "    mov [%s_ptr], rax ; store in iterator\n", decl->var_name);
            fprintf(out, "    mov qword [%s_idx], 0  ; initialize index\n", decl->var_name);
            break;
            
        case ITER_RANGE:
            fprintf(out, "    ; Range iterator: %s\n", decl->source);
            fprintf(out, "    ; Parse range bounds and create iterator\n");
            fprintf(out, "    mov qword [%s_current], 0\n", decl->var_name);
            fprintf(out, "    mov qword [%s_end], 10\n", decl->var_name);
            break;
            
        case ITER_MAP:
            fprintf(out, "    ; Map iterator with transform\n");
            fprintf(out, "    ; Store transformation function pointer\n");
            break;
            
        case ITER_FILTER:
            fprintf(out, "    ; Filter iterator with predicate\n");
            fprintf(out, "    ; Store predicate function pointer\n");
            break;
            
        default:
            fprintf(out, "    ; Generic iterator\n");
    }
    
    fprintf(out, "\n");
}

// Generate assembly for iterator operation
void iter_codegen_operation(FILE* out, IteratorOpNode* op) {
    fprintf(out, "; Iterator operation: %d on %s\n", op->op, op->iterator_name);
    
    switch (op->op) {
        case ITER_OP_NEXT:
            fprintf(out, "    ; Get next element\n");
            fprintf(out, "    mov rax, [%s_ptr]\n", op->iterator_name);
            fprintf(out, "    mov rbx, [%s_idx]\n", op->iterator_name);
            fprintf(out, "    mov rax, [rax + rbx*8]  ; load element\n");
            fprintf(out, "    inc qword [%s_idx]      ; increment index\n", op->iterator_name);
            break;
            
        case ITER_OP_HAS_NEXT:
            fprintf(out, "    ; Check if has more elements\n");
            fprintf(out, "    mov rax, [%s_idx]\n", op->iterator_name);
            fprintf(out, "    mov rbx, [%s_len]\n", op->iterator_name);
            fprintf(out, "    cmp rax, rbx\n");
            fprintf(out, "    setl al              ; set AL if rax < rbx\n");
            break;
            
        case ITER_OP_MAP:
            fprintf(out, "    ; Map transformation: %s\n", op->function ? op->function : "none");
            fprintf(out, "    ; Apply function to each element\n");
            fprintf(out, ".map_loop_%s:\n", op->iterator_name);
            fprintf(out, "    ; Get next element\n");
            fprintf(out, "    mov rax, [%s_ptr]\n", op->iterator_name);
            fprintf(out, "    mov rbx, [%s_idx]\n", op->iterator_name);
            fprintf(out, "    mov rdi, [rax + rbx*8]\n");
            fprintf(out, "    ; Call transform function\n");
            fprintf(out, "    call %s\n", op->function ? op->function : "identity");
            fprintf(out, "    ; Store result\n");
            fprintf(out, "    inc qword [%s_idx]\n", op->iterator_name);
            break;
            
        case ITER_OP_FILTER:
            fprintf(out, "    ; Filter with predicate: %s\n", op->function ? op->function : "none");
            fprintf(out, ".filter_loop_%s:\n", op->iterator_name);
            fprintf(out, "    ; Get next element\n");
            fprintf(out, "    mov rax, [%s_ptr]\n", op->iterator_name);
            fprintf(out, "    mov rbx, [%s_idx]\n", op->iterator_name);
            fprintf(out, "    mov rdi, [rax + rbx*8]\n");
            fprintf(out, "    ; Call predicate function\n");
            fprintf(out, "    call %s\n", op->function ? op->function : "always_true");
            fprintf(out, "    test rax, rax\n");
            fprintf(out, "    jz .filter_skip_%s\n", op->iterator_name);
            fprintf(out, "    ; Include element\n");
            fprintf(out, ".filter_skip_%s:\n", op->iterator_name);
            fprintf(out, "    inc qword [%s_idx]\n", op->iterator_name);
            break;
            
        case ITER_OP_REDUCE:
            fprintf(out, "    ; Reduce with function: %s\n", op->function ? op->function : "none");
            fprintf(out, "    mov rax, 0           ; accumulator\n");
            fprintf(out, ".reduce_loop_%s:\n", op->iterator_name);
            fprintf(out, "    ; Get next element\n");
            fprintf(out, "    mov rbx, [%s_ptr]\n", op->iterator_name);
            fprintf(out, "    mov rcx, [%s_idx]\n", op->iterator_name);
            fprintf(out, "    mov rdi, rax         ; accumulator\n");
            fprintf(out, "    mov rsi, [rbx + rcx*8]  ; element\n");
            fprintf(out, "    call %s\n", op->function ? op->function : "add");
            fprintf(out, "    inc qword [%s_idx]\n", op->iterator_name);
            fprintf(out, "    ; Check if done\n");
            fprintf(out, "    mov rbx, [%s_idx]\n", op->iterator_name);
            fprintf(out, "    cmp rbx, [%s_len]\n", op->iterator_name);
            fprintf(out, "    jl .reduce_loop_%s\n", op->iterator_name);
            break;
            
        case ITER_OP_COLLECT:
            fprintf(out, "    ; Collect into collection\n");
            fprintf(out, "    ; Allocate result array\n");
            fprintf(out, "    mov rdi, [%s_len]\n", op->iterator_name);
            fprintf(out, "    shl rdi, 3           ; multiply by 8\n");
            fprintf(out, "    call malloc\n");
            fprintf(out, "    mov [result_array], rax\n");
            break;
            
        case ITER_OP_CHAIN:
            fprintf(out, "    ; Chain iterators\n");
            fprintf(out, "    ; Link first and second iterator\n");
            break;
            
        case ITER_OP_ENUMERATE:
            fprintf(out, "    ; Add enumeration\n");
            fprintf(out, "    ; Pair index with element\n");
            break;
            
        case ITER_OP_TAKE:
            fprintf(out, "    ; Take first n elements\n");
            fprintf(out, "    mov qword [%s_limit], %s\n", 
                    op->iterator_name, 
                    op->arg_count > 0 ? op->args[0] : "10");
            break;
            
        case ITER_OP_SKIP:
            fprintf(out, "    ; Skip first n elements\n");
            fprintf(out, "    add qword [%s_idx], %s\n",
                    op->iterator_name,
                    op->arg_count > 0 ? op->args[0] : "0");
            break;
            
        case ITER_OP_FOREACH:
            fprintf(out, "    ; Execute for each element\n");
            fprintf(out, ".foreach_loop_%s:\n", op->iterator_name);
            fprintf(out, "    ; Get element and call function\n");
            break;
            
        case ITER_OP_COUNT:
            fprintf(out, "    ; Count elements\n");
            fprintf(out, "    mov rax, [%s_len]\n", op->iterator_name);
            break;
            
        default:
            fprintf(out, "    ; Unknown operation\n");
    }
    
    fprintf(out, "\n");
}

// Generate full assembly for iterator module
void iter_codegen_full(IteratorContext* ctx, const char* output_file) {
    FILE* out = fopen(output_file, "w");
    if (!out) {
        ctx->error_message = strdup("Failed to open output file");
        return;
    }
    
    fprintf(out, "section .data\n");
    fprintf(out, "    ; Iterator data section\n\n");
    
    fprintf(out, "section .bss\n");
    for (int i = 0; i < ctx->decl_count; i++) {
        fprintf(out, "    %s_ptr resq 1      ; iterator pointer\n", ctx->declarations[i]->var_name);
        fprintf(out, "    %s_idx resq 1      ; current index\n", ctx->declarations[i]->var_name);
        fprintf(out, "    %s_len resq 1      ; length\n", ctx->declarations[i]->var_name);
    }
    fprintf(out, "    result_array resq 1\n\n");
    
    fprintf(out, "section .text\n");
    fprintf(out, "    global main\n");
    fprintf(out, "    extern malloc, free, printf\n\n");
    
    fprintf(out, "main:\n");
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n\n");
    
    // Generate code for all declarations
    for (int i = 0; i < ctx->decl_count; i++) {
        iter_codegen_declaration(out, ctx->declarations[i]);
    }
    
    // Generate code for all operations
    for (int i = 0; i < ctx->op_count; i++) {
        iter_codegen_operation(out, ctx->operations[i]);
    }
    
    fprintf(out, "    ; Clean exit\n");
    fprintf(out, "    mov rax, 0\n");
    fprintf(out, "    mov rsp, rbp\n");
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
    
    fclose(out);
}
