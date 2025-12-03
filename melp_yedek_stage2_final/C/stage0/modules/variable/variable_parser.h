#ifndef VARIABLE_PARSER_H
#define VARIABLE_PARSER_H

#include "../../lexer.h"
#include "../../parser.h"
#include "variable.h"

// Variable-specific extensions to Declaration (if needed)
// Note: Parser, Program, Declaration are now in parser.h

Program* variable_parse(Parser* parser);

#endif
