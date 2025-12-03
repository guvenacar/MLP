#ifndef CONTROL_FLOW_CODEGEN_H
#define CONTROL_FLOW_CODEGEN_H

#include <stdio.h>
#include "control_flow.h"

void control_flow_generate_if(FILE* output, IfStatement* stmt);
void control_flow_generate_while(FILE* output, WhileStatement* stmt);
void control_flow_generate_for(FILE* output, ForStatement* stmt);

#endif // CONTROL_FLOW_CODEGEN_H
