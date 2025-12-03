#include "pointer_parser.h"
#include <stdlib.h>

// TODO: Stage 1 - Pointer tanımı parse et
// Syntax: numeric* ptr = &x
Pointer* parse_pointer_declaration(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Adres alma operasyonu parse et
// Syntax: &variable
AddressOf* parse_address_of(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Dereference operasyonu parse et
// Syntax: *pointer
Dereference* parse_dereference(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Pointer aritmetiği parse et
// Syntax: ptr + 5, ptr - 3, ptr1 - ptr2
PointerArith* parse_pointer_arith(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}
