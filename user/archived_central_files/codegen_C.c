#include "codegen.h"
#include "modules/control_flow/control_flow.h"
#include "modules/control_flow/control_flow_codegen.h"
#include "modules/arithmetic/arithmetic_codegen.h"
#include "modules/variable/variable.h"
#include "modules/variable/variable_codegen.h"
#include "modules/string_ops/string_ops_codegen.h"
#include "modules/cli_io/cli_io_codegen.h"
#include "modules/function/function_codegen.h"
#include "modules/print/print_codegen.h"
#include "modules/program/program_codegen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ============================================================================
// GLOBAL STATE (shared across modules via codegen.h)
// ============================================================================

// Global label counter - shared across modules
static int label_counter = 0;

// Global print string counter
int print_string_counter = 0;

// Global loop context stack (for exit/continue)
LoopContext loop_stack[MAX_LOOP_DEPTH];
int loop_depth = 0;

// Global variable type tracking
VarInfo global_var_types[MAX_GLOBAL_VARS];
int global_var_count = 0;

// Current function context for parameter resolution
FunctionContext* current_function_ctx = NULL;

// ============================================================================
// STATEMENT CODE GENERATION ROUTER
// ============================================================================

// Generate code for statement - dispatches to module codegens
void codegen_statement(FILE* f, Statement* stmt) {
    if (!stmt) return;
    
    switch (stmt->type) {
        case STMT_PRINT:
            codegen_print_statement(f, &stmt->data.print_stmt);
            break;
        case STMT_CALL:
            codegen_function_call_stmt(f, &stmt->data.call_stmt);
            break;
        case STMT_RETURN:
            codegen_return_stmt(f, &stmt->data.return_stmt);
            break;
        case STMT_EXIT:
            // Exit from current loop
            if (loop_depth > 0) {
                fprintf(f, "    ; exit for/while\n");
                fprintf(f, "    jmp .loop_end_%d\n", loop_stack[loop_depth - 1].exit_label);
            } else {
                fprintf(stderr, "Error: 'exit' used outside loop\n");
            }
            break;
        case STMT_CONTINUE:
            // Continue to next iteration
            if (loop_depth > 0) {
                fprintf(f, "    ; continue for/while\n");
                fprintf(f, "    jmp .loop_start_%d\n", loop_stack[loop_depth - 1].continue_label);
            } else {
                fprintf(stderr, "Error: 'continue' used outside loop\n");
            }
            break;
        case STMT_FUNCTION:
            // Functions are generated separately, not inline
            break;
        default:
            // Use control_flow module for other statements
            codegen_control_flow_statement(f, stmt, &label_counter);
            break;
    }
}

// ============================================================================
// MAIN CODE GENERATION ENTRY POINT
// ============================================================================

void codegen_generate(Program* program, const char* output_file) {
    FILE* f = fopen(output_file, "w");
    if (!f) {
        fprintf(stderr, "Error: Cannot open output file\n");
        return;
    }
    
    // Reset global counters
    label_counter = 0;
    print_string_counter = 0;
    global_var_count = 0;
    string_literal_count = 0;
    
    // First pass: collect all string literals from expressions
    collect_statement_strings(program->statements, program->stmt_count);
    for (int i = 0; i < program->func_count; i++) {
        Statement* func = program->functions[i];
        collect_statement_strings(func->data.func_stmt.body, func->data.func_stmt.body_count);
    }
    for (int i = 0; i < program->decl_count; i++) {
        Declaration* decl = program->declarations[i];
        if (decl->init_expr) {
            collect_expression_strings(decl->init_expr);
        }
    }
    
    // Generate assembly sections (delegated to program module)
    codegen_data_section(f, program);
    codegen_bss_section(f, program);
    codegen_text_section(f, program);
    
    // Generate _start entry point
    fprintf(f, "\n_start:\n");
    
    // Initialize global variables
    codegen_initialize_declarations(f, program);
    
    // Generate main program statements
    codegen_main_statements(f, program);
    
    // Generate program exit
    codegen_program_exit(f);
    
    fclose(f);
}
