#ifndef PATTERN_MATCHING_PARSER_H
#define PATTERN_MATCHING_PARSER_H

#include "pattern_matching.h"
#include "../../lexer.h"
#include "../parser_core/parser_core.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
Match* parse_match(Lexer* lexer);
MatchArm* parse_match_arm(Lexer* lexer);
Pattern* parse_pattern(Lexer* lexer);
Destructure* parse_destructure(Lexer* lexer);

#endif // PATTERN_MATCHING_PARSER_H
