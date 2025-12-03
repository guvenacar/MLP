#ifndef STATEMENT_PARSER_H
#define STATEMENT_PARSER_H

#include "parser.h"
#include "modules/control_flow/control_flow.h"

// Parse a single statement (for function bodies)
Statement* parse_statement(Parser* parser);

// Free statement memory
void statement_free(Statement* stmt);

#endif
