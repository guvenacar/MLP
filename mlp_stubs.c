// MLP Compiler Stubs - implementation

#include "mlp_stubs.h"
#include <stdio.h>
#include <stdlib.h>

// Stub method implementations
static mlp_array_t* stub_tokenize_file(char* filename) {
    printf("[LEXER] Tokenizing %s (stub)\n", filename);
    return mlp_array_new(0);
}

static AST* stub_parse(mlp_array_t* tokens) {
    printf("[PARSER] Parsing tokens (stub)\n");
    AST* ast = malloc(sizeof(AST));
    ast->dummy = 0;
    return ast;
}

static void stub_analyze(AST* ast) {
    printf("[SEMANTIC] Analyzing AST (stub)\n");
}

static AST* stub_optimize(AST* ast) {
    printf("[OPTIMIZER] Optimizing AST (stub)\n");
    return ast;
}

static char* stub_generate(AST* ast, char* backend) {
    printf("[CODEGEN] Generating %s code (stub)\n", backend);
    return "// Generated code stub\n";
}

// Module instances
LexerModule lexer_instance = { .tokenize_file = stub_tokenize_file };
ParserModule parser_instance = { .parse = stub_parse };
SemanticModule semantic_instance = { .analyze = stub_analyze };
OptimizerModule optimizer_instance = { .optimize = stub_optimize };
CodegenModule codegen_instance = { .generate = stub_generate };

LexerModule* lexer = &lexer_instance;
ParserModule* parser = &parser_instance;
SemanticModule* semantic = &semantic_instance;
OptimizerModule* optimizer = &optimizer_instance;
CodegenModule* codegen = &codegen_instance;

// Note: parse_flags(), print_usage(), print_debug(), analyze(), write_file()
// are implemented in main.mlp, not here
