#ifndef ENUM_PARSER_H
#define ENUM_PARSER_H

#include "enum.h"
#include "../../lexer.h"
#include "../parser_core/parser_core.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
EnumDef* parse_enum_def(Lexer* lexer);
EnumVariant* parse_enum_variant(Lexer* lexer);
EnumMatch* parse_enum_match(Lexer* lexer);

#endif // ENUM_PARSER_H
