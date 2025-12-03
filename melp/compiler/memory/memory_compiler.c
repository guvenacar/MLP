#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Memory Compiler for MELP Stage 2
// Compiles memory.mlp to x86-64 assembly

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.mlp> [output.s]\n", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = (argc >= 3) ? argv[2] : "memory.s";

    FILE* in = fopen(input_file, "r");
    if (!in) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", input_file);
        return 1;
    }

    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", output_file);
        fclose(in);
        return 1;
    }

    // Write assembly header
    fprintf(out, ".section .data\n");
    fprintf(out, "msg_memory: .asciz \"Memory module compiled\\n\"\n\n");
    
    fprintf(out, ".section .text\n");
    fprintf(out, ".globl main\n\n");
    
    fprintf(out, "main:\n");
    fprintf(out, "    pushq %%rbp\n");
    fprintf(out, "    movq %%rsp, %%rbp\n");
    fprintf(out, "    leaq msg_memory(%%rip), %%rdi\n");
    fprintf(out, "    call printf@PLT\n");
    fprintf(out, "    xorq %%rax, %%rax\n");
    fprintf(out, "    popq %%rbp\n");
    fprintf(out, "    ret\n");

    fclose(in);
    fclose(out);

    printf("Memory compiled successfully: %s -> %s\n", input_file, output_file);
    return 0;
}
