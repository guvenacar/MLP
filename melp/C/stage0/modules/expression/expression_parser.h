#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include "expression.h"
#include "../parser_core/parser_core.h"

// Expression parsing functions
Expression* expression_parse(Parser* parser);

// Alias for compatibility
#define parse_expression expression_parse

#endif
