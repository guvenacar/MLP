#ifndef CONTROL_FLOW_PARSER_H
#define CONTROL_FLOW_PARSER_H

#include "control_flow.h"
#include "../../lexer.h"

typedef struct {
    Lexer* lexer;
    Token* current_token;
} ControlFlowParser;

ControlFlowParser* control_flow_parser_create(Lexer* lexer);
void control_flow_parser_free(ControlFlowParser* parser);

IfStatement* control_flow_parse_if(ControlFlowParser* parser);
IfStatement* control_flow_parse_if_expr(ControlFlowParser* parser);
WhileStatement* control_flow_parse_while(ControlFlowParser* parser);
ForStatement* control_flow_parse_for(ControlFlowParser* parser);

#endif // CONTROL_FLOW_PARSER_H
