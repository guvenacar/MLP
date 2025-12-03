#ifndef CONTROL_FLOW_PARSER_H
#define CONTROL_FLOW_PARSER_H

#include "control_flow.h"
#include "../../lexer.h"

// Parser for control flow statements
typedef struct {
    Lexer* lexer;
    Token* current_token;
} ControlFlowParser;

// Parser functions
ControlFlowParser* cf_parser_create(Lexer* lexer);
void cf_parser_free(ControlFlowParser* parser);

// Parse statements
Statement* parse_if_statement(ControlFlowParser* parser);
Statement* parse_while_statement(ControlFlowParser* parser);
Statement* parse_for_statement(ControlFlowParser* parser);

// Free statement
void statement_free(Statement* stmt);

#endif
