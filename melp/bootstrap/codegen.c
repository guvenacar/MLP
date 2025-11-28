// MELP Phase 1 - Bootstrap Compiler  
// Codegen: x86-64 Assembly üretir (declarations + assignments + print)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.c"

typedef struct VarInfo {
    char* name;
    int stack_offset;
    struct VarInfo* next;
} VarInfo;

typedef struct {
    FILE* out;
    int stack_offset;
    VarInfo* variables;  // Symbol table
    int label_counter;   // For unique labels
    int loop_start_label;    // Current loop start (condition check)
    int loop_continue_label; // Current loop continue (increment/next iteration)
    int loop_end_label;      // Current loop end for exit
    int in_loop;             // Are we inside a loop?
} Codegen;

Codegen* codegen_create(const char* output_file) {
    Codegen* gen = malloc(sizeof(Codegen));
    if (!gen) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    
    gen->out = fopen(output_file, "w");
    if (!gen->out) {
        fprintf(stderr, "Error: Cannot open output file '%s'\n", output_file);
        perror("fopen");
        exit(1);
    }
    
    gen->stack_offset = 0;
    gen->variables = NULL;
    gen->label_counter = 0;
    gen->loop_start_label = -1;
    gen->loop_continue_label = -1;
    gen->loop_end_label = -1;
    gen->in_loop = 0;
    return gen;
}

void codegen_add_variable(Codegen* gen, const char* name, int offset) {
    VarInfo* var = malloc(sizeof(VarInfo));
    var->name = malloc(strlen(name) + 1);
    strcpy(var->name, name);
    var->stack_offset = offset;
    var->next = gen->variables;
    gen->variables = var;
}

int codegen_find_variable(Codegen* gen, const char* name) {
    VarInfo* current = gen->variables;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current->stack_offset;
        }
        current = current->next;
    }
    fprintf(stderr, "Codegen error: Undefined variable '%s'\n", name);
    exit(1);
}

void codegen_emit(Codegen* gen, const char* line) {
    fprintf(gen->out, "%s\n", line);
}

// Forward declaration
void codegen_generate_statement(Codegen* gen, Statement* stmt);
void codegen_generate_expression_value(Codegen* gen, Expression* expr);

void codegen_emit_prologue(Codegen* gen) {
    codegen_emit(gen, "section .text");
    codegen_emit(gen, "extern print_number");
    codegen_emit(gen, "global _start");
    codegen_emit(gen, "");
    codegen_emit(gen, "_start:");
    codegen_emit(gen, "    push rbp");
    codegen_emit(gen, "    mov rbp, rsp");
}

void codegen_emit_epilogue(Codegen* gen) {
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Exit program");
    codegen_emit(gen, "    mov rax, 60        ; sys_exit");
    codegen_emit(gen, "    xor rdi, rdi       ; exit code 0");
    codegen_emit(gen, "    syscall");
}

void codegen_generate_declaration(Codegen* gen, Declaration* decl) {
    // Stack'te yer ayır (her değişken 8 byte)
    gen->stack_offset += 8;
    codegen_add_variable(gen, decl->name, gen->stack_offset);
    
    char buffer[256];
    snprintf(buffer, sizeof(buffer), 
             "    ; Declaration: %s %s", 
             decl->type == TYPE_NUMERIC ? "numeric" :
             decl->type == TYPE_DECIMAL ? "decimal" : "boolean",
             decl->name);
    codegen_emit(gen, buffer);
    
    snprintf(buffer, sizeof(buffer),
             "    sub rsp, 8         ; Allocate space for %s",
             decl->name);
    codegen_emit(gen, buffer);
    
    // Initialize if value provided
    if (decl->init_value) {
        if (decl->init_value->type == EXPR_NUMBER) {
            snprintf(buffer, sizeof(buffer),
                     "    mov qword [rbp-%d], %ld   ; Initialize %s",
                     gen->stack_offset, decl->init_value->number_value, decl->name);
            codegen_emit(gen, buffer);
        } else if (decl->init_value->type == EXPR_VARIABLE) {
            int src_offset = codegen_find_variable(gen, decl->init_value->var_name);
            snprintf(buffer, sizeof(buffer),
                     "    mov rax, [rbp-%d]   ; Load %s",
                     src_offset, decl->init_value->var_name);
            codegen_emit(gen, buffer);
            snprintf(buffer, sizeof(buffer),
                     "    mov [rbp-%d], rax   ; Store to %s",
                     gen->stack_offset, decl->name);
            codegen_emit(gen, buffer);
        }
    }
}

