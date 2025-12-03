/**
 * MLP Ownership System Module - Code Generator
 * 
 * Generates x86-64 assembly with runtime ownership checks
 */

#include "ownership_system.h"

// ============================================================================
// CODEGEN HELPERS
// ============================================================================

static void emit_header(FILE* out) {
    fprintf(out, "section .data\n");
    fprintf(out, "    ownership_violation_msg db \"OWNERSHIP VIOLATION: \", 0\n");
    fprintf(out, "    use_after_move_msg db \"Use after move detected\", 10, 0\n");
    fprintf(out, "    double_borrow_msg db \"Multiple mutable borrows\", 10, 0\n");
    fprintf(out, "    lifetime_msg db \"Lifetime violation\", 10, 0\n");
    fprintf(out, "    borrow_ok_msg db \"Borrow check: OK\", 10, 0\n");
    fprintf(out, "    move_ok_msg db \"Move: OK\", 10, 0\n");
    fprintf(out, "\n");
}

static void emit_bss(FILE* out, OwnershipCtx* ctx) {
    fprintf(out, "section .bss\n");
    
    // Emit storage for each owned variable
    for (int i = 0; i < ctx->owner_count; i++) {
        OwnershipInfo* info = &ctx->owners[i];
        fprintf(out, "    %s resq 1          ; %s (owned)\n", 
                info->var_name, info->type_name);
        fprintf(out, "    %s_moved resb 1     ; move flag\n", info->var_name);
        fprintf(out, "    %s_borrow_count resq 1  ; active borrows\n", info->var_name);
    }
    
    // Emit storage for borrowed references
    for (int i = 0; i < ctx->borrow_count; i++) {
        BorrowOp* borrow = &ctx->borrows[i];
        fprintf(out, "    %s resq 1          ; ref to %s\n", 
                borrow->borrowed_as, borrow->var_name);
    }
    
    fprintf(out, "\n");
}

static void emit_text_header(FILE* out) {
    fprintf(out, "section .text\n");
    fprintf(out, "    global _start\n");
    fprintf(out, "    extern printf\n\n");
}

static void emit_runtime_checks(FILE* out) {
    fprintf(out, "; Runtime ownership check functions\n");
    fprintf(out, "check_use_after_move:\n");
    fprintf(out, "    ; rdi = variable address\n");
    fprintf(out, "    ; rsi = moved flag address\n");
    fprintf(out, "    mov al, [rsi]\n");
    fprintf(out, "    test al, al\n");
    fprintf(out, "    jz .ok\n");
    fprintf(out, "    ; Report violation\n");
    fprintf(out, "    lea rdi, [rel use_after_move_msg]\n");
    fprintf(out, "    call print_error\n");
    fprintf(out, "    mov rax, 60\n");
    fprintf(out, "    mov rdi, 1\n");
    fprintf(out, "    syscall\n");
    fprintf(out, ".ok:\n");
    fprintf(out, "    ret\n\n");
    
    fprintf(out, "check_borrow_conflict:\n");
    fprintf(out, "    ; rdi = variable borrow_count address\n");
    fprintf(out, "    ; rsi = 1 for mutable, 0 for immutable\n");
    fprintf(out, "    mov rax, [rdi]\n");
    fprintf(out, "    test rsi, rsi\n");
    fprintf(out, "    jz .immutable_borrow\n");
    fprintf(out, ".mutable_borrow:\n");
    fprintf(out, "    ; Check if already borrowed\n");
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jnz .conflict\n");
    fprintf(out, "    ; Mark as exclusively borrowed\n");
    fprintf(out, "    mov qword [rdi], 0xFFFFFFFFFFFFFFFF\n");
    fprintf(out, "    jmp .ok\n");
    fprintf(out, ".immutable_borrow:\n");
    fprintf(out, "    ; Check if mutably borrowed\n");
    fprintf(out, "    cmp rax, 0xFFFFFFFFFFFFFFFF\n");
    fprintf(out, "    je .conflict\n");
    fprintf(out, "    ; Increment shared borrow count\n");
    fprintf(out, "    inc qword [rdi]\n");
    fprintf(out, "    jmp .ok\n");
    fprintf(out, ".conflict:\n");
    fprintf(out, "    lea rdi, [rel double_borrow_msg]\n");
    fprintf(out, "    call print_error\n");
    fprintf(out, "    mov rax, 60\n");
    fprintf(out, "    mov rdi, 1\n");
    fprintf(out, "    syscall\n");
    fprintf(out, ".ok:\n");
    fprintf(out, "    ret\n\n");
    
    fprintf(out, "print_error:\n");
    fprintf(out, "    ; rdi = message address\n");
    fprintf(out, "    mov rsi, rdi\n");
    fprintf(out, "    mov rdi, 1          ; stdout\n");
    fprintf(out, "    mov rdx, 50         ; max length\n");
    fprintf(out, "    mov rax, 1          ; sys_write\n");
    fprintf(out, "    syscall\n");
    fprintf(out, "    ret\n\n");
}

