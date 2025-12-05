#include "channels.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ============================================================================
// ASSEMBLY GENERATION - CHANNEL DECLARATION
// ============================================================================

char* codegen_channel_declaration(ChannelDecl* decl, ChannelContext* ctx) {
    // Generate assembly for: dim ch as Channel<i32>
    // Calls: channel_create(ctx, "i32", sizeof(i32), capacity)
    
    (void)ctx;  // Unused but kept for API consistency
    
    if (!decl) return NULL;
    
    char* asm_code = (char*)malloc(4096);
    size_t offset = 0;
    
    offset += sprintf(asm_code + offset, 
        "; Channel declaration: %s as Channel<%s>\n",
        decl->var_name, decl->element_type);
    
    // Determine element size based on type
    size_t element_size = 8;  // Default: pointer size
    
    if (strcmp(decl->element_type, "i32") == 0 ||
        strcmp(decl->element_type, "u32") == 0) {
        element_size = 4;
    } else if (strcmp(decl->element_type, "i64") == 0 ||
               strcmp(decl->element_type, "u64") == 0) {
        element_size = 8;
    } else if (strcmp(decl->element_type, "i16") == 0 ||
               strcmp(decl->element_type, "u16") == 0) {
        element_size = 2;
    } else if (strcmp(decl->element_type, "i8") == 0 ||
               strcmp(decl->element_type, "u8") == 0) {
        element_size = 1;
    }
    
    // Push capacity (4th arg)
    offset += sprintf(asm_code + offset, "    mov rcx, %zu\n", decl->capacity);
    
    // Push element_size (3rd arg)
    offset += sprintf(asm_code + offset, "    mov rdx, %zu\n", element_size);
    
    // Push type_name (2nd arg)
    offset += sprintf(asm_code + offset, 
        "    lea rsi, [rel .type_name_%s]\n", 
        decl->var_name);
    
    // Push context (1st arg)
    offset += sprintf(asm_code + offset, "    mov rdi, [rel channel_context]\n");
    
    // Call channel_create
    offset += sprintf(asm_code + offset, "    call channel_create\n");
    
    // Store result in variable
    offset += sprintf(asm_code + offset, "    mov [rel %s], rax\n", decl->var_name);
    
    // Add string constant for type name
    offset += sprintf(asm_code + offset, "\n");
    offset += sprintf(asm_code + offset, "section .rodata\n");
    offset += sprintf(asm_code + offset, ".type_name_%s:\n", decl->var_name);
    offset += sprintf(asm_code + offset, "    db \"%s\", 0\n", decl->element_type);
    offset += sprintf(asm_code + offset, "section .text\n\n");
    
    return asm_code;
}

// ============================================================================
// ASSEMBLY GENERATION - CHANNEL OPERATIONS
// ============================================================================

char* codegen_channel_send(ChannelOp* op, ChannelContext* ctx) {
    // Generate assembly for: send(ch, value)
    // Calls: channel_send(ctx, ch, &value)
    
    (void)ctx;  // Unused but kept for API consistency
    
    if (!op || op->op_type != CHAN_OP_SEND) return NULL;
    
    char* asm_code = (char*)malloc(4096);
    size_t offset = 0;
    
    offset += sprintf(asm_code + offset, 
        "; Channel send: %s <- %s\n",
        op->channel_name, op->value_expr);
    
    // Evaluate value expression and store in temporary
    offset += sprintf(asm_code + offset, "    ; Evaluate value expression\n");
    offset += sprintf(asm_code + offset, "    mov rax, %s\n", op->value_expr);
    offset += sprintf(asm_code + offset, "    mov [rel .send_value_temp], rax\n");
    
    // Push &value (3rd arg)
    offset += sprintf(asm_code + offset, "    lea rdx, [rel .send_value_temp]\n");
    
    // Push channel (2nd arg)
    offset += sprintf(asm_code + offset, "    mov rsi, [rel %s]\n", op->channel_name);
    
    // Push context (1st arg)
    offset += sprintf(asm_code + offset, "    mov rdi, [rel channel_context]\n");
    
    // Call channel_send or channel_try_send
    if (op->is_blocking) {
        offset += sprintf(asm_code + offset, "    call channel_send\n");
    } else {
        offset += sprintf(asm_code + offset, "    call channel_try_send\n");
        
        // Store result if result_var is specified
        if (op->result_var) {
            offset += sprintf(asm_code + offset, "    mov [rel %s], rax\n", op->result_var);
        }
    }
    
    offset += sprintf(asm_code + offset, "\n");
    offset += sprintf(asm_code + offset, "section .bss\n");
    offset += sprintf(asm_code + offset, ".send_value_temp: resq 1\n");
    offset += sprintf(asm_code + offset, "section .text\n\n");
    
    return asm_code;
}

