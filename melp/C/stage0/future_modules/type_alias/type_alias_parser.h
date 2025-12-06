#ifndef TYPE_ALIAS_PARSER_H
#define TYPE_ALIAS_PARSER_H

#include "type_alias.h"
#include "../../lexer.h"

// Parse type alias declaration
TypeAlias* parse_type_alias(Lexer* lexer);

#endif // TYPE_ALIAS_PARSER_H
