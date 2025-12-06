#include "operator_overloading_parser.h"
#include <stdlib.h>

// TODO: Stage 1 - Operator overload parse et
// Syntax: operator +(Vector a, Vector b) -> Vector { ... }
OperatorOverload* parse_operator_overload(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Custom operator parse et
// Syntax: operator @@ precedence 5 { ... }
CustomOperator* parse_custom_operator(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Operator sembolü parse et
OperatorType parse_operator_symbol(Lexer* lexer) {
    // Placeholder implementation
    return OP_ADD;
}
