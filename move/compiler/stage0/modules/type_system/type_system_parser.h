#ifndef TYPE_SYSTEM_PARSER_H
#define TYPE_SYSTEM_PARSER_H

#include "type_system.h"
#include "../../lexer.h"

// Parse type annotations
// Syntax examples:
//   int
//   float
//   string
//   array[int]
//   *int
//   func(int, string) -> bool
Type* parse_type_annotation(Lexer* lexer);

// Parse type inference (from expression)
Type* parse_infer_type(Lexer* lexer, TypeContext* ctx);

#endif
