/**
 * MLP Codegen - Statement Code Generation (Dispatcher)
 * 
 * Main statement dispatcher that routes to specific handlers:
 * - Struct/Interface/Enum definitions
 * - Function definitions
 * - Control flow (if, for, while, switch)
 * - Assignments, declarations
 * - Try/catch, throw
 * - Module definitions
 * - And more...
 */

#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations for statement handlers
void codegen_generate_func_def(Codegen* gen, Statement* stmt);
void codegen_generate_return(Codegen* gen, Statement* stmt);
void codegen_generate_declaration(Codegen* gen, Declaration* decl);
void codegen_generate_assignment(Codegen* gen, const char* var_name, const char* field_name, 
                                  Expression* array_index, Expression* deref, Expression* value);
void codegen_generate_print(Codegen* gen, Expression* expr);
void codegen_generate_if(Codegen* gen, Statement* stmt);
void codegen_generate_for(Codegen* gen, Statement* stmt);
void codegen_generate_for_in(Codegen* gen, Statement* stmt);
void codegen_generate_while(Codegen* gen, Statement* stmt);
void codegen_generate_do_while(Codegen* gen, Statement* stmt);
void codegen_generate_switch(Codegen* gen, Statement* stmt);
void codegen_generate_expression_value(Codegen* gen, Expression* expr);
void codegen_generate_comparison(Codegen* gen, Expression* condition, int false_label);
int codegen_add_string(Codegen* gen, const char* str);

