#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "modules/variable/variable_parser.h"
#include "modules/variable/variable_codegen.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    // Read source file
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open input file\n");
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* source = malloc(size + 1);
    fread(source, 1, size, f);
    source[size] = '\0';
    fclose(f);
    
    // Compile
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    Program* program = parser_parse(parser);
    
    if (program) {
        codegen_generate(program, argv[2]);
        printf("Compilation successful: %s -> %s\n", argv[1], argv[2]);
        program_free(program);
    } else {
        fprintf(stderr, "Compilation failed\n");
        parser_free(parser);
        lexer_free(lexer);
        free(source);
        return 1;
    }
    
    parser_free(parser);
    lexer_free(lexer);
    free(source);
    
    return 0;
}