// ============================================================================
// CODEGEN OPERATIONS
// ============================================================================

void ownership_codegen_borrow_check(FILE* out, BorrowOp* borrow) {
    fprintf(out, "    ; Borrow check: %s -> %s (%s)\n",
            borrow->var_name, borrow->borrowed_as,
            borrow->is_mutable ? "mutable" : "immutable");
    
    // Check if source variable is moved
    fprintf(out, "    lea rdi, [rel %s]\n", borrow->var_name);
    fprintf(out, "    lea rsi, [rel %s_moved]\n", borrow->var_name);
    fprintf(out, "    call check_use_after_move\n");
    
    // Check borrow conflicts
    fprintf(out, "    lea rdi, [rel %s_borrow_count]\n", borrow->var_name);
    fprintf(out, "    mov rsi, %d\n", borrow->is_mutable ? 1 : 0);
    fprintf(out, "    call check_borrow_conflict\n");
    
    // Create reference (just copy address)
    fprintf(out, "    lea rax, [rel %s]\n", borrow->var_name);
    fprintf(out, "    mov [rel %s], rax\n", borrow->borrowed_as);
    
    fprintf(out, "    ; Borrow successful\n\n");
}

void ownership_codegen_move(FILE* out, MoveOp* move) {
    fprintf(out, "    ; Move: %s -> %s (semantics: %d)\n",
            move->from_var, move->to_var, move->semantics);
    
    if (move->semantics == MOVE_MOVE) {
        // Check if already moved
        fprintf(out, "    lea rdi, [rel %s]\n", move->from_var);
        fprintf(out, "    lea rsi, [rel %s_moved]\n", move->from_var);
        fprintf(out, "    call check_use_after_move\n");
        
        // Transfer ownership
        fprintf(out, "    mov rax, [rel %s]\n", move->from_var);
        fprintf(out, "    mov [rel %s], rax\n", move->to_var);
        
        // Mark as moved
        fprintf(out, "    mov byte [rel %s_moved], 1\n", move->from_var);
        
    } else if (move->semantics == MOVE_COPY) {
        // Simple copy (Copy trait)
        fprintf(out, "    mov rax, [rel %s]\n", move->from_var);
        fprintf(out, "    mov [rel %s], rax\n", move->to_var);
        
    } else if (move->semantics == MOVE_CLONE) {
        // Explicit clone
        fprintf(out, "    mov rax, [rel %s]\n", move->from_var);
        fprintf(out, "    mov [rel %s], rax\n", move->to_var);
    }
    
    fprintf(out, "    ; Move complete\n\n");
}

