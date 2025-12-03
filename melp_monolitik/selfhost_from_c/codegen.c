#include "codegen.h"
#include "modules/control_flow/control_flow.h"
#include "modules/control_flow/control_flow_codegen.h"
#include "modules/arithmetic/arithmetic_codegen.h"
#include "modules/variable/variable.h"
#include "modules/variable/variable_codegen.h"
#include "modules/string_ops/string_ops_codegen.h"
#include "modules/cli_io/cli_io_codegen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Global label counter - shared across modules
static int label_counter = 0;

// Global print string counter (shared between modules via codegen.h)
int print_string_counter = 0;

// Global variable type tracking
VarInfo global_var_types[MAX_GLOBAL_VARS];
int global_var_count = 0;

// String literals registry
StringLiteralInfo string_literals[MAX_STRING_LITERALS];
int string_literal_count = 0;

// Current function context for parameter resolution
FunctionContext* current_function_ctx = NULL;

// Add string literal to registry and return its ID
int add_string_literal(const char* value) {
    if (string_literal_count >= MAX_STRING_LITERALS) return -1;
    
    // Check if already exists
    for (int i = 0; i < string_literal_count; i++) {
        if (strcmp(string_literals[i].value, value) == 0) {
            return string_literals[i].id;
        }
    }
    
    // Add new
    int id = string_literal_count;
    string_literals[string_literal_count].value = strdup(value);
    string_literals[string_literal_count].id = id;
    string_literal_count++;
    return id;
}

// Helper to check if a variable is a parameter and get its stack offset
int get_param_stack_offset(const char* var_name) {
    if (!current_function_ctx || !var_name) return -1;
    
    for (int i = 0; i < current_function_ctx->param_count; i++) {
        if (strcmp(current_function_ctx->param_names[i], var_name) == 0) {
            return (i + 1) * 8;  // First param at [rbp-8], second at [rbp-16], etc.
        }
    }
    return -1;  // Not a parameter
}

// Helper to get local variable stack offset, or add new local
int get_or_add_local_offset(const char* var_name) {
    if (!current_function_ctx || !var_name) return -1;
    
    // Check if already exists
    for (int i = 0; i < current_function_ctx->local_count; i++) {
        if (strcmp(current_function_ctx->local_names[i], var_name) == 0) {
            // Locals start after params: params at [rbp-8] to [rbp-param_count*8]
            // Locals at [rbp-(param_count+1)*8] onwards
            return (current_function_ctx->param_count + i + 1) * 8;
        }
    }
    
    // Add new local
    if (current_function_ctx->local_count < MAX_LOCAL_VARS) {
        current_function_ctx->local_names[current_function_ctx->local_count] = strdup(var_name);
        int offset = (current_function_ctx->param_count + current_function_ctx->local_count + 1) * 8;
        current_function_ctx->local_count++;
        return offset;
    }
    
    return -1;  // Too many locals
}

// Check if variable is local (parameter or local var in current function)
int is_local_variable(const char* var_name) {
    if (!current_function_ctx || !var_name) return 0;
    
    // Check params
    for (int i = 0; i < current_function_ctx->param_count; i++) {
        if (strcmp(current_function_ctx->param_names[i], var_name) == 0) {
            return 1;
        }
    }
    
    // Check locals
    for (int i = 0; i < current_function_ctx->local_count; i++) {
        if (current_function_ctx->local_names[i] && 
            strcmp(current_function_ctx->local_names[i], var_name) == 0) {
            return 1;
        }
    }
    
    return 0;
}

// Forward declarations
static void codegen_print_statement(FILE* f, PrintStmt* print_stmt);
static void codegen_statement(FILE* f, Statement* stmt);
static void codegen_function_body_statements(FILE* f, Statement** body, int count);

