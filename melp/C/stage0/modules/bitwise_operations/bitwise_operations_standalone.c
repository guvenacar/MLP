#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitwise_operations.h"
#include "bitwise_operations_parser.h"
#include "bitwise_operations_codegen.h"
#include "../../lexer.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }

    printf("🔧 Bitwise Operations Module\n");
    printf("============================\n");
    printf("Input:  %s\n", argv[1]);
    printf("Output: %s\n\n", argv[2]);

    // Read source file
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open %s\n", argv[1]);
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* src = malloc(sz + 1);
    fread(src, 1, sz, f);
    src[sz] = 0;
    fclose(f);

    // Create lexer
    Lexer* lexer = lexer_create(src);

    // Open output file
    FILE* out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create %s\n", argv[2]);
        free(src);
        lexer_free(lexer);
        return 1;
    }

    // Scan for bitwise operations
    int op_count = 0;
    Token* tok;

    fprintf(out, "; MLP Bitwise Operations Module - Generated Assembly\n");
    fprintf(out, "; Target: x86-64 Linux\n\n");
    fprintf(out, "section .text\n");
    fprintf(out, "    global _start\n");
    fprintf(out, "_start:\n\n");

    // Simple scan: look for & | ^ << >> operators
    while (1) {
        tok = lexer_next_token(lexer);

        if (tok->type == TOKEN_EOF) {
            token_free(tok);
            break;
        }

        // Check for bitwise operators (simplified)
        if (strcmp(tok->value, "&") == 0 || strcmp(tok->value, "|") == 0 ||
            strcmp(tok->value, "^") == 0 || strcmp(tok->value, "<<") == 0 ||
            strcmp(tok->value, ">>") == 0) {

            printf("  ✓ Found bitwise operator: %s\n", tok->value);
            op_count++;
        }

        token_free(tok);
    }

    printf("\n✅ Found %d bitwise operations\n", op_count);

    // Add exit syscall
    fprintf(out, "    ; Program exit\n");
    fprintf(out, "    mov rax, 60\n");
    fprintf(out, "    xor rdi, rdi\n");
    fprintf(out, "    syscall\n");

    // Cleanup
    free(src);
    lexer_free(lexer);
    fclose(out);

    return 0;
}
