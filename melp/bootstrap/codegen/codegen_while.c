// MELP Modular Codegen - While/Do-While Generation
// While ve do-while döngü kodu üretimi

#include "codegen.h"

// Forward declarations
extern void codegen_generate_comparison(Codegen* gen, Expression* condition, int false_label);
extern void codegen_generate_statement(Codegen* gen, Statement* stmt);

// ============================================================================
// While Loop Code Generation
// ============================================================================

void codegen_generate_while(Codegen* gen, Statement* stmt) {
    char buffer[256];
    int loop_start = gen->label_counter++;
    int loop_end = gen->label_counter++;
    
    // Save previous loop context
    int prev_start = gen->loop_start_label;
    int prev_continue = gen->loop_continue_label;
    int prev_end = gen->loop_end_label;
    int prev_in_loop = gen->in_loop;
    
    // Set current loop context (while: continue = start)
    gen->loop_start_label = loop_start;
    gen->loop_continue_label = loop_start;
    gen->loop_end_label = loop_end;
    gen->in_loop = 1;
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; While loop");
    
    // Loop start label
    snprintf(buffer, sizeof(buffer), ".L%d:  ; while_start", loop_start);
    codegen_emit(gen, buffer);
    
    // Evaluate condition and jump to end if false
    codegen_generate_comparison(gen, stmt->while_stmt.condition, loop_end);
    
    // Loop body
    codegen_emit(gen, "    ; While body");
    for (int i = 0; i < stmt->while_stmt.body_count; i++) {
        codegen_generate_statement(gen, stmt->while_stmt.body[i]);
    }
    
    // Jump back to loop start
    snprintf(buffer, sizeof(buffer), "    jmp .L%d", loop_start);
    codegen_emit(gen, buffer);
    
    // Loop end label
    snprintf(buffer, sizeof(buffer), ".L%d:  ; while_end", loop_end);
    codegen_emit(gen, buffer);
    
    // Restore previous loop context
    gen->loop_start_label = prev_start;
    gen->loop_continue_label = prev_continue;
    gen->loop_end_label = prev_end;
    gen->in_loop = prev_in_loop;
}

// ============================================================================
// Do-While Loop Code Generation
// ============================================================================

void codegen_generate_do_while(Codegen* gen, Statement* stmt) {
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
    codegen_emit(gen, "    ; Do-while loop");
    
    // Loop start label
    snprintf(buffer, sizeof(buffer), ".L%d:  ; do_while_start", loop_start);
    codegen_emit(gen, buffer);
    
    // Loop body
    codegen_emit(gen, "    ; Do-while body");
    for (int i = 0; i < stmt->do_while_stmt.body_count; i++) {
        codegen_generate_statement(gen, stmt->do_while_stmt.body[i]);
    }
    
    // Continue label (for continue statement)
    snprintf(buffer, sizeof(buffer), ".L%d:  ; do_while_continue", loop_continue);
    codegen_emit(gen, buffer);
    
    // Check condition
    if (stmt->do_while_stmt.condition == NULL) {
        fprintf(stderr, "ERROR: do-while condition is NULL!\n");
        exit(1);
    }
    
    codegen_emit(gen, "    ; Check condition");
    codegen_generate_comparison(gen, stmt->do_while_stmt.condition, loop_end);
    
    // If we reach here, condition was TRUE, so loop again
    snprintf(buffer, sizeof(buffer), "    jmp .L%d         ; Condition true, loop", loop_start);
    codegen_emit(gen, buffer);
    
    // Loop end label
    snprintf(buffer, sizeof(buffer), ".L%d:  ; do_while_end", loop_end);
    codegen_emit(gen, buffer);
    
    // Restore previous loop context
    gen->loop_start_label = prev_start;
    gen->loop_continue_label = prev_continue;
    gen->loop_end_label = prev_end;
    gen->in_loop = prev_in_loop;
}