void codegen_generate_statement(Codegen* gen, Statement* stmt) {
    char buffer[512];
    
    if (stmt->type == STMT_STRUCT_DEF) {
        VarType* field_types = malloc(sizeof(VarType) * stmt->struct_def.field_count);
        char** field_names = malloc(sizeof(char*) * stmt->struct_def.field_count);
        for (int i = 0; i < stmt->struct_def.field_count; i++) {
            field_types[i] = stmt->struct_def.fields[i].type;
            field_names[i] = stmt->struct_def.fields[i].name;
        }
        codegen_add_struct(gen, stmt->struct_def.struct_name, 
                          stmt->struct_def.field_count, field_types, field_names,
                          stmt->struct_def.implements, stmt->struct_def.implements_count);
        free(field_types);
        free(field_names);
        
    } else if (stmt->type == STMT_INTERFACE_DEF) {
        codegen_add_interface(gen, stmt->interface_def.interface_name,
                             stmt->interface_def.methods, stmt->interface_def.method_count);
                             
    } else if (stmt->type == STMT_ENUM_DEF) {
        for (int i = 0; i < stmt->enum_def.member_count; i++) {
            codegen_add_enum_value(gen, stmt->enum_def.enum_name,
                                  stmt->enum_def.members[i].name,
                                  stmt->enum_def.members[i].value);
        }
        
    } else if (stmt->type == STMT_TYPE_ALIAS) {
        codegen_add_type_alias(gen, stmt->type_alias.alias_name,
                              stmt->type_alias.base_type,
                              stmt->type_alias.struct_name);
                              
    } else if (stmt->type == STMT_FUNC_DEF) {
        codegen_generate_func_def(gen, stmt);
        
    } else if (stmt->type == STMT_RETURN) {
        codegen_generate_return(gen, stmt);
        
    } else if (stmt->type == STMT_DECLARATION) {
        codegen_generate_declaration(gen, stmt->declaration);
        
    } else if (stmt->type == STMT_MULTI_DECLARATION) {
        for (int i = 0; i < stmt->multi_decl.count; i++) {
            codegen_generate_declaration(gen, stmt->multi_decl.declarations[i]);
        }
        
    } else if (stmt->type == STMT_ASSIGNMENT) {
        codegen_generate_assignment(gen, stmt->assignment.var_name, stmt->assignment.field_name,
                                   stmt->assignment.array_index, stmt->assignment.dereference_target,
                                   stmt->assignment.value);
                                   
    } else if (stmt->type == STMT_MULTI_ASSIGNMENT) {
        if (!stmt->multi_assignment.func_call) {
            fprintf(stderr, "Codegen error: NULL function call in multi-assignment\n");
            exit(1);
        }
        codegen_generate_expression_value(gen, stmt->multi_assignment.func_call);
        
        if (stmt->multi_assignment.var_count >= 1) {
            int offset = codegen_find_variable(gen, stmt->multi_assignment.var_names[0]);
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax", offset);
            codegen_emit(gen, buffer);
        }
        if (stmt->multi_assignment.var_count >= 2) {
            int offset = codegen_find_variable(gen, stmt->multi_assignment.var_names[1]);
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rdx", offset);
            codegen_emit(gen, buffer);
        }
        
    } else if (stmt->type == STMT_PRINT) {
        codegen_generate_print(gen, stmt->print_stmt.expr);
        
    } else if (stmt->type == STMT_IF) {
        codegen_generate_if(gen, stmt);
        
    } else if (stmt->type == STMT_FOR) {
        codegen_generate_for(gen, stmt);
        
    } else if (stmt->type == STMT_FOR_IN) {
        codegen_generate_for_in(gen, stmt);
        
    } else if (stmt->type == STMT_WHILE) {
        codegen_generate_while(gen, stmt);
        
    } else if (stmt->type == STMT_DO_WHILE) {
        codegen_generate_do_while(gen, stmt);
        
    } else if (stmt->type == STMT_SWITCH) {
        codegen_generate_switch(gen, stmt);
        
    } else if (stmt->type == STMT_YIELD) {
        codegen_emit(gen, "    ; Yield statement");
        codegen_generate_expression_value(gen, stmt->yield_stmt.value);
        codegen_emit(gen, "    push rax");
        int arr_offset = codegen_find_variable(gen, "__gen_array__");
        int idx_offset = codegen_find_variable(gen, "__gen_index__");
        snprintf(buffer, sizeof(buffer), "    mov rdi, [rbp-%d]", arr_offset);
        codegen_emit(gen, buffer);
        snprintf(buffer, sizeof(buffer), "    mov rsi, [rbp-%d]", idx_offset);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    pop rax");
        codegen_emit(gen, "    mov [rdi + rsi*8], rax");
        snprintf(buffer, sizeof(buffer), "    inc qword [rbp-%d]", idx_offset);
        codegen_emit(gen, buffer);
        
    } else if (stmt->type == STMT_TRY_CATCH) {
        codegen_generate_try_catch(gen, stmt);
        
    } else if (stmt->type == STMT_THROW) {
        codegen_generate_throw(gen, stmt);
        
    } else if (stmt->type == STMT_EXIT) {
        if (!gen->in_loop) {
            fprintf(stderr, "Codegen error: 'exit' outside loop\n");
            exit(1);
        }
        snprintf(buffer, sizeof(buffer), "    jmp .L%d", gen->loop_end_label);
        codegen_emit(gen, buffer);
        
    } else if (stmt->type == STMT_CONTINUE) {
        if (!gen->in_loop) {
            fprintf(stderr, "Codegen error: 'continue' outside loop\n");
            exit(1);
        }
        snprintf(buffer, sizeof(buffer), "    jmp .L%d", gen->loop_continue_label);
        codegen_emit(gen, buffer);
        
    } else if (stmt->type == STMT_EXPR_STMT) {
        codegen_generate_expression_value(gen, stmt->print_stmt.expr);
        
    } else if (stmt->type == STMT_MODULE_DEF) {
        codegen_generate_module_def(gen, stmt);
        
    } else if (stmt->type == STMT_OPERATOR_DEF) {
        codegen_generate_operator_def(gen, stmt);
        
    } else if (stmt->type == STMT_MATCH) {
        codegen_generate_match(gen, stmt);
        
    } else if (stmt->type == STMT_STATE_DECL) {
        codegen_generate_state_decl(gen, stmt);
        
    } else if (stmt->type == STMT_DEBUG_LABEL) {
        snprintf(buffer, sizeof(buffer), ".debug_%s:", stmt->debug_label.label_name);
        codegen_emit(gen, buffer);
        
    } else if (stmt->type == STMT_DEBUG_GOTO) {
        snprintf(buffer, sizeof(buffer), "    jmp .debug_%s", stmt->debug_goto.target_label);
        codegen_emit(gen, buffer);
        
    } else if (stmt->type == STMT_DEBUG_IF) {
        int end_label = gen->label_counter++;
        if (stmt->debug_if.condition->type == EXPR_COMPARISON) {
            codegen_generate_comparison(gen, stmt->debug_if.condition, end_label);
        } else {
            codegen_generate_expression_value(gen, stmt->debug_if.condition);
            codegen_emit(gen, "    cmp rax, 0");
            snprintf(buffer, sizeof(buffer), "    je .L%d", end_label);
            codegen_emit(gen, buffer);
        }
        for (int i = 0; i < stmt->debug_if.body_count; i++) {
            codegen_generate_statement(gen, stmt->debug_if.body[i]);
        }
        snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
        codegen_emit(gen, buffer);
        
    } else if (stmt->type == STMT_DEBUG_PAUSE) {
        codegen_emit(gen, "    int3");
        
    } else if (stmt->type == STMT_IMPORT) {
        snprintf(buffer, sizeof(buffer), "; Import: %s", stmt->import_stmt.module_name);
        codegen_emit(gen, buffer);
        
    } else if (stmt->type == STMT_DESTRUCTURE) {
        codegen_generate_destructure(gen, stmt);
    }
}

