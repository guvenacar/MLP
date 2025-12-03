#ifndef DEBUG_FEATURES_PARSER_H
#define DEBUG_FEATURES_PARSER_H

#include "debug_features.h"
#include "../../lexer.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
DebugLabel* parse_debug_label(Lexer* lexer);
DebugGoto* parse_debug_goto(Lexer* lexer);
DebugPause* parse_debug_pause(Lexer* lexer);
DebugPrint* parse_debug_print(Lexer* lexer);
DebugIf* parse_debug_if(Lexer* lexer);

#endif // DEBUG_FEATURES_PARSER_H
