#include "lambda_parser.h"
#include <stdlib.h>

// TODO: Stage 1 - Lambda parse et
// Syntax: |x, y| => x + y, [x, y](a, b) => x + a + b
Lambda* parse_lambda(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Lambda çağrısı parse et
// Syntax: lambda(1, 2), map(arr, |x| => x * 2)
LambdaCall* parse_lambda_call(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Higher-order function parse et
// Syntax: map(arr, func), filter(list, predicate)
HigherOrder* parse_higher_order(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}

// TODO: Stage 1 - Capture listesi parse et
// Syntax: [x, &y] - x by value, y by reference
CapturedVar* parse_capture_list(Lexer* lexer) {
    // Placeholder implementation
    return NULL;
}
