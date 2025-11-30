/**
 * MLP Codegen - Main Code Generation Entry Point
 * 
 * Main entry point for code generation including:
 * - Prologue generation
 * - First pass: struct, state, function, operator definitions
 * - _start entry point
 * - State variable initialization
 * - Main function call
 * - Second pass: non-definition statements
 * - Epilogue
 * - Generic function instantiation
 * - Data section generation (strings, state vars)
 */

#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
void codegen_emit_prologue(Codegen* gen);
void codegen_emit_epilogue(Codegen* gen);
void codegen_generate_statement(Codegen* gen, Statement* stmt);
void codegen_generate_expression_value(Codegen* gen, Expression* expr);
void codegen_generate_func_def(Codegen* gen, Statement* stmt);

void codegen_generate(Codegen* gen, AST* ast) {
    char buffer[2048];
    
    // Emit assembly prologue
    codegen_emit_prologue(gen);
    codegen_emit(gen, "");
    
    // First pass: Register struct types, state variables, generate function definitions,
    // operator definitions, and module definitions
    for (int i = 0; i < ast->count; i++) {
        if (ast->statements[i]->type == STMT_STRUCT_DEF) {
            codegen_generate_statement(gen, ast->statements[i]);
        } else if (ast->statements[i]->type == STMT_STATE_DECL) {
            codegen_generate_statement(gen, ast->statements[i]);
        } else if (ast->statements[i]->type == STMT_FUNC_DEF) {
            codegen_generate_statement(gen, ast->statements[i]);
        } else if (ast->statements[i]->type == STMT_OPERATOR_DEF) {
            codegen_generate_statement(gen, ast->statements[i]);
        } else if (ast->statements[i]->type == STMT_MODULE_DEF) {
            codegen_generate_statement(gen, ast->statements[i]);
        }
    }
    
    // Generate main code (_start entry point)
    codegen_emit(gen, "");
    codegen_emit(gen, "_start:");
    
    // Get argc and argv from stack BEFORE modifying stack
    codegen_emit(gen, "    ; Get argc and argv from stack");
    codegen_emit(gen, "    pop rdi               ; argc");
    codegen_emit(gen, "    mov rsi, rsp          ; argv");
    codegen_emit(gen, "    call mlp_get_argv     ; Convert to MLP string array");
    codegen_emit(gen, "");
    
    // Initialize GC
    codegen_emit(gen, "    ; Initialize GC");
    codegen_emit(gen, "    call gc_init");
    codegen_emit(gen, "");
    
    // Setup stack frame
    codegen_emit(gen, "    push rbp");
    codegen_emit(gen, "    mov rbp, rsp");
    codegen_emit(gen, "");
    
    // Initialize state variables
    if (gen->global_var_count > 0) {
        codegen_emit(gen, "    ; Initialize state variables");
        for (int i = 0; i < gen->global_var_count; i++) {
            if (gen->global_vars[i].init_expr) {
                codegen_generate_expression_value(gen, gen->global_vars[i].init_expr);
                snprintf(buffer, sizeof(buffer), "    mov [state_%s], rax", gen->global_vars[i].name);
                codegen_emit(gen, buffer);
            }
        }
        codegen_emit(gen, "");
    }
    
    // Check if main function exists and call it
    int has_main = 0;
    for (int i = 0; i < ast->count; i++) {
        if (ast->statements[i]->type == STMT_FUNC_DEF &&
            strcmp(ast->statements[i]->func_def.func_name, "main") == 0) {
            has_main = 1;
            break;
        }
    }
    
    if (has_main) {
        codegen_emit(gen, "    ; Call main function");
        codegen_emit(gen, "    call func_main");
    }
    
    // Second pass: Generate non-function, non-struct, non-module statements
    for (int i = 0; i < ast->count; i++) {
        if (ast->statements[i]->type != STMT_FUNC_DEF && 
            ast->statements[i]->type != STMT_STRUCT_DEF &&
            ast->statements[i]->type != STMT_MODULE_DEF &&
            ast->statements[i]->type != STMT_OPERATOR_DEF &&
            ast->statements[i]->type != STMT_STATE_DECL) {
            codegen_generate_statement(gen, ast->statements[i]);
        }
    }
    
    // Emit epilogue
    codegen_emit_epilogue(gen);
    
    // Generate pending generic function instances
    GenericInstance* gi = gen->generic_instances;
    while (gi) {
        if (!gi->generated) {
            gi->generated = 1;
            
            GenericFunc* gf = codegen_find_generic_func(gen, gi->original_name);
            if (gf && gf->func_stmt) {
                Statement* func_stmt = gf->func_stmt;
                
                VarType* original_types = func_stmt->func_def.param_types;
                VarType* instantiated_types = malloc(sizeof(VarType) * func_stmt->func_def.param_count);
                
                for (int i = 0; i < func_stmt->func_def.param_count; i++) {
                    if (func_stmt->func_def.param_is_generic && func_stmt->func_def.param_is_generic[i]) {
                        int idx = func_stmt->func_def.param_generic_index[i];
                        instantiated_types[i] = gi->type_args[idx];
                    } else {
                        instantiated_types[i] = original_types[i];
                    }
                }
                
                func_stmt->func_def.param_types = instantiated_types;
                char* original_name = func_stmt->func_def.func_name;
                func_stmt->func_def.func_name = gi->mangled_name;
                int saved_type_param_count = func_stmt->func_def.type_param_count;
                func_stmt->func_def.type_param_count = 0;
                
                codegen_emit(gen, "");
                codegen_emit(gen, "section .text");
                codegen_generate_func_def(gen, func_stmt);
                
                func_stmt->func_def.type_param_count = saved_type_param_count;
                func_stmt->func_def.func_name = original_name;
                func_stmt->func_def.param_types = original_types;
                free(instantiated_types);
            }
        }
        gi = gi->next;
    }
    
    // Emit string data section
    if (gen->strings) {
        codegen_emit(gen, "");
        codegen_emit(gen, "section .data");
        StringLiteral* str = gen->strings;
        while (str) {
            char escaped[1024];
            int j = 0;
            for (int i = 0; str->value[i] && j < 1020; i++) {
                char c = str->value[i];
                if (c == '\n') {
                    escaped[j++] = '"'; escaped[j++] = ','; escaped[j++] = ' ';
                    escaped[j++] = '1'; escaped[j++] = '0';
                    escaped[j++] = ','; escaped[j++] = ' '; escaped[j++] = '"';
                } else if (c == '\t') {
                    escaped[j++] = '"'; escaped[j++] = ','; escaped[j++] = ' ';
                    escaped[j++] = '9';
                    escaped[j++] = ','; escaped[j++] = ' '; escaped[j++] = '"';
                } else if (c == '\r') {
                    escaped[j++] = '"'; escaped[j++] = ','; escaped[j++] = ' ';
                    escaped[j++] = '1'; escaped[j++] = '3';
                    escaped[j++] = ','; escaped[j++] = ' '; escaped[j++] = '"';
                } else if (c == '"') {
                    escaped[j++] = '"'; escaped[j++] = ','; escaped[j++] = ' ';
                    escaped[j++] = '3'; escaped[j++] = '4';
                    escaped[j++] = ','; escaped[j++] = ' '; escaped[j++] = '"';
                } else if (c == '\\') {
                    escaped[j++] = '\\'; escaped[j++] = '\\';
                } else {
                    escaped[j++] = c;
                }
            }
            escaped[j] = '\0';
            snprintf(buffer, sizeof(buffer), "str_%d: db \"%s\", 0", str->id, escaped);
            codegen_emit(gen, buffer);
            str = str->next;
        }
    }
    
    // Emit state variables in .bss section
    if (gen->global_var_count > 0) {
        codegen_emit(gen, "");
        codegen_emit(gen, "section .bss");
        codegen_emit(gen, "    ; Global state variables");
        for (int i = 0; i < gen->global_var_count; i++) {
            snprintf(buffer, sizeof(buffer), "state_%s: resq 1", gen->global_vars[i].name);
            codegen_emit(gen, buffer);
        }
    }
}