// Helper for try-catch
void codegen_generate_try_catch(Codegen* gen, Statement* stmt) {
    char buffer[256];
    int try_label = gen->label_counter++;
    int end_label = gen->label_counter++;
    
    codegen_emit(gen, "    ; Try-catch block");
    codegen_emit(gen, "    call mlp_exception_push");
    codegen_emit(gen, "    mov r15, rax");
    codegen_emit(gen, "    mov rdi, r15");
    codegen_emit(gen, "    call setjmp");
    codegen_emit(gen, "    test rax, rax");
    snprintf(buffer, sizeof(buffer), "    jnz .L%d_catch", try_label);
    codegen_emit(gen, buffer);
    
    // Try body
    for (int i = 0; i < stmt->try_catch.try_count; i++) {
        codegen_generate_statement(gen, stmt->try_catch.try_body[i]);
    }
    codegen_emit(gen, "    call mlp_exception_pop");
    snprintf(buffer, sizeof(buffer), "    jmp .L%d_end", end_label);
    codegen_emit(gen, buffer);
    
    // Catch
    snprintf(buffer, sizeof(buffer), ".L%d_catch:", try_label);
    codegen_emit(gen, buffer);
    for (int c = 0; c < stmt->try_catch.catch_count; c++) {
        for (int i = 0; i < stmt->try_catch.catch_blocks[c].body_count; i++) {
            codegen_generate_statement(gen, stmt->try_catch.catch_blocks[c].body[i]);
        }
    }
    codegen_emit(gen, "    call mlp_exception_pop");
    
    snprintf(buffer, sizeof(buffer), ".L%d_end:", end_label);
    codegen_emit(gen, buffer);
}

// Helper for throw
void codegen_generate_throw(Codegen* gen, Statement* stmt) {
    char buffer[256];
    codegen_emit(gen, "    ; Throw exception");
    
    if (stmt->throw_stmt.error_type) {
        int str_id = codegen_add_string(gen, stmt->throw_stmt.error_type);
        snprintf(buffer, sizeof(buffer), "    mov rdi, str_%d", str_id);
    } else {
        int str_id = codegen_add_string(gen, "Error");
        snprintf(buffer, sizeof(buffer), "    mov rdi, str_%d", str_id);
    }
    codegen_emit(gen, buffer);
    
    if (stmt->throw_stmt.error_message) {
        codegen_emit(gen, "    push rdi");
        codegen_generate_expression_value(gen, stmt->throw_stmt.error_message);
        codegen_emit(gen, "    mov rsi, rax");
        codegen_emit(gen, "    pop rdi");
    } else {
        int str_id = codegen_add_string(gen, "Unknown error");
        snprintf(buffer, sizeof(buffer), "    mov rsi, str_%d", str_id);
        codegen_emit(gen, buffer);
    }
    
    codegen_emit(gen, "    mov rdx, 1");
    codegen_emit(gen, "    call mlp_throw");
}

// Helper for module definition
void codegen_generate_module_def(Codegen* gen, Statement* stmt) {
    char buffer[256];
    
    if (codegen_module_seen(gen, stmt->module_def.module_name)) {
        snprintf(buffer, sizeof(buffer), "; Skipping duplicate module: %s", stmt->module_def.module_name);
        codegen_emit(gen, buffer);
        return;
    }
    
    codegen_mark_module_seen(gen, stmt->module_def.module_name);
    snprintf(buffer, sizeof(buffer), "; Module: %s", stmt->module_def.module_name);
    codegen_emit(gen, buffer);
    
    gen->current_module = stmt->module_def.module_name;
    for (int i = 0; i < stmt->module_def.body_count; i++) {
        codegen_generate_statement(gen, stmt->module_def.body[i]);
    }
    gen->current_module = NULL;
}

