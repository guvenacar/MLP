#ifndef SWITCH_MATCH_PARSER_H
#define SWITCH_MATCH_PARSER_H

#include "switch_match.h"
#include "../../lexer.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
Switch* parse_switch(Lexer* lexer);
SwitchMatch* parse_switch_match(Lexer* lexer);
Case* parse_case(Lexer* lexer);
Range* parse_range(Lexer* lexer);

#endif // SWITCH_MATCH_PARSER_H
