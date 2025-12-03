#ifndef CONTROL_FLOW_CODEGEN_H
#define CONTROL_FLOW_CODEGEN_H

#include "control_flow.h"
#include <stdio.h>

// Code generation for control flow statements
void codegen_if_statement(FILE* output, IfStatement* if_stmt, int* label_counter);
void codegen_while_statement(FILE* output, WhileStatement* while_stmt, int* label_counter);
void codegen_for_statement(FILE* output, ForStatement* for_stmt, int* label_counter);

#endif
