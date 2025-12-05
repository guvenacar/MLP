#ifndef STRING_OPERATIONS_PARSER_H
#define STRING_OPERATIONS_PARSER_H

#include "../../lexer.h"
#include "string_operations.h"

// Parse string concatenation: str1 + str2
StringOperation* parse_string_concat(Lexer* lexer);

// Parse string function calls: length(str), substring(str, start, end), etc.
StringOperation* parse_string_function(Lexer* lexer, const char* func_name);

#endif
