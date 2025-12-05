#ifndef NULL_SAFETY_PARSER_H
#define NULL_SAFETY_PARSER_H

#include "null_safety.h"
#include "../../lexer.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
Optional* parse_optional(Lexer* lexer);
NullCheck* parse_null_check(Lexer* lexer);
NullCoalesce* parse_null_coalesce(Lexer* lexer);
SafeNav* parse_safe_nav(Lexer* lexer);
NullAssert* parse_null_assert(Lexer* lexer);

#endif // NULL_SAFETY_PARSER_H
