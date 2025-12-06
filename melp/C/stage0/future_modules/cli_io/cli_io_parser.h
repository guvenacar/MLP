#ifndef CLI_IO_PARSER_H
#define CLI_IO_PARSER_H

#include "cli_io.h"
#include "../../lexer.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
PrintOp* parse_print(Lexer* lexer);
InputOp* parse_input(Lexer* lexer);
ArgAccess* parse_arg_access(Lexer* lexer);
ColorOutput* parse_color_output(Lexer* lexer);

#endif // CLI_IO_PARSER_H
