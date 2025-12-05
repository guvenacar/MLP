#include <stdio.h>
#include <stdlib.h>
#include "optimization_pass.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.s> [-O0|-O1|-O2|-O3]\n", argv[0]);
        return 1;
    }

    printf("Optimization Pass Module - Standalone Test\n");
    printf("=========================================\n\n");

    // Parse optimization level
    OptimizationLevel level = OPT_MODERATE; // Default -O2
    if (argc >= 3) {
        if (strcmp(argv[2], "-O0") == 0) level = OPT_NONE;
        else if (strcmp(argv[2], "-O1") == 0) level = OPT_BASIC;
        else if (strcmp(argv[2], "-O2") == 0) level = OPT_MODERATE;
        else if (strcmp(argv[2], "-O3") == 0) level = OPT_AGGRESSIVE;
    }

    printf("Input file: %s\n", argv[1]);
    printf("Optimization level: ");
    switch (level) {
        case OPT_NONE: printf("-O0 (none)\n"); break;
        case OPT_BASIC: printf("-O1 (basic)\n"); break;
        case OPT_MODERATE: printf("-O2 (moderate)\n"); break;
        case OPT_AGGRESSIVE: printf("-O3 (aggressive)\n"); break;
    }
    printf("\n");

    // Create optimization context
    OptContext* ctx = opt_context_create(level);

    // Load assembly file
    ctx->instructions = opt_load_assembly(argv[1]);
    if (!ctx->instructions) {
        fprintf(stderr, "Failed to load assembly file\n");
        opt_context_destroy(ctx);
        return 1;
    }

    // Apply optimizations
    opt_apply_all_passes(ctx);

    // Print statistics
    opt_print_stats(ctx);

    // Write optimized output
    char output_file[256];
    snprintf(output_file, sizeof(output_file), "%s.opt", argv[1]);
    opt_write_assembly(output_file, ctx->instructions);
    printf("\nOptimized output written to: %s\n", output_file);

    printf("\nOptimization techniques to implement:\n");
    printf("  1. Dead code elimination\n");
    printf("  2. Constant folding and propagation\n");
    printf("  3. Common subexpression elimination\n");
    printf("  4. Register allocation\n");
    printf("  5. Peephole optimizations\n");
    printf("  6. Loop unrolling\n");
    printf("  7. Function inlining\n");
    printf("  8. Tail call optimization\n");
    printf("  9. Strength reduction\n");

    // Cleanup
    opt_context_destroy(ctx);

    return 0;
}