char* codegen_channel_receive(ChannelOp* op, ChannelContext* ctx) {
    // Generate assembly for: value = receive(ch)
    // Calls: channel_receive(ctx, ch)
    
    (void)ctx;  // Unused but kept for API consistency
    
    if (!op || op->op_type != CHAN_OP_RECEIVE) return NULL;
    
    char* asm_code = (char*)malloc(4096);
    size_t offset = 0;
    
    offset += sprintf(asm_code + offset, 
        "; Channel receive: %s <- %s\n",
        op->result_var ? op->result_var : "?",
        op->channel_name);
    
    // Push channel (2nd arg)
    offset += sprintf(asm_code + offset, "    mov rsi, [rel %s]\n", op->channel_name);
    
    // Push context (1st arg)
    offset += sprintf(asm_code + offset, "    mov rdi, [rel channel_context]\n");
    
    // Call channel_receive or channel_try_receive
    if (op->is_blocking) {
        offset += sprintf(asm_code + offset, "    call channel_receive\n");
    } else {
        // For try_receive, pass &ok as 3rd arg
        offset += sprintf(asm_code + offset, "    lea rdx, [rel .receive_ok_temp]\n");
        offset += sprintf(asm_code + offset, "    call channel_try_receive\n");
    }
    
    // Store result in variable
    if (op->result_var) {
        offset += sprintf(asm_code + offset, "    ; Dereference pointer\n");
        offset += sprintf(asm_code + offset, "    mov rbx, [rax]\n");
        offset += sprintf(asm_code + offset, "    mov [rel %s], rbx\n", op->result_var);
        
        // Free the allocated pointer
        offset += sprintf(asm_code + offset, "    mov rdi, rax\n");
        offset += sprintf(asm_code + offset, "    call free\n");
    }
    
    if (!op->is_blocking) {
        offset += sprintf(asm_code + offset, "\n");
        offset += sprintf(asm_code + offset, "section .bss\n");
        offset += sprintf(asm_code + offset, ".receive_ok_temp: resb 1\n");
        offset += sprintf(asm_code + offset, "section .text\n");
    }
    
    offset += sprintf(asm_code + offset, "\n");
    
    return asm_code;
}

char* codegen_channel_close(ChannelOp* op, ChannelContext* ctx) {
    // Generate assembly for: close(ch)
    // Calls: channel_close(ctx, ch)
    
    (void)ctx;  // Unused but kept for API consistency
    
    if (!op || op->op_type != CHAN_OP_CLOSE) return NULL;
    
    char* asm_code = (char*)malloc(4096);
    size_t offset = 0;
    
    offset += sprintf(asm_code + offset, 
        "; Channel close: %s\n",
        op->channel_name);
    
    // Push channel (2nd arg)
    offset += sprintf(asm_code + offset, "    mov rsi, [rel %s]\n", op->channel_name);
    
    // Push context (1st arg)
    offset += sprintf(asm_code + offset, "    mov rdi, [rel channel_context]\n");
    
    // Call channel_close
    offset += sprintf(asm_code + offset, "    call channel_close\n\n");
    
    return asm_code;
}

// ============================================================================
// ASSEMBLY GENERATION - SELECT STATEMENT
// ============================================================================