void ownership_codegen_lifetime(FILE* out, Lifetime* lifetime) {
    fprintf(out, "    ; Lifetime '%s' (type: %s, scope: %d-%d)\n",
            lifetime->name, 
            lifetime_type_str(lifetime->type),
            lifetime->scope_start,
            lifetime->scope_end);
    
    if (lifetime->type == LIFETIME_STATIC) {
        fprintf(out, "    ; Static lifetime - no runtime check needed\n");
    } else {
        fprintf(out, "    ; Named lifetime - scope tracked at compile time\n");
    }
    
    fprintf(out, "\n");
}

void ownership_codegen_end_borrow(FILE* out, const char* borrowed_name, const char* original_var) {
    fprintf(out, "    ; End borrow: %s (return to %s)\n", borrowed_name, original_var);
    
    // Decrement borrow count (or clear exclusive borrow)
    fprintf(out, "    mov rax, [rel %s_borrow_count]\n", original_var);
    fprintf(out, "    cmp rax, 0xFFFFFFFFFFFFFFFF\n");
    fprintf(out, "    je .clear_exclusive\n");
    fprintf(out, "    ; Decrement shared borrow\n");
    fprintf(out, "    dec qword [rel %s_borrow_count]\n", original_var);
    fprintf(out, "    jmp .done\n");
    fprintf(out, ".clear_exclusive:\n");
    fprintf(out, "    ; Clear exclusive borrow\n");
    fprintf(out, "    mov qword [rel %s_borrow_count], 0\n", original_var);
    fprintf(out, ".done:\n\n");
}

// ============================================================================
// MAIN CODEGEN
// ============================================================================

void ownership_generate_asm(const char* output_file, OwnershipCtx* ctx) {
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot open %s for writing\n", output_file);
        return;
    }
    
    fprintf(out, "; MLP Ownership System - Generated Assembly\n");
    fprintf(out, "; Module #55: Ownership, Borrow Checker, Lifetimes\n\n");
    
    // Emit sections
    emit_header(out);
    emit_bss(out, ctx);
    emit_text_header(out);
    emit_runtime_checks(out);
    
    // Main program
    fprintf(out, "_start:\n");
    fprintf(out, "    ; Initialize ownership system\n\n");
    
    // Generate ownership declarations
    for (int i = 0; i < ctx->owner_count; i++) {
        OwnershipInfo* info = &ctx->owners[i];
        fprintf(out, "    ; Owner: %s : %s\n", info->var_name, info->type_name);
        fprintf(out, "    mov qword [rel %s], 0\n", info->var_name);
        fprintf(out, "    mov byte [rel %s_moved], 0\n", info->var_name);
        fprintf(out, "    mov qword [rel %s_borrow_count], 0\n\n", info->var_name);
    }
    
    // Generate borrow operations
    for (int i = 0; i < ctx->borrow_count; i++) {
        ownership_codegen_borrow_check(out, &ctx->borrows[i]);
    }
    
    // Generate move operations
    for (int i = 0; i < ctx->move_count; i++) {
        ownership_codegen_move(out, &ctx->moves[i]);
    }
    
    // Generate lifetime annotations (compile-time only, comments in asm)
    for (int i = 0; i < ctx->lifetime_count; i++) {
        ownership_codegen_lifetime(out, &ctx->lifetimes[i]);
    }
    
    // Report violations if any
    if (ctx->violation_count > 0) {
        fprintf(out, "    ; OWNERSHIP VIOLATIONS DETECTED:\n");
        for (int i = 0; i < ctx->violation_count; i++) {
            fprintf(out, "    ; Line %d: %s (%s)\n",
                    ctx->violations[i].line,
                    ctx->violations[i].message,
                    ctx->violations[i].var_name);
        }
        fprintf(out, "\n");
    }
    
    // Exit
    fprintf(out, "    ; Exit program\n");
    fprintf(out, "    mov rax, 60\n");
    fprintf(out, "    xor rdi, rdi\n");
    fprintf(out, "    syscall\n");
    
    fclose(out);
}
