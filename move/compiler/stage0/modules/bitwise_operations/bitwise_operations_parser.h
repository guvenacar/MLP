#ifndef BITWISE_OPERATIONS_PARSER_H
#define BITWISE_OPERATIONS_PARSER_H

#include "bitwise_operations.h"
#include "../../lexer.h"

// Parse bitwise operations
// Syntax examples:
//   x & y        (bitwise AND)
//   x | y        (bitwise OR)
//   x ^ y        (bitwise XOR)
//   ~x           (bitwise NOT)
//   x << 2       (left shift)
//   x >> 3       (right shift)
BitwiseExpr* parse_bitwise_operation(Lexer* lexer);

#endif
