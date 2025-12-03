#include "memory_parser.h"
#include <stdlib.h>

// TODO: Stage 1 - Bellek ayırma parse et
// Syntax: allocate(size) veya new Type[count]
MemoryAlloc* parse_memory_alloc(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Bellek serbest bırakma parse et
// Syntax: free(var) veya delete var
MemoryFree* parse_memory_free(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Bellek kopyalama parse et
// Syntax: copy(dest, src, size)
MemoryCopy* parse_memory_copy(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Bellek sıfırlama parse et
// Syntax: zero(var, size)
MemoryZero* parse_memory_zero(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Bellek yeniden boyutlandırma parse et
// Syntax: resize(var, new_size)
MemoryRealloc* parse_memory_realloc(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}
