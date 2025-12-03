#ifndef GENERIC_TYPES_PARSER_H
#define GENERIC_TYPES_PARSER_H

#include "generic_types.h"
#include "../../lexer.h"
#include "../parser_core/parser_core.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
GenericType* parse_generic_type(Lexer* lexer);
TypeParameter* parse_type_parameter(Lexer* lexer);
TypeConstraint* parse_type_constraint(Lexer* lexer);
TypeInstantiation* parse_type_instantiation(Lexer* lexer);
GenericFunction* parse_generic_function(Lexer* lexer);

#endif // GENERIC_TYPES_PARSER_H
