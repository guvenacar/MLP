#ifndef MODULE_SYSTEM_PARSER_H
#define MODULE_SYSTEM_PARSER_H

#include "module_system.h"
#include "../../lexer.h"
#include "../parser_core/parser_core.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
Module* parse_module(Lexer* lexer);
Import* parse_import(Lexer* lexer);
Export* parse_export(Lexer* lexer);
Symbol* parse_symbol_declaration(Lexer* lexer);

#endif // MODULE_SYSTEM_PARSER_H
