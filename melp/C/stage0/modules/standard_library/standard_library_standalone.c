#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "standard_library.h"

// List of common stdlib functions to detect
const char* stdlib_functions[] = {
    "printf", "scanf", "fprintf", "sprintf",
    "malloc", "free", "calloc", "realloc",
    "strlen", "strcmp", "strcpy", "strcat",
    "fopen", "fclose", "fread", "fwrite",
    "sin", "cos", "tan", "sqrt", "pow", "log", "exp",
    NULL
};

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }

    printf("🔧 Standard Library Module\n");
    printf("==========================\n");
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

    // Open output file
    FILE* out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create %s\n", argv[2]);
        free(src);
        return 1;
    }

    // Scan for stdlib function calls
    int function_count = 0;
    int needs_libc = 0;
    int needs_libm = 0;

    fprintf(out, "; MLP Standard Library Module - Generated Assembly\n");
    fprintf(out, "; Target: x86-64 Linux\n\n");

    // Detect which functions are used
    for (int i = 0; stdlib_functions[i] != NULL; i++) {
        if (strstr(src, stdlib_functions[i]) != NULL) {
            printf("  ✓ Found stdlib function: %s\n", stdlib_functions[i]);
            function_count++;

            // Determine which library is needed
            if (strcmp(stdlib_functions[i], "sin") == 0 ||
                strcmp(stdlib_functions[i], "cos") == 0 ||
                strcmp(stdlib_functions[i], "tan") == 0 ||
                strcmp(stdlib_functions[i], "sqrt") == 0 ||
                strcmp(stdlib_functions[i], "pow") == 0 ||
                strcmp(stdlib_functions[i], "log") == 0 ||
                strcmp(stdlib_functions[i], "exp") == 0) {
                needs_libm = 1;
            } else {
                needs_libc = 1;
            }
        }
    }

    printf("\n✅ Found %d stdlib function calls\n", function_count);

    // Generate linking information
    fprintf(out, "; Required libraries:\n");
    if (needs_libc) {
        fprintf(out, ";   -lc (libc - standard C library)\n");
    }
    if (needs_libm) {
        fprintf(out, ";   -lm (libm - math library)\n");
    }
    fprintf(out, "\n");

    fprintf(out, "section .text\n");
    fprintf(out, "    global _start\n");
    fprintf(out, "_start:\n");
    fprintf(out, "    ; Stdlib functions will be linked by external linker\n");
    fprintf(out, "    ; Use: gcc -o program program.o");
    if (needs_libc) fprintf(out, " -lc");
    if (needs_libm) fprintf(out, " -lm");
    fprintf(out, "\n\n");

    // Add exit syscall
    fprintf(out, "    mov rax, 60\n");
    fprintf(out, "    xor rdi, rdi\n");
    fprintf(out, "    syscall\n");

    // Cleanup
    free(src);
    fclose(out);

    return 0;
}
