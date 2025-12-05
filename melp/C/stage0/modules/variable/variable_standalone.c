#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"
#include "variable_parser.h"
#include "variable_codegen.h"

int main(int argc, char **argv) {
    if(argc < 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    printf("🔧 Variable Module - Parser & Codegen\n");
    printf("======================================\n");
    printf("Input:  %s\n", argv[1]);
    printf("Output: %s\n\n", argv[2]);
    
    // Read source file
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open %s\n", argv[1]);
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *src = malloc(sz + 1);
    fread(src, 1, sz, f);
    src[sz] = 0;
    fclose(f);
    
    // Create lexer
    Lexer* lexer = lexer_create(src);
    
    // Create parser
    VariableParser* parser = variable_parser_create(lexer);
    
    // Open output file
    FILE *out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create %s\n", argv[2]);
        free(src);
        variable_parser_free(parser);
        lexer_free(lexer);
        return 1;
    }
    
    // Create codegen
    VariableCodegen* codegen = variable_codegen_create(out);
    
    // Parse and generate code for all variable declarations
    int decl_count = 0;
    VariableDeclaration* decl;
    
    fprintf(out, "; MLP Variable Module - Generated Assembly\n");
    fprintf(out, "; Target: x86-64 Linux\n\n");
    
    while ((decl = variable_parse_declaration(parser)) != NULL) {
        printf("  ✓ Parsed variable: %s\n", decl->name);
        
        // Generate code
        variable_codegen_declaration(codegen, decl);
        
        // Free declaration
        if (decl->name) free(decl->name);
        if (decl->value) free(decl->value);
        free(decl);
        
        decl_count++;
    }
    
    printf("\n✅ Generated %d variable declarations\n", decl_count);
    
    // Cleanup
    variable_codegen_free(codegen);
    variable_parser_free(parser);
    lexer_free(lexer);
    free(src);
    fclose(out);
    
    return 0;
}