// Helper: Generate code for function call
static void codegen_function_call(FILE* f, CallStmt* call) {
    fprintf(f, "    ; Call function: %s\n", call->name);
    
    // System V AMD64 ABI: rdi, rsi, rdx, rcx, r8, r9 for first 6 args
    const char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    
    // Evaluate arguments and place in registers (in reverse order to handle stack)
    for (int i = 0; i < call->arg_count && i < 6; i++) {
        codegen_expression(f, call->args[i]);
        fprintf(f, "    push rax\n");
    }
    
    // Pop into registers in correct order
    for (int i = call->arg_count - 1; i >= 0 && i < 6; i--) {
        fprintf(f, "    pop %s\n", arg_regs[i]);
    }
    
    // Call the function
    fprintf(f, "    call %s\n", call->name);
}

// Helper: Generate return statement
static void codegen_return_statement(FILE* f, ReturnStmt* ret) {
    fprintf(f, "    ; return\n");
    if (ret->value) {
        codegen_expression(f, ret->value);
    } else {
        fprintf(f, "    xor rax, rax\n");
    }
    fprintf(f, "    mov rsp, rbp\n");
    fprintf(f, "    pop rbp\n");
    fprintf(f, "    ret\n");
}

// Forward declaration for recursive expression scanning
static void collect_expression_strings(Expression* expr);

// Recursively collect string literals from an expression
static void collect_expression_strings(Expression* expr) {
    if (!expr) return;
    
    if (expr->type == EXPR_STRING) {
        // Register the string literal
        add_string_literal(expr->data.string_value);
    } else if (expr->type == EXPR_BINARY_OP) {
        collect_expression_strings(expr->data.binary.left);
        collect_expression_strings(expr->data.binary.right);
    } else if (expr->type == EXPR_CALL) {
        // Scan function arguments
        for (int i = 0; i < expr->data.call.arg_count; i++) {
            collect_expression_strings(expr->data.call.args[i]);
        }
    }
}

// Collect string literals from statements (for expression contexts)
static void collect_statement_strings(Statement** stmts, int count) {
    for (int i = 0; i < count; i++) {
        Statement* stmt = stmts[i];
        if (!stmt) continue;
        
        // Check for declaration with value expression
        if (stmt->type == STMT_DECLARATION && stmt->data.decl_stmt.value) {
            collect_expression_strings(stmt->data.decl_stmt.value);
        }
        
        // Check print statements
        if (stmt->type == STMT_PRINT && stmt->data.print_stmt.value) {
            collect_expression_strings(stmt->data.print_stmt.value);
        }
        
        // Recurse into control structures
        if (stmt->type == STMT_IF) {
            collect_statement_strings(stmt->data.if_stmt.then_body, stmt->data.if_stmt.then_count);
            if (stmt->data.if_stmt.else_body) {
                collect_statement_strings(stmt->data.if_stmt.else_body, stmt->data.if_stmt.else_count);
            }
        } else if (stmt->type == STMT_WHILE) {
            collect_statement_strings(stmt->data.while_stmt.body, stmt->data.while_stmt.body_count);
        } else if (stmt->type == STMT_FOR) {
            collect_statement_strings(stmt->data.for_stmt.body, stmt->data.for_stmt.body_count);
        } else if (stmt->type == STMT_FUNCTION) {
            collect_statement_strings(stmt->data.func_stmt.body, stmt->data.func_stmt.body_count);
        }
    }
}

// Output collected string literals to data section
static void output_string_literals(FILE* f) {
    for (int i = 0; i < string_literal_count; i++) {
        fprintf(f, "    str_lit_%d db \"%s\", 0\n", string_literals[i].id, string_literals[i].value);
    }
}

