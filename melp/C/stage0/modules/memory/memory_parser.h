#ifndef MEMORY_PARSER_H
#define MEMORY_PARSER_H

#include "memory.h"
#include "../../lexer.h"
#include "../parser_core/parser_core.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
MemoryAlloc* parse_memory_alloc(Lexer* lexer);
MemoryFree* parse_memory_free(Lexer* lexer);
MemoryCopy* parse_memory_copy(Lexer* lexer);
MemoryZero* parse_memory_zero(Lexer* lexer);
MemoryRealloc* parse_memory_realloc(Lexer* lexer);

#endif // MEMORY_PARSER_H
