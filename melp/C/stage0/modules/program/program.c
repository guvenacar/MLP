// Program Module - Program building utilities
#include "program.h"
#include "../variable/variable.h"
#include "../function/function.h"
#include <stdlib.h>
#include <string.h>

// Create new empty program
Program* program_create(void) {
    Program* program = malloc(sizeof(Program));
    
    // Initialize declarations
    program->decl_capacity = 10;
    program->decl_count = 0;
    program->declarations = malloc(sizeof(Declaration*) * program->decl_capacity);
    
    // Initialize statements
    program->stmt_capacity = 10;
    program->stmt_count = 0;
    program->statements = malloc(sizeof(Statement*) * program->stmt_capacity);
    
    // Initialize functions
    program->func_capacity = 10;
    program->func_count = 0;
    program->functions = malloc(sizeof(Statement*) * program->func_capacity);
    
    return program;
}

// Add statement to program
void program_add_statement(Program* program, Statement* stmt) {
    if (program->stmt_count >= program->stmt_capacity) {
        program->stmt_capacity *= 2;
        program->statements = realloc(program->statements, 
                                     sizeof(Statement*) * program->stmt_capacity);
    }
    program->statements[program->stmt_count++] = stmt;
}

// Add declaration to program
void program_add_declaration(Program* program, Declaration* decl) {
    if (program->decl_count >= program->decl_capacity) {
        program->decl_capacity *= 2;
        program->declarations = realloc(program->declarations, 
                                       sizeof(Declaration*) * program->decl_capacity);
    }
    program->declarations[program->decl_count++] = decl;
}

// Add function to program
void program_add_function(Program* program, Statement* func) {
    if (program->func_count >= program->func_capacity) {
        program->func_capacity *= 2;
        program->functions = realloc(program->functions, 
                                    sizeof(Statement*) * program->func_capacity);
    }
    program->functions[program->func_count++] = func;
}

// Free program and all its contents
void program_destroy(Program* program) {
    // Free declarations
    for (int i = 0; i < program->decl_count; i++) {
        free(program->declarations[i]->name);
        if (program->declarations[i]->value) {
            free(program->declarations[i]->value);
        }
        if (program->declarations[i]->init_expr) {
            expression_free(program->declarations[i]->init_expr);
        }
        free(program->declarations[i]);
    }
    free(program->declarations);
    
    // Free statements
    for (int i = 0; i < program->stmt_count; i++) {
        statement_free(program->statements[i]);
    }
    free(program->statements);
    
    // Free functions
    for (int i = 0; i < program->func_count; i++) {
        statement_free(program->functions[i]);
    }
    free(program->functions);
    
    free(program);
}
