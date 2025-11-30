// MELP Modular Codegen - For Loop Generation
// For ve for-in döngü kodu üretimi

#include "codegen.h"

// Forward declarations
extern void codegen_generate_expression_value(Codegen* gen, Expression* expr);
extern void codegen_generate_statement(Codegen* gen, Statement* stmt);

// ============================================================================
// For Loop Code Generation
// ============================================================================

void codegen_generate_for(Codegen* gen, Statement* stmt) {
    char buffer[256];
    int loop_start = gen->label_counter++;
    int loop_continue = gen->label_counter++;
    int loop_end = gen->label_counter++;
    
    // Save previous loop context
    int prev_start = gen->loop_start_label;
    int prev_continue = gen->loop_continue_label;
    int prev_end = gen->loop_end_label;
    int prev_in_loop = gen->in_loop;
    
    // Set current loop context
    gen->loop_start_label = loop_start;
    gen->loop_continue_label = loop_continue;
    gen->loop_end_label = loop_end;
    gen->in_loop = 1;
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; For loop");
    
    // Allocate loop variable on stack
    gen->stack_offset += 8;
    codegen_add_variable(gen, stmt->for_stmt.var_name, gen->stack_offset, TYPE_NUMERIC, NULL);
    snprintf(buffer, sizeof(buffer), "    sub rsp, 8         ; Allocate %s", 
             stmt->for_stmt.var_name);
    codegen_emit(gen, buffer);
    
    // Initialize loop variable with start value
    codegen_generate_expression_value(gen, stmt->for_stmt.start);
    int var_offset = codegen_find_variable(gen, stmt->for_stmt.var_name);
    snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Initialize %s",
             var_offset, stmt->for_stmt.var_name);
    codegen_emit(gen, buffer);
    
    // Loop start label
    snprintf(buffer, sizeof(buffer), ".L%d:  ; loop_start", loop_start);
    codegen_emit(gen, buffer);
    
    // Check condition: loop_var <= end_value
    snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]   ; Load %s",
             var_offset, stmt->for_stmt.var_name);
    codegen_emit(gen, buffer);
    codegen_emit(gen, "    push rax");
    
    codegen_generate_expression_value(gen, stmt->for_stmt.end);
    codegen_emit(gen, "    mov rbx, rax");
    codegen_emit(gen, "    pop rax");
    codegen_emit(gen, "    cmp rax, rbx");
    
    snprintf(buffer, sizeof(buffer), "    jg .L%d           ; Exit if i > end", loop_end);
    codegen_emit(gen, buffer);
    
    // Loop body
    codegen_emit(gen, "    ; Loop body");
    for (int i = 0; i < stmt->for_stmt.body_count; i++) {
        codegen_generate_statement(gen, stmt->for_stmt.body[i]);
    }
    
    // Loop continue label (for continue statement)
    snprintf(buffer, sizeof(buffer), ".L%d:  ; loop_continue", loop_continue);
    codegen_emit(gen, buffer);
    
    // Increment loop variable
    codegen_emit(gen, "    ; Increment loop variable");
    snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]", var_offset);
    codegen_emit(gen, buffer);
    
    if (stmt->for_stmt.step) {
        // Custom step value
        codegen_emit(gen, "    push rax");
        codegen_generate_expression_value(gen, stmt->for_stmt.step);
        codegen_emit(gen, "    mov rbx, rax");
        codegen_emit(gen, "    pop rax");
        codegen_emit(gen, "    add rax, rbx");
    } else {
        // Default step = 1
        codegen_emit(gen, "    add rax, 1");
    }
    
    snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax", var_offset);
    codegen_emit(gen, buffer);
    
    // Jump back to loop start
    snprintf(buffer, sizeof(buffer), "    jmp .L%d", loop_start);
    codegen_emit(gen, buffer);
    
    // Loop end label
    snprintf(buffer, sizeof(buffer), ".L%d:  ; loop_end", loop_end);
    codegen_emit(gen, buffer);
    
    // Restore previous loop context
    gen->loop_start_label = prev_start;
    gen->loop_continue_label = prev_continue;
    gen->loop_end_label = prev_end;
    gen->in_loop = prev_in_loop;
}

// ============================================================================
// For-In Loop Code Generation (Array Iteration)
// ============================================================================

