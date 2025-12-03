// MELP Codegen - Switch Statement Generation
// Include file - DO NOT compile separately

void codegen_generate_switch(Codegen* gen, Statement* stmt) {
    char buffer[256];
    int switch_end = gen->label_counter++;
    
    // Save previous loop context (switch acts like a loop for exit)
    int prev_end = gen->loop_end_label;
    int prev_in_loop = gen->in_loop;
    
    // Set switch context for 'exit' to work
    gen->loop_end_label = switch_end;
    gen->in_loop = 1;
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Switch statement");
    
    // Evaluate switch expression and store in rax
    codegen_generate_expression_value(gen, stmt->switch_stmt.value);
    codegen_emit(gen, "    push rax         ; Save switch value");
    
    // Generate labels for each case
    int* case_labels = malloc(sizeof(int) * stmt->switch_stmt.case_count);
    for (int i = 0; i < stmt->switch_stmt.case_count; i++) {
        case_labels[i] = gen->label_counter++;
    }
    int default_label = (stmt->switch_stmt.default_body != NULL) ? gen->label_counter++ : switch_end;
    
    // Generate comparison jumps for each case
    for (int i = 0; i < stmt->switch_stmt.case_count; i++) {
        SwitchCase* current_case = stmt->switch_stmt.cases[i];
        
        codegen_emit(gen, "    ; Case comparison");
        codegen_emit(gen, "    pop rax          ; Restore switch value");
        codegen_emit(gen, "    push rax         ; Save again for next comparison");
        
        int guard_check_label = -1;
        if (current_case->guard != NULL) {
            guard_check_label = gen->label_counter++;
        }
        
        // Range matching
        if (current_case->range_start != NULL && current_case->range_end != NULL) {
            codegen_emit(gen, "    ; Range match");
            
            codegen_generate_expression_value(gen, current_case->range_start);
            codegen_emit(gen, "    mov rbx, rax");
            codegen_emit(gen, "    pop rax          ; Get switch value");
            codegen_emit(gen, "    push rax         ; Save again");
            codegen_emit(gen, "    cmp rax, rbx");
            int skip_label = gen->label_counter++;
            snprintf(buffer, sizeof(buffer), "    jl .L%d          ; Skip if value < start", skip_label);
            codegen_emit(gen, buffer);
            
            codegen_generate_expression_value(gen, current_case->range_end);
            codegen_emit(gen, "    mov rbx, rax");
            codegen_emit(gen, "    pop rax          ; Get switch value");
            codegen_emit(gen, "    push rax         ; Save again");
            codegen_emit(gen, "    cmp rax, rbx");
            
            if (guard_check_label >= 0) {
                snprintf(buffer, sizeof(buffer), "    jle .L%d         ; Range matched, check guard", guard_check_label);
            } else {
                snprintf(buffer, sizeof(buffer), "    jle .L%d         ; Range matched, execute case", case_labels[i]);
            }
            codegen_emit(gen, buffer);
            
            snprintf(buffer, sizeof(buffer), ".L%d:  ; Skip label", skip_label);
            codegen_emit(gen, buffer);
        } else if (current_case->is_type_check) {
            // Type checking: null
            codegen_emit(gen, "    ; Type check (null)");
            codegen_emit(gen, "    pop rax          ; Get switch value");
            codegen_emit(gen, "    push rax         ; Save again");
            codegen_emit(gen, "    cmp rax, 0");
            
            if (guard_check_label >= 0) {
                snprintf(buffer, sizeof(buffer), "    je .L%d          ; Null matched, check guard", guard_check_label);
            } else {
                snprintf(buffer, sizeof(buffer), "    je .L%d          ; Null matched, execute case", case_labels[i]);
            }
            codegen_emit(gen, buffer);
        } else if (current_case->value != NULL) {
            // Regular value matching
            codegen_generate_expression_value(gen, current_case->value);
            codegen_emit(gen, "    mov rbx, rax");
            codegen_emit(gen, "    pop rax          ; Get switch value");
            codegen_emit(gen, "    push rax         ; Save again");
            
            codegen_emit(gen, "    cmp rax, rbx");
            
            if (guard_check_label >= 0) {
                snprintf(buffer, sizeof(buffer), "    je .L%d          ; Value matched, check guard", guard_check_label);
            } else {
                snprintf(buffer, sizeof(buffer), "    je .L%d          ; Value matched, execute case", case_labels[i]);
            }
            codegen_emit(gen, buffer);
        }
        
        // Guard check
        if (guard_check_label >= 0) {
            snprintf(buffer, sizeof(buffer), ".L%d:  ; Guard check", guard_check_label);
            codegen_emit(gen, buffer);
            
            codegen_emit(gen, "    ; Evaluate guard condition");
            codegen_generate_expression_value(gen, current_case->guard);
            codegen_emit(gen, "    cmp rax, 0");
            snprintf(buffer, sizeof(buffer), "    jne .L%d         ; Guard passed, execute case", case_labels[i]);
            codegen_emit(gen, buffer);
        }
    }
    
    // No match - jump to default or end
    codegen_emit(gen, "    pop rax          ; Clean up switch value");
    snprintf(buffer, sizeof(buffer), "    jmp .L%d         ; Jump to default/end", default_label);
    codegen_emit(gen, buffer);
    
    // Generate case bodies
    for (int i = 0; i < stmt->switch_stmt.case_count; i++) {
        snprintf(buffer, sizeof(buffer), ".L%d:  ; case %d", case_labels[i], i);
        codegen_emit(gen, buffer);
        
        for (int j = 0; j < stmt->switch_stmt.cases[i]->body_count; j++) {
            codegen_generate_statement(gen, stmt->switch_stmt.cases[i]->body[j]);
        }
        
        snprintf(buffer, sizeof(buffer), "    jmp .L%d         ; Fall through to end", switch_end);
        codegen_emit(gen, buffer);
    }
    
    // Default body
    if (stmt->switch_stmt.default_body != NULL) {
        snprintf(buffer, sizeof(buffer), ".L%d:  ; default", default_label);
        codegen_emit(gen, buffer);
        
        for (int i = 0; i < stmt->switch_stmt.default_count; i++) {
            codegen_generate_statement(gen, stmt->switch_stmt.default_body[i]);
        }
    }
    
    // Switch end label
    snprintf(buffer, sizeof(buffer), ".L%d:  ; switch_end", switch_end);
    codegen_emit(gen, buffer);
    
    // Restore previous loop context
    gen->loop_end_label = prev_end;
    gen->in_loop = prev_in_loop;
    
    free(case_labels);
}