char* codegen_channel_select(SelectStmt* stmt, ChannelContext* ctx) {
    // Generate assembly for select statement
    // Calls: channel_select(ctx, cases, case_count)
    
    (void)ctx;  // Unused but kept for API consistency
    
    if (!stmt) return NULL;
    
    char* asm_code = (char*)malloc(8192);
    size_t offset = 0;
    
    offset += sprintf(asm_code + offset, "; Select statement\n");
    
    // Allocate SelectCase array
    offset += sprintf(asm_code + offset, "    ; Allocate SelectCase array\n");
    offset += sprintf(asm_code + offset, "    mov rdi, %zu\n", 
                     sizeof(SelectCase*) * stmt->case_count);
    offset += sprintf(asm_code + offset, "    call malloc\n");
    offset += sprintf(asm_code + offset, "    mov [rel .select_cases], rax\n");
    
    // Build each case
    for (size_t i = 0; i < stmt->case_count; i++) {
        SelectCase* sc = stmt->cases[i];
        
        offset += sprintf(asm_code + offset, "\n    ; Case %zu\n", i);
        
        if (sc->op_type == SELECT_SEND) {
            // Allocate value
            offset += sprintf(asm_code + offset, "    mov rax, %s\n", 
                             sc->value_expr ? sc->value_expr : "0");
            offset += sprintf(asm_code + offset, "    mov [rel .select_value_%zu], rax\n", i);
            
            // Call select_case_send
            offset += sprintf(asm_code + offset, "    mov rdx, %d\n", sc->case_index);
            offset += sprintf(asm_code + offset, "    lea rsi, [rel .select_value_%zu]\n", i);
            offset += sprintf(asm_code + offset, "    mov rdi, [rel %s]\n", 
                             sc->channel_name);
            offset += sprintf(asm_code + offset, "    call select_case_send\n");
            
        } else if (sc->op_type == SELECT_RECEIVE) {
            // Call select_case_receive
            offset += sprintf(asm_code + offset, "    mov rsi, %d\n", sc->case_index);
            offset += sprintf(asm_code + offset, "    mov rdi, [rel %s]\n", 
                             sc->channel_name);
            offset += sprintf(asm_code + offset, "    call select_case_receive\n");
            
        } else if (sc->op_type == SELECT_DEFAULT) {
            // Call select_case_default
            offset += sprintf(asm_code + offset, "    mov rdi, %d\n", sc->case_index);
            offset += sprintf(asm_code + offset, "    call select_case_default\n");
        }
        
        // Store in array
        offset += sprintf(asm_code + offset, "    mov rbx, [rel .select_cases]\n");
        offset += sprintf(asm_code + offset, "    mov [rbx + %zu], rax\n", i * 8);
    }
    
    // Call channel_select
    offset += sprintf(asm_code + offset, "\n    ; Call channel_select\n");
    offset += sprintf(asm_code + offset, "    mov rdx, %zu\n", stmt->case_count);
    offset += sprintf(asm_code + offset, "    mov rsi, [rel .select_cases]\n");
    offset += sprintf(asm_code + offset, "    mov rdi, [rel channel_context]\n");
    offset += sprintf(asm_code + offset, "    call channel_select\n");
    offset += sprintf(asm_code + offset, "    mov [rel .select_result], rax\n");
    
    // Get selected case
    offset += sprintf(asm_code + offset, "\n    ; Get selected case\n");
    offset += sprintf(asm_code + offset, "    mov rax, [rel .select_result]\n");
    offset += sprintf(asm_code + offset, "    mov eax, [rax]\n");  // selected_case field
    offset += sprintf(asm_code + offset, "    mov [rel .selected_case], eax\n");
    
    // Generate jump table
    offset += sprintf(asm_code + offset, "\n    ; Jump to selected case\n");
    for (size_t i = 0; i < stmt->case_count; i++) {
        offset += sprintf(asm_code + offset, "    cmp eax, %d\n", 
                         stmt->cases[i]->case_index);
        offset += sprintf(asm_code + offset, "    je .select_case_%zu\n", i);
    }
    offset += sprintf(asm_code + offset, "    jmp .select_end\n\n");
    
    // Generate case labels (user code will be inserted here)
    for (size_t i = 0; i < stmt->case_count; i++) {
        offset += sprintf(asm_code + offset, ".select_case_%zu:\n", i);
        
        // For receive cases, extract received data
        if (stmt->cases[i]->op_type == SELECT_RECEIVE && 
            stmt->cases[i]->result_var) {
            offset += sprintf(asm_code + offset, "    mov rax, [rel .select_result]\n");
            offset += sprintf(asm_code + offset, "    mov rax, [rax + 16]\n");  // received_data field
            offset += sprintf(asm_code + offset, "    mov rbx, [rax]\n");
            offset += sprintf(asm_code + offset, "    mov [rel %s], rbx\n", 
                             stmt->cases[i]->result_var);
            offset += sprintf(asm_code + offset, "    mov rdi, rax\n");
            offset += sprintf(asm_code + offset, "    call free\n");
        }
        
        offset += sprintf(asm_code + offset, "    ; [User code for case %zu]\n", i);
        offset += sprintf(asm_code + offset, "    jmp .select_end\n\n");
    }
    
    offset += sprintf(asm_code + offset, ".select_end:\n");
    
    // Free resources
    offset += sprintf(asm_code + offset, "    ; Free select resources\n");
    offset += sprintf(asm_code + offset, "    mov rdi, [rel .select_result]\n");
    offset += sprintf(asm_code + offset, "    call select_result_free\n");
    offset += sprintf(asm_code + offset, "    mov rdi, [rel .select_cases]\n");
    offset += sprintf(asm_code + offset, "    call free\n\n");
    
    // Add data section
    offset += sprintf(asm_code + offset, "section .bss\n");
    offset += sprintf(asm_code + offset, ".select_cases: resq 1\n");
    offset += sprintf(asm_code + offset, ".select_result: resq 1\n");
    offset += sprintf(asm_code + offset, ".selected_case: resd 1\n");
    
    for (size_t i = 0; i < stmt->case_count; i++) {
        if (stmt->cases[i]->op_type == SELECT_SEND) {
            offset += sprintf(asm_code + offset, ".select_value_%zu: resq 1\n", i);
        }
    }
    
    offset += sprintf(asm_code + offset, "section .text\n\n");
    
    return asm_code;
}