void codegen_generate_for_in(Codegen* gen, Statement* stmt) {
    char buffer[256];
    int loop_start = gen->label_counter++;
    int loop_end = gen->label_counter++;
    int loop_continue = gen->label_counter++;
    
    // Save previous loop context
    int prev_start = gen->loop_start_label;
    int prev_continue = gen->loop_continue_label;
    int prev_end = gen->loop_end_label;
    int prev_in_loop = gen->in_loop;
    
    gen->loop_start_label = loop_start;
    gen->loop_continue_label = loop_continue;
    gen->loop_end_label = loop_end;
    gen->in_loop = 1;
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; For-in loop");
    
    // Allocate space for loop variable and index
    gen->stack_offset += 8;  // Loop variable
    int var_offset = gen->stack_offset;
    codegen_add_variable(gen, stmt->for_in.var_name, var_offset, stmt->for_in.var_type, NULL);
    snprintf(buffer, sizeof(buffer), "    sub rsp, 8         ; Allocate space for %s", stmt->for_in.var_name);
    codegen_emit(gen, buffer);
    
    gen->stack_offset += 8;  // Index variable
    int index_offset = gen->stack_offset;
    snprintf(buffer, sizeof(buffer), "    sub rsp, 8         ; Allocate space for loop index");
    codegen_emit(gen, buffer);
    
    gen->stack_offset += 8;  // Array base address
    int arr_offset = gen->stack_offset;
    snprintf(buffer, sizeof(buffer), "    sub rsp, 8         ; Allocate space for array base");
    codegen_emit(gen, buffer);
    
    gen->stack_offset += 8;  // Array length
    int len_offset = gen->stack_offset;
    snprintf(buffer, sizeof(buffer), "    sub rsp, 8         ; Allocate space for array length");
    codegen_emit(gen, buffer);
    
    // Setup array iteration
    codegen_setup_for_in_iterable(gen, stmt, arr_offset, len_offset);
    
    // Initialize index to 0
    snprintf(buffer, sizeof(buffer), "    mov qword [rbp-%d], 0   ; Initialize index", index_offset);
    codegen_emit(gen, buffer);
    
    // Loop start
    snprintf(buffer, sizeof(buffer), ".L%d:  ; for_in_start", loop_start);
    codegen_emit(gen, buffer);
    
    // Check if index < length
    snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]", index_offset);
    codegen_emit(gen, buffer);
    snprintf(buffer, sizeof(buffer), "    cmp rax, [rbp-%d]", len_offset);
    codegen_emit(gen, buffer);
    snprintf(buffer, sizeof(buffer), "    jge .L%d", loop_end);
    codegen_emit(gen, buffer);
    
    // Load current element: arr[index]
    snprintf(buffer, sizeof(buffer), "    mov rdi, [rbp-%d]   ; Array base", arr_offset);
    codegen_emit(gen, buffer);
    snprintf(buffer, sizeof(buffer), "    mov rsi, [rbp-%d]   ; Index", index_offset);
    codegen_emit(gen, buffer);
    codegen_emit(gen, "    mov rax, [rdi + rsi*8]   ; Load element");
    snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store in loop variable", var_offset);
    codegen_emit(gen, buffer);
    
    // Generate loop body
    for (int i = 0; i < stmt->for_in.body_count; i++) {
        codegen_generate_statement(gen, stmt->for_in.body[i]);
    }
    
    // Continue label (for continue statement)
    snprintf(buffer, sizeof(buffer), ".L%d:  ; for_in_continue", loop_continue);
    codegen_emit(gen, buffer);
    
    // Increment index
    snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]", index_offset);
    codegen_emit(gen, buffer);
    codegen_emit(gen, "    add rax, 1");
    snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax", index_offset);
    codegen_emit(gen, buffer);
    
    // Jump back to loop start
    snprintf(buffer, sizeof(buffer), "    jmp .L%d", loop_start);
    codegen_emit(gen, buffer);
    
    // Loop end label
    snprintf(buffer, sizeof(buffer), ".L%d:  ; for_in_end", loop_end);
    codegen_emit(gen, buffer);
    
    // Restore previous loop context
    gen->loop_start_label = prev_start;
    gen->loop_continue_label = prev_continue;
    gen->loop_end_label = prev_end;
    gen->in_loop = prev_in_loop;
}

// ============================================================================
// For-In Iterable Setup Helper
// ============================================================================

void codegen_setup_for_in_iterable(Codegen* gen, Statement* stmt, int arr_offset, int len_offset) {
    char buffer[256];
    
    if (stmt->for_in.iterable->type == EXPR_VARIABLE) {
        VarInfo* var = gen->variables;
        int found_offset = 0;
        int is_dynamic = 0;
        int is_array = 0;
        int array_size = 0;
        
        while (var) {
            if (strcmp(var->name, stmt->for_in.iterable->var_name) == 0) {
                found_offset = var->stack_offset;
                is_dynamic = var->is_dynamic_array;
                is_array = var->is_array;
                array_size = var->array_size;
                break;
            }
            var = var->next;
        }
        
        if (is_dynamic) {
            snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]   ; Load dynamic array pointer", found_offset);
            codegen_emit(gen, buffer);
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store array pointer", arr_offset);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    mov rdi, rax");
            codegen_emit(gen, "    call mlp_array_length");
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store array length", len_offset);
            codegen_emit(gen, buffer);
        } else if (is_array && array_size > 0) {
            snprintf(buffer, sizeof(buffer), "    lea rax, [rbp-%d]   ; Get address of stack array", found_offset);
            codegen_emit(gen, buffer);
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store array base address", arr_offset);
            codegen_emit(gen, buffer);
            snprintf(buffer, sizeof(buffer), "    mov rax, %d   ; Array length (from declaration)", array_size);
            codegen_emit(gen, buffer);
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store array length", len_offset);
            codegen_emit(gen, buffer);
        } else {
            snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]   ; Load potential array pointer", found_offset);
            codegen_emit(gen, buffer);
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store array pointer", arr_offset);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    mov rdi, rax");
            codegen_emit(gen, "    call mlp_array_length");
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store array length", len_offset);
            codegen_emit(gen, buffer);
        }
    } else {
        codegen_generate_expression_value(gen, stmt->for_in.iterable);
        snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store array pointer", arr_offset);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    mov rdi, rax");
        codegen_emit(gen, "    call mlp_array_length");
        snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store array length", len_offset);
        codegen_emit(gen, buffer);
    }
}