// Collect print string literals from statements
static void collect_print_strings(FILE* f, Statement** stmts, int count, int* str_counter) {
    for (int i = 0; i < count; i++) {
        Statement* stmt = stmts[i];
        if (!stmt) continue;
        
        if (stmt->type == STMT_PRINT) {
            Expression* expr = stmt->data.print_stmt.value;
            if (expr && expr->type == EXPR_STRING) {
                fprintf(f, "    print_str_%d db \"%s\", 10, 0\n", *str_counter, expr->data.string_value);
                fprintf(f, "    print_str_%d_len equ $ - print_str_%d\n", *str_counter, *str_counter);
                (*str_counter)++;
            }
        } else if (stmt->type == STMT_IF) {
            collect_print_strings(f, stmt->data.if_stmt.then_body, stmt->data.if_stmt.then_count, str_counter);
            if (stmt->data.if_stmt.else_body) {
                collect_print_strings(f, stmt->data.if_stmt.else_body, stmt->data.if_stmt.else_count, str_counter);
            }
        } else if (stmt->type == STMT_WHILE) {
            collect_print_strings(f, stmt->data.while_stmt.body, stmt->data.while_stmt.body_count, str_counter);
        } else if (stmt->type == STMT_FOR) {
            collect_print_strings(f, stmt->data.for_stmt.body, stmt->data.for_stmt.body_count, str_counter);
        } else if (stmt->type == STMT_FUNCTION) {
            // Also collect from function bodies
            collect_print_strings(f, stmt->data.func_stmt.body, stmt->data.func_stmt.body_count, str_counter);
        }
    }
}

// Generate print statement code
static void codegen_print_statement(FILE* f, PrintStmt* print_stmt) {
    Expression* expr = print_stmt->value;
    
    if (expr->type == EXPR_STRING) {
        // Print string literal
        int str_id = print_string_counter++;
        fprintf(f, "    ; print string literal\n");
        fprintf(f, "    mov rax, 1          ; sys_write\n");
        fprintf(f, "    mov rdi, 1          ; stdout\n");
        fprintf(f, "    mov rsi, print_str_%d\n", str_id);
        fprintf(f, "    mov rdx, print_str_%d_len\n", str_id);
        fprintf(f, "    syscall\n");
    } else if (expr->type == EXPR_VARIABLE) {
        // Check if variable is string type
        VarType var_type = get_var_type(expr->data.var_name);
        if (var_type == VAR_STRING) {
            // Print string variable
            fprintf(f, "    ; print string variable\n");
            codegen_expression(f, expr);  // Gets pointer into rax
            fprintf(f, "    call print_string\n");
        } else {
            // Print numeric variable
            fprintf(f, "    ; print numeric variable\n");
            codegen_expression(f, expr);
            fprintf(f, "    call print_int\n");
        }
    } else if (expr->type == EXPR_CALL) {
        // Function call - check if it's a string-returning built-in
        const char* func_name = expr->data.call.func_name;
        if (strcmp(func_name, "concat") == 0 || 
            strcmp(func_name, "substr") == 0) {
            // String-returning function
            fprintf(f, "    ; print string function result\n");
            codegen_expression(f, expr);
            fprintf(f, "    call print_string\n");
        } else {
            // Numeric function
            fprintf(f, "    ; print function result\n");
            codegen_expression(f, expr);
            fprintf(f, "    call print_int\n");
        }
    } else {
        // Print numeric expression
        fprintf(f, "    ; print expression\n");
        codegen_expression(f, expr);
        fprintf(f, "    call print_int\n");
    }
}

