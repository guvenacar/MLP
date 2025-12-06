#ifndef PROGRAM_H
#define PROGRAM_H

#include "../../parser.h"

// Program module - Main program structure and building utilities
// This module handles program creation, modification, and cleanup

// Program building functions
Program* program_create(void);
void program_add_statement(Program* program, Statement* stmt);
void program_add_declaration(Program* program, Declaration* decl);
void program_add_function(Program* program, Statement* func);
void program_destroy(Program* program);

#endif