// ============================================================================
// MAIN CODEGEN FUNCTION
// ============================================================================

char* codegen_channel_operation(ChannelOp* op, ChannelContext* ctx) {
    if (!op) return NULL;
    
    switch (op->op_type) {
        case CHAN_OP_SEND:
            return codegen_channel_send(op, ctx);
        case CHAN_OP_RECEIVE:
            return codegen_channel_receive(op, ctx);
        case CHAN_OP_CLOSE:
            return codegen_channel_close(op, ctx);
        default:
            return NULL;
    }
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

char* codegen_channel_stats(const char* channel_name) {
    // Generate assembly to print channel statistics
    
    char* asm_code = (char*)malloc(2048);
    size_t offset = 0;
    
    offset += sprintf(asm_code + offset, "; Channel stats: %s\n", channel_name);
    
    // Allocate stack space for stats
    offset += sprintf(asm_code + offset, "    sub rsp, 32\n");
    
    // Call channel_stats
    offset += sprintf(asm_code + offset, "    lea r9, [rsp + 24]\n");  // blocked_recv
    offset += sprintf(asm_code + offset, "    lea r8, [rsp + 16]\n");  // blocked_send
    offset += sprintf(asm_code + offset, "    lea rcx, [rsp + 8]\n");   // receives
    offset += sprintf(asm_code + offset, "    lea rdx, [rsp]\n");       // sends
    offset += sprintf(asm_code + offset, "    mov rdi, [rel %s]\n", channel_name);
    offset += sprintf(asm_code + offset, "    call channel_stats\n");
    
    // Print stats (would need printf calls here)
    offset += sprintf(asm_code + offset, "    ; Print stats...\n");
    
    offset += sprintf(asm_code + offset, "    add rsp, 32\n\n");
    
    return asm_code;
}
