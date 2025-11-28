// MELP Phase 0 - Bootstrap Compiler
// Main: Tüm pipeline'ı çalıştırır

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.c"

char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        exit(1);
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = malloc(size + 1);
    fread(content, 1, size, file);
    content[size] = '\0';
    
    fclose(file);
    return content;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    const char* input_file = argv[1];
    const char* output_file = argv[2];
    
    printf("MELP Phase 0 Bootstrap Compiler\n");
    printf("=====================================\n");
    printf("Input:  %s\n", input_file);
    printf("Output: %s\n\n", output_file);
    
    // 1. Read source
    char* source = read_file(input_file);
    
    // 2. Lexer
    printf("[1/3] Lexing...\n");
    Lexer* lexer = lexer_create(source);
    
    // 3. Parser
    printf("[2/3] Parsing...\n");
    Parser* parser = parser_create(lexer);
    AST* ast = parser_parse(parser);
    printf("      Found %d declarations\n", ast->count);
    
    // 4. Codegen
    printf("[3/3] Generating assembly...\n");
    Codegen* codegen = codegen_create(output_file);
    codegen_generate(codegen, ast);
    
    printf("\n✓ Compilation successful!\n");
    printf("  Assembly written to: %s\n", output_file);
    printf("\nTo assemble and link:\n");
    printf("  nasm -f elf64 %s -o output.o\n", output_file);
    printf("  ld output.o -o program\n");
    
    // Cleanup
    codegen_free(codegen);
    ast_free(ast);
    parser_free(parser);
    lexer_free(lexer);
    free(source);
    
    return 0;
}
