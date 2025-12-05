#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linking.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input1.s> <input2.s> ... <output.s>\n", argv[0]);
        return 1;
    }

    printf("🔧 Linking Module\n");
    printf("=================\n");

    int num_inputs = argc - 2;
    char* output_file = argv[argc - 1];

    printf("Input files: %d\n", num_inputs);
    printf("Output: %s\n\n", output_file);

    // Open output file
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create %s\n", output_file);
        return 1;
    }

    fprintf(out, "; MLP Linking Module - Merged Assembly\n");
    fprintf(out, "; Target: x86-64 Linux\n\n");

    // First pass: collect all .data sections
    fprintf(out, "section .data\n");
    int data_items = 0;

    for (int i = 1; i < argc - 1; i++) {
        FILE* f = fopen(argv[i], "r");
        if (!f) {
            fprintf(stderr, "Warning: Cannot open %s\n", argv[i]);
            continue;
        }

        printf("  ✓ Processing: %s\n", argv[i]);

        char line[1024];
        int in_data = 0;

        while (fgets(line, sizeof(line), f)) {
            // Check for section markers
            if (strstr(line, "section .data")) {
                in_data = 1;
                continue;
            } else if (strstr(line, "section .text") || strstr(line, "section .bss")) {
                in_data = 0;
                continue;
            }

            // Copy data section lines
            if (in_data && line[0] != ';' && strlen(line) > 1) {
                fprintf(out, "%s", line);
                data_items++;
            }
        }

        fclose(f);
    }

    printf("\n  → Merged %d data items\n\n", data_items);

    // Second pass: collect all .text sections
    fprintf(out, "\nsection .text\n");
    fprintf(out, "    global _start\n");
    fprintf(out, "_start:\n");

    int text_lines = 0;

    for (int i = 1; i < argc - 1; i++) {
        FILE* f = fopen(argv[i], "r");
        if (!f) continue;

        char line[1024];
        int in_text = 0;

        while (fgets(line, sizeof(line), f)) {
            // Check for section markers
            if (strstr(line, "section .text")) {
                in_text = 1;
                continue;
            } else if (strstr(line, "section .data") || strstr(line, "section .bss")) {
                in_text = 0;
                continue;
            }

            // Skip global and _start declarations (we already have them)
            if (strstr(line, "global _start") || strstr(line, "_start:")) {
                continue;
            }

            // Copy text section lines
            if (in_text && line[0] != ';' && strlen(line) > 1) {
                fprintf(out, "%s", line);
                text_lines++;
            }
        }

        fclose(f);
    }

    printf("  → Merged %d text instructions\n", text_lines);

    // Add exit syscall if not present
    fprintf(out, "\n    ; Ensure clean exit\n");
    fprintf(out, "    mov rax, 60\n");
    fprintf(out, "    xor rdi, rdi\n");
    fprintf(out, "    syscall\n");

    fclose(out);

    printf("\n✅ Linking complete: %s\n", output_file);
    return 0;
}
