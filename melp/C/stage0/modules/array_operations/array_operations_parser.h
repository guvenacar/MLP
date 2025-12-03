#ifndef ARRAY_OPERATIONS_PARSER_H
#define ARRAY_OPERATIONS_PARSER_H

#include "../../lexer.h"
#include "array_operations.h"

// Parse array method calls: array.push(value), array.pop(), etc.
ArrayOperation* parse_array_method(Lexer* lexer, const char* method_name);

// Parse array property access: array.length
ArrayOperation* parse_array_property(Lexer* lexer, const char* property_name);

#endif