void codegen_generate_assignment(Codegen* gen, char* var_name, Expression* value) {
    int offset = codegen_find_variable(gen, var_name);
    char buffer[256];
    
    codegen_emit(gen, "");
    snprintf(buffer, sizeof(buffer), "    ; Assignment: %s = ...", var_name);
    codegen_emit(gen, buffer);
    
    // Evaluate expression and store result
    codegen_generate_expression_value(gen, value);
    snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store to %s",
             offset, var_name);
    codegen_emit(gen, buffer);
}

void codegen_generate_print(Codegen* gen, Expression* expr) {
    char buffer[256];
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Print statement");
    
    // Load value into rdi (first argument for print)
    if (expr->type == EXPR_NUMBER) {
        snprintf(buffer, sizeof(buffer),
                 "    mov rdi, %ld       ; Immediate value",
                 expr->number_value);
        codegen_emit(gen, buffer);
    } else if (expr->type == EXPR_VARIABLE) {
        int offset = codegen_find_variable(gen, expr->var_name);
        snprintf(buffer, sizeof(buffer),
                 "    mov rdi, [rbp-%d]   ; Load %s",
                 offset, expr->var_name);
        codegen_emit(gen, buffer);
    }
    
    // Call print_number from runtime
    codegen_emit(gen, "    call print_number");
}

void codegen_generate_expression_value(Codegen* gen, Expression* expr) {
    char buffer[256];
    
    if (expr->type == EXPR_NUMBER) {
        snprintf(buffer, sizeof(buffer), "    mov rax, %ld", expr->number_value);
        codegen_emit(gen, buffer);
    } else if (expr->type == EXPR_VARIABLE) {
        int offset = codegen_find_variable(gen, expr->var_name);
        snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]", offset);
        codegen_emit(gen, buffer);
    } else if (expr->type == EXPR_BINARY_OP) {
        // Evaluate left side
        codegen_generate_expression_value(gen, expr->binary_op.left);
        codegen_emit(gen, "    push rax");
        
        // Evaluate right side
        codegen_generate_expression_value(gen, expr->binary_op.right);
        codegen_emit(gen, "    mov rbx, rax");
        codegen_emit(gen, "    pop rax");
        
        // Perform operation
        switch (expr->binary_op.op) {
            case BIN_OP_ADD:
                codegen_emit(gen, "    add rax, rbx");
                break;
            case BIN_OP_SUB:
                codegen_emit(gen, "    sub rax, rbx");
                break;
            case BIN_OP_MUL:
                codegen_emit(gen, "    imul rax, rbx");
                break;
            case BIN_OP_DIV:
                codegen_emit(gen, "    cqo");  // Sign extend rax to rdx:rax
                codegen_emit(gen, "    idiv rbx");
                break;
        }
    }
}

