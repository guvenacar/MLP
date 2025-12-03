#ifndef POINTER_PARSER_H
#define POINTER_PARSER_H

#include "pointer.h"
#include "../../lexer.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
Pointer* parse_pointer_declaration(Lexer* lexer);
AddressOf* parse_address_of(Lexer* lexer);
Dereference* parse_dereference(Lexer* lexer);
PointerArith* parse_pointer_arith(Lexer* lexer);

#endif // POINTER_PARSER_H
