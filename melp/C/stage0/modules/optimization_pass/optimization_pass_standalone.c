#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "optimization_pass.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.s> <output.s>\n", argv[0]);
        return 1;
    }

    printf("🔧 Optimization Pass Module\n");
    printf("===========================\n");
    printf("Input:  %s\n", argv[1]);
    printf("Output: %s\n\n", argv[2]);

    // Read input assembly file
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

    // Open output file
    FILE* out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create %s\n", argv[2]);
        free(src);
        return 1;
    }

    // Simple optimization: remove redundant instructions
    int optimizations_found = 0;
    char* line = strtok(src, "\n");

    fprintf(out, "; MLP Optimization Pass - Optimized Assembly\n\n");

    while (line != NULL) {
        // Skip redundant instructions
        if (strstr(line, "mov rax, rax") != NULL) {
            printf("  ✓ Removed redundant: mov rax, rax\n");
            optimizations_found++;
        } else if (strstr(line, "add rax, 0") != NULL) {
            printf("  ✓ Removed redundant: add rax, 0\n");
            optimizations_found++;
        } else if (strstr(line, "nop") != NULL) {
            printf("  ✓ Removed: nop\n");
            optimizations_found++;
        } else {
            fprintf(out, "%s\n", line);
        }

        line = strtok(NULL, "\n");
    }

    printf("\n✅ Applied %d optimizations\n", optimizations_found);

    // Cleanup
    free(src);
    fclose(out);

    return 0;
}
