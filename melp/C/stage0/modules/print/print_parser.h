#ifndef PRINT_PARSER_H
#define PRINT_PARSER_H

#include "print.h"
#include "../../lexer.h"

// Parse print statement: print <expression>
PrintStatement* parse_print_statement(Lexer* lexer);

#endif
