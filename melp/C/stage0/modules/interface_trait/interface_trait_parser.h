#ifndef INTERFACE_TRAIT_PARSER_H
#define INTERFACE_TRAIT_PARSER_H

#include "interface_trait.h"
#include "../../lexer.h"
#include "../parser_core/parser_core.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
TraitDef* parse_trait_def(Lexer* lexer);
TraitMethod* parse_trait_method(Lexer* lexer);
TraitImpl* parse_trait_impl(Lexer* lexer);
TraitBound* parse_trait_bound(Lexer* lexer);

#endif // INTERFACE_TRAIT_PARSER_H