// Generate code for statement - dispatches to module codegens
static void codegen_statement(FILE* f, Statement* stmt) {
    if (!stmt) return;
    
    switch (stmt->type) {
        case STMT_PRINT:
            codegen_print_statement(f, &stmt->data.print_stmt);
            break;
        case STMT_CALL:
            codegen_function_call(f, &stmt->data.call_stmt);
            break;
        case STMT_RETURN:
            codegen_return_statement(f, &stmt->data.return_stmt);
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

// Generate statements in function body
static void codegen_function_body_statements(FILE* f, Statement** body, int count) {
    for (int i = 0; i < count; i++) {
        codegen_statement(f, body[i]);
    }
}

// Generate a function definition
static void codegen_function_def(FILE* f, FunctionStmt* func) {
    fprintf(f, "\n; ============ Function: %s ============\n", func->name);
    fprintf(f, "%s:\n", func->name);
    
    // Prologue: Set up stack frame
    fprintf(f, "    push rbp\n");
    fprintf(f, "    mov rbp, rsp\n");
    
    // Reserve space for local variables (estimate 8 bytes each * 16 max)
    fprintf(f, "    sub rsp, 256\n");  // Increased for more locals
    
    // Set up function context for parameter and local variable resolution
    FunctionContext ctx;
    ctx.func_name = func->name;
    ctx.param_count = func->param_count;
    ctx.param_names = malloc(sizeof(char*) * func->param_count);
    for (int i = 0; i < func->param_count; i++) {
        ctx.param_names[i] = func->params[i]->name;
    }
    ctx.local_count = 0;
    for (int i = 0; i < MAX_LOCAL_VARS; i++) {
        ctx.local_names[i] = NULL;
    }
    current_function_ctx = &ctx;
    
    // Save parameters to stack (System V AMD64 ABI)
    // rdi, rsi, rdx, rcx, r8, r9 contain first 6 args
    const char* param_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    for (int i = 0; i < func->param_count && i < 6; i++) {
        fprintf(f, "    mov [rbp-%d], %s  ; param %s\n", 
                (i+1)*8, param_regs[i], func->params[i]->name);
    }
    
    // Generate function body
    codegen_function_body_statements(f, func->body, func->body_count);
    
    // Clear function context - free local names
    for (int i = 0; i < ctx.local_count; i++) {
        free(ctx.local_names[i]);
    }
    free(ctx.param_names);
    current_function_ctx = NULL;
    
    // Epilogue: If no explicit return, add default return
    fprintf(f, "    ; default return\n");
    fprintf(f, "    xor rax, rax\n");
    fprintf(f, "    mov rsp, rbp\n");
    fprintf(f, "    pop rbp\n");
    fprintf(f, "    ret\n");
}

void codegen_generate(Program* program, const char* output_file) {
    FILE* f = fopen(output_file, "w");
    if (!f) {
        fprintf(stderr, "Error: Cannot open output file\n");
        return;
    }
    
    // Reset counters
    label_counter = 0;
    print_string_counter = 0;
    global_var_count = 0;  // Reset variable type tracking
    string_literal_count = 0;  // Reset string literal registry
    
    // First pass: collect all string literals from expressions
    collect_statement_strings(program->statements, program->stmt_count);
    for (int i = 0; i < program->func_count; i++) {
        Statement* func = program->functions[i];
        collect_statement_strings(func->data.func_stmt.body, func->data.func_stmt.body_count);
    }
    // Also collect from declarations with init_expr
    for (int i = 0; i < program->decl_count; i++) {
        Declaration* decl = program->declarations[i];
        if (decl->init_expr) {
            collect_expression_strings(decl->init_expr);
        }
    }
    
    fprintf(f, "section .data\n");
    
    // String constants from declarations
    for (int i = 0; i < program->decl_count; i++) {
        Declaration* decl = program->declarations[i];
        if (decl->type == VAR_STRING) {
            fprintf(f, "    str_%s db \"%s\", 0\n", decl->name, decl->value);
        }
    }
    
    // Collect print string literals from main code
    int temp_counter = 0;
    collect_print_strings(f, program->statements, program->stmt_count, &temp_counter);
    
    // Collect print string literals from functions
    for (int i = 0; i < program->func_count; i++) {
        Statement* func = program->functions[i];
        collect_print_strings(f, func->data.func_stmt.body, func->data.func_stmt.body_count, &temp_counter);
    }
    
    // Output string literals used in expressions
    output_string_literals(f);
    
    fprintf(f, "\nsection .bss\n");;
    
    // Variables from declarations
    for (int i = 0; i < program->decl_count; i++) {
        Declaration* decl = program->declarations[i];
        // Register variable type
        register_global_var(decl->name, decl->type);
        
        if (decl->type == VAR_NUMERIC) {
            fprintf(f, "    var_%s resq 1 ; INT64\n", decl->name);
        } else if (decl->type == VAR_BOOLEAN) {
            fprintf(f, "    var_%s resb 1 ; BOOLEAN\n", decl->name);
        } else if (decl->type == VAR_STRING) {
            fprintf(f, "    var_%s resq 1 ; STRING pointer\n", decl->name);
        }
    }
    
    // For loop variables
    for (int i = 0; i < program->stmt_count; i++) {
        Statement* stmt = program->statements[i];
        if (stmt->type == STMT_FOR) {
            fprintf(f, "    var_%s resq 1 ; FOR loop variable\n", stmt->data.for_stmt.variable);
        }
    }
    
    // Collect declarations from statement bodies
    collect_body_declarations(f, program->statements, program->stmt_count);
    
    // Collect declarations from function bodies
    for (int i = 0; i < program->func_count; i++) {
        Statement* func = program->functions[i];
        collect_body_declarations(f, func->data.func_stmt.body, func->data.func_stmt.body_count);
        
        // Also add function parameters as variables (use simple names, check duplicates later)
        for (int j = 0; j < func->data.func_stmt.param_count; j++) {
            FunctionParam* param = func->data.func_stmt.params[j];
            // Only add if not already declared by another function
            // For now, we're using global storage so same-named params share storage
            // This works because we only have single-threaded, non-recursive simple functions
            int already_declared = 0;
            for (int k = 0; k < i; k++) {
                Statement* other_func = program->functions[k];
                for (int l = 0; l < other_func->data.func_stmt.param_count; l++) {
                    if (strcmp(other_func->data.func_stmt.params[l]->name, param->name) == 0) {
                        already_declared = 1;
                        break;
                    }
                }
                if (already_declared) break;
            }
            if (!already_declared) {
                fprintf(f, "    var_%s resq 1 ; PARAM (function %s)\n", param->name, func->data.func_stmt.name);
            }
        }
    }
    
    fprintf(f, "\nsection .text\n");
    fprintf(f, "    global _start\n");
    
    // Generate CLI/IO helper functions (from cli_io module)
    cli_io_generate_helpers(f);
    
    // Generate string helper functions (from string_ops module)
    string_ops_generate_helpers(f);
    
    // Generate all function definitions BEFORE _start
    for (int i = 0; i < program->func_count; i++) {
        Statement* func = program->functions[i];
        codegen_function_def(f, &func->data.func_stmt);
    }
    
    fprintf(f, "\n_start:\n");
    
    // Initialize declarations
    for (int i = 0; i < program->decl_count; i++) {
        Declaration* decl = program->declarations[i];
        
        // Check if declaration has an expression initializer
        if (decl->init_expr) {
            fprintf(f, "    ; %s = <expression>\n", decl->name);
            codegen_expression(f, decl->init_expr);
            fprintf(f, "    mov [var_%s], rax\n", decl->name);
        } else if (decl->type == VAR_NUMERIC) {
            fprintf(f, "    ; %s = %s (INT64)\n", decl->name, decl->value);
            fprintf(f, "    mov rax, %s\n", decl->value);
            fprintf(f, "    mov [var_%s], rax\n", decl->name);
        } else if (decl->type == VAR_BOOLEAN) {
            int val = (decl->value && strcmp(decl->value, "true") == 0) ? 1 : 0;
            fprintf(f, "    ; %s = %s\n", decl->name, decl->value ? decl->value : "false");
            fprintf(f, "    mov byte [var_%s], %d\n", decl->name, val);
        } else if (decl->type == VAR_STRING) {
            fprintf(f, "    ; %s = \"%s\"\n", decl->name, decl->value);
            fprintf(f, "    mov rax, str_%s\n", decl->name);
            fprintf(f, "    mov [var_%s], rax\n", decl->name);
        }
    }
    
    // Generate code for statements
    for (int i = 0; i < program->stmt_count; i++) {
        codegen_statement(f, program->statements[i]);
    }
    
    // Exit
    fprintf(f, "\n    ; Exit program\n");
    fprintf(f, "    mov rax, 60\n");
    fprintf(f, "    xor rdi, rdi\n");
    fprintf(f, "    syscall\n");
    
    fclose(f);
}
