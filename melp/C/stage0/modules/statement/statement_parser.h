#ifndef STATEMENT_PARSER_H
#define STATEMENT_PARSER_H

#include "statement.h"
#include "../parser_core/parser_core.h"

// Statement parsing delegated to specific modules
Statement* statement_parse(Parser* parser);

// Alias for compatibility
#define parse_statement statement_parse

#endif