// Helper for operator definition
void codegen_generate_operator_def(Codegen* gen, Statement* stmt) {
    char buffer[256];
    char func_name[256];
    
    const char* op_name;
    if (strcmp(stmt->operator_def.op, "+") == 0) op_name = "add";
    else if (strcmp(stmt->operator_def.op, "-") == 0) op_name = "sub";
    else if (strcmp(stmt->operator_def.op, "*") == 0) op_name = "mul";
    else if (strcmp(stmt->operator_def.op, "/") == 0) op_name = "div";
    else op_name = "op";
    
    snprintf(func_name, sizeof(func_name), "__op_%s_%s", stmt->operator_def.type_name, op_name);
    codegen_add_operator_overload(gen, stmt->operator_def.op, stmt->operator_def.type_name, func_name);
    
    snprintf(buffer, sizeof(buffer), "%s:", func_name);
    codegen_emit(gen, buffer);
    codegen_emit(gen, "    push rbp");
    codegen_emit(gen, "    mov rbp, rsp");
    
    // Body
    for (int i = 0; i < stmt->operator_def.body_count; i++) {
        codegen_generate_statement(gen, stmt->operator_def.body[i]);
    }
    
    codegen_emit(gen, "    mov rsp, rbp");
    codegen_emit(gen, "    pop rbp");
    codegen_emit(gen, "    ret");
}

// Helper for match statement
void codegen_generate_match(Codegen* gen, Statement* stmt) {
    char buffer[256];
    int end_label = gen->label_counter++;
    
    codegen_emit(gen, "    ; Pattern match");
    codegen_generate_expression_value(gen, stmt->match_stmt.value);
    codegen_emit(gen, "    push rax");
    
    for (int i = 0; i < stmt->match_stmt.case_count; i++) {
        int next_case = gen->label_counter++;
        
        if (!stmt->match_stmt.cases[i].is_wildcard) {
            codegen_generate_expression_value(gen, stmt->match_stmt.cases[i].patterns[0]);
            codegen_emit(gen, "    mov rbx, rax");
            codegen_emit(gen, "    mov rax, [rsp]");
            codegen_emit(gen, "    cmp rax, rbx");
            snprintf(buffer, sizeof(buffer), "    jne .L%d", next_case);
            codegen_emit(gen, buffer);
        }
        
        for (int b = 0; b < stmt->match_stmt.cases[i].body_count; b++) {
            codegen_generate_statement(gen, stmt->match_stmt.cases[i].body[b]);
        }
        
        snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
        codegen_emit(gen, buffer);
        snprintf(buffer, sizeof(buffer), ".L%d:", next_case);
        codegen_emit(gen, buffer);
    }
    
    snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
    codegen_emit(gen, buffer);
    codegen_emit(gen, "    add rsp, 8");
}

// Helper for state declaration
void codegen_generate_state_decl(Codegen* gen, Statement* stmt) {
    int found = 0;
    for (int i = 0; i < gen->global_var_count; i++) {
        if (strcmp(gen->global_vars[i].name, stmt->state_decl.name) == 0) {
            found = 1;
            break;
        }
    }
    
    if (!found && gen->global_var_count < MAX_STATE_VARS) {
        char* name_copy = malloc(strlen(stmt->state_decl.name) + 1);
        strcpy(name_copy, stmt->state_decl.name);
        gen->global_vars[gen->global_var_count].name = name_copy;
        gen->global_vars[gen->global_var_count].type = stmt->state_decl.type;
        gen->global_vars[gen->global_var_count].is_global = 1;
        gen->global_vars[gen->global_var_count].is_state = 1;
        gen->global_vars[gen->global_var_count].init_expr = stmt->state_decl.initial_value;
        gen->global_var_count++;
    }
}

// Helper for destructure
void codegen_generate_destructure(Codegen* gen, Statement* stmt) {
    char buffer[256];
    
    if (stmt->destructure.source->type != EXPR_VARIABLE) {
        fprintf(stderr, "Codegen error: Destructuring source must be a variable\n");
        exit(1);
    }
    
    VarInfo* src_var = codegen_get_variable(gen, stmt->destructure.source->var_name);
    snprintf(buffer, sizeof(buffer), "    lea rax, [rbp-%d]", src_var->stack_offset);
    codegen_emit(gen, buffer);
    
    for (int i = 0; i < stmt->destructure.var_count; i++) {
        gen->stack_offset += 8;
        codegen_add_variable(gen, stmt->destructure.var_names[i],
                           gen->stack_offset, stmt->destructure.var_type, NULL);
        codegen_emit(gen, "    sub rsp, 8");
        codegen_emit(gen, "    push rax");
        snprintf(buffer, sizeof(buffer), "    mov rbx, [rax+%d]", i * 8);
        codegen_emit(gen, buffer);
        snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rbx", gen->stack_offset);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    pop rax");
    }
}
