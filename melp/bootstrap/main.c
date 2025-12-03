// MELP Bootstrap Compiler - Main Entry Point
// Minimal compiler to bootstrap Stage 1 MELP modules

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

static char* read_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        return NULL;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *content = (char*)malloc(size + 1);
    fread(content, 1, size, f);
    content[size] = '\0';
    fclose(f);
    
    return content;
}

int main(int argc, char **argv) {
    printf("🚀 MELP Bootstrap Compiler v0.1\n");
    printf("================================\n\n");
    
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    const char *input_file = argv[1];
    const char *output_file = argv[2];
    
    printf("[1/4] Reading: %s\n", input_file);
    char *source = read_file(input_file);
    if (!source) return 1;
    
    printf("[2/4] Lexing & Parsing...\n");
    ASTNode *ast = parse(source);
    if (!ast) {
        fprintf(stderr, "Parse error!\n");
        free(source);
        return 1;
    }
    
    printf("[3/4] Generating assembly...\n");
    codegen_init(output_file);
    codegen(ast);
    codegen_close();
    
    printf("[4/4] Complete! Output: %s\n", output_file);
    printf("\n✅ Compilation successful!\n");
    printf("\nNext steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", output_file);
    printf("  ld output.o ../C/runtime/runtime.o -o program\n");
    
    free_ast(ast);
    free(source);
    
    return 0;
}
