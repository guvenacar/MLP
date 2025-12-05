#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linking.h"

void print_usage(const char* prog) {
    printf("Usage: %s [options] <object-files...>\n", prog);
    printf("Options:\n");
    printf("  -o <file>    Output file name\n");
    printf("  -v           Verbose output\n");
    printf("  -L <path>    Add library search path\n");
    printf("  -l <name>    Link library\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    printf("Linking Module - Standalone Test\n");
    printf("================================\n\n");

    // Create linker context
    LinkerContext* ctx = linker_context_create();
    ctx->verbose = false;
    char* output_file = "a.out";

    // Parse arguments
    int obj_file_count = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (strcmp(argv[i], "-v") == 0) {
            ctx->verbose = true;
        } else if (strcmp(argv[i], "-L") == 0 && i + 1 < argc) {
            // TODO: Add library search path
            i++;
        } else if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
            // TODO: Link library
            i++;
        } else if (argv[i][0] != '-') {
            // This is an object file
            printf("Loading object file: %s\n", argv[i]);
            if (linker_load_object(ctx, argv[i])) {
                obj_file_count++;
            } else {
                fprintf(stderr, "Failed to load object file: %s\n", argv[i]);
            }
        }
    }

    if (obj_file_count == 0) {
        fprintf(stderr, "No object files loaded\n");
        linker_context_destroy(ctx);
        return 1;
    }

    printf("\nLoaded %d object file(s)\n", obj_file_count);

    // Print symbols if verbose
    if (ctx->verbose) {
        linker_print_symbols(ctx);
        linker_print_relocations(ctx);
    }

    // Link
    printf("\nLinking...\n");
    if (linker_generate_executable(ctx, output_file)) {
        printf("Successfully created executable: %s\n", output_file);
    } else {
        fprintf(stderr, "Linking failed\n");
        linker_context_destroy(ctx);
        return 1;
    }

    // Print statistics
    linker_print_stats(ctx);

    printf("\nLinker features to implement:\n");
    printf("  1. Object file parsing (.o, .s)\n");
    printf("  2. Symbol table management\n");
    printf("  3. Symbol resolution\n");
    printf("  4. Relocation application\n");
    printf("  5. Section layout\n");
    printf("  6. ELF executable generation\n");
    printf("  7. Archive (.a) support\n");
    printf("  8. Dynamic linking\n");
    printf("  9. Library search paths\n");

    // Cleanup
    linker_context_destroy(ctx);

    return 0;
}
