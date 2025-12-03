#ifndef STRUCT_PARSER_H
#define STRUCT_PARSER_H

#include "struct.h"
#include "../../lexer.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
StructDef* parse_struct_definition(Lexer* lexer);
StructInstance* parse_struct_instance(Lexer* lexer);
MemberAccess* parse_member_access(Lexer* lexer);

#endif // STRUCT_PARSER_H
