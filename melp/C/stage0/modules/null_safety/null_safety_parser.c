#include "null_safety_parser.h"
#include <stdlib.h>

// TODO: Stage 1 - Optional parse et
// Syntax: numeric? x = some(5), numeric? y = none
Optional* parse_optional(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Null kontrol parse et
// Syntax: if x == null, if x != null, if opt.has_value()
NullCheck* parse_null_check(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Null-coalescing parse et
// Syntax: result = value ?? default
NullCoalesce* parse_null_coalesce(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Safe navigation parse et
// Syntax: value = obj?.field, chain = a?.b?.c
SafeNav* parse_safe_nav(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Null assertion parse et
// Syntax: value = opt!, value = opt! "Custom error message"
NullAssert* parse_null_assert(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}
