#ifndef GENERATOR_PARSER_H
#define GENERATOR_PARSER_H

#include "generator.h"
#include "../../lexer.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
Generator* parse_generator(Lexer* lexer);
Yield* parse_yield(Lexer* lexer);
GenCall* parse_gen_call(Lexer* lexer);
Iterator* parse_for_generator(Lexer* lexer);

#endif // GENERATOR_PARSER_H
