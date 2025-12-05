#include <stdio.h>
#include <stdlib.h>
#include "bitwise_operations.h"
#include "bitwise_operations_parser.h"
#include "bitwise_operations_codegen.h"
#include "../../lexer.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.mlp>\n", argv[0]);
        return 1;
    }

    // Initialize lexer
    Lexer* lexer = lexer_create(argv[1]);
    if (!lexer) {
        fprintf(stderr, "Failed to open file: %s\n", argv[1]);
        return 1;
    }

    printf("Bitwise Operations Module - Standalone Test\n");
    printf("===========================================\n\n");

    // TODO: Parse ve test et
    // BitwiseExpr* expr = parse_bitwise_operation(lexer);
    // if (expr) {
    //     FILE* out = fopen("output.s", "w");
    //     codegen_bitwise_operation(out, expr);
    //     fclose(out);
    //     bitwise_expr_free(expr);
    // }

    printf("Bitwise operations parsing not yet implemented.\n");
    printf("Example syntax:\n");
    printf("  x & y      # Bitwise AND\n");
    printf("  x | y      # Bitwise OR\n");
    printf("  x ^ y      # Bitwise XOR\n");
    printf("  ~x         # Bitwise NOT\n");
    printf("  x << 2     # Left shift\n");
    printf("  x >> 3     # Right shift\n");

    lexer_destroy(lexer);
    return 0;
}