void codegen_generate_comparison(Codegen* gen, Expression* condition, int false_label) {
    char buffer[256];
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Evaluate comparison");
    
    // Load left side into rax
    codegen_generate_expression_value(gen, condition->comparison.left);
    codegen_emit(gen, "    push rax");
    
    // Load right side into rbx
    codegen_generate_expression_value(gen, condition->comparison.right);
    codegen_emit(gen, "    mov rbx, rax");
    codegen_emit(gen, "    pop rax");
    
    // Compare
    codegen_emit(gen, "    cmp rax, rbx");
    
    // Jump to false label based on comparison
    const char* jump_instr;
    switch (condition->comparison.op) {
        case CMP_EQUAL:         jump_instr = "jne"; break;
        case CMP_NOT_EQUAL:     jump_instr = "je"; break;
        case CMP_LESS:          jump_instr = "jge"; break;
        case CMP_LESS_EQUAL:    jump_instr = "jg"; break;
        case CMP_GREATER:       jump_instr = "jle"; break;
        case CMP_GREATER_EQUAL: jump_instr = "jl"; break;
        default: jump_instr = "jmp"; break;
    }
    
    snprintf(buffer, sizeof(buffer), "    %s .L%d", jump_instr, false_label);
    codegen_emit(gen, buffer);
}

void codegen_generate_if(Codegen* gen, Statement* stmt) {
    char buffer[256];
    int else_label = gen->label_counter++;
    int end_label = gen->label_counter++;
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; If statement");
    
    // Generate condition check
    if (stmt->if_stmt.condition->type == EXPR_COMPARISON) {
        codegen_generate_comparison(gen, stmt->if_stmt.condition, else_label);
    }
    
    // Then body
    codegen_emit(gen, "    ; Then body");
    for (int i = 0; i < stmt->if_stmt.then_count; i++) {
        codegen_generate_statement(gen, stmt->if_stmt.then_body[i]);
    }
    
    if (stmt->if_stmt.else_count > 0) {
        snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
        codegen_emit(gen, buffer);
    }
    
    // Else label
    snprintf(buffer, sizeof(buffer), ".L%d:", else_label);
    codegen_emit(gen, buffer);
    
    // Else body (if exists)
    if (stmt->if_stmt.else_count > 0) {
        codegen_emit(gen, "    ; Else body");
        for (int i = 0; i < stmt->if_stmt.else_count; i++) {
            codegen_generate_statement(gen, stmt->if_stmt.else_body[i]);
        }
        
        snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
        codegen_emit(gen, buffer);
    }
}

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
    codegen_add_variable(gen, stmt->for_stmt.var_name, gen->stack_offset);
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

void codegen_generate_statement(Codegen* gen, Statement* stmt) {
    if (stmt->type == STMT_DECLARATION) {
        codegen_generate_declaration(gen, stmt->declaration);
    } else if (stmt->type == STMT_ASSIGNMENT) {
        codegen_generate_assignment(gen, stmt->assignment.var_name, stmt->assignment.value);
    } else if (stmt->type == STMT_PRINT) {
        codegen_generate_print(gen, stmt->print_stmt.expr);
    } else if (stmt->type == STMT_IF) {
        codegen_generate_if(gen, stmt);
    } else if (stmt->type == STMT_FOR) {
        codegen_generate_for(gen, stmt);
    } else if (stmt->type == STMT_WHILE) {
        codegen_generate_while(gen, stmt);
    } else if (stmt->type == STMT_EXIT) {
        if (!gen->in_loop) {
            fprintf(stderr, "Codegen error: 'exit' outside loop\n");
            exit(1);
        }
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "    jmp .L%d           ; exit", gen->loop_end_label);
        codegen_emit(gen, buffer);
    } else if (stmt->type == STMT_CONTINUE) {
        if (!gen->in_loop) {
            fprintf(stderr, "Codegen error: 'continue' outside loop\n");
            exit(1);
        }
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "    jmp .L%d           ; continue", gen->loop_continue_label);
        codegen_emit(gen, buffer);
    }
}

void codegen_generate(Codegen* gen, AST* ast) {
    codegen_emit_prologue(gen);
    codegen_emit(gen, "");
    
    for (int i = 0; i < ast->count; i++) {
        codegen_generate_statement(gen, ast->statements[i]);
    }
    
    codegen_emit_epilogue(gen);
}

void codegen_free(Codegen* gen) {
    fclose(gen->out);
    free(gen);
}
