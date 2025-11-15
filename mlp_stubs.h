// MLP Compiler Stubs - for main.mlp compilation
// This provides minimal stubs for the module system

#ifndef MLP_STUBS_H
#define MLP_STUBS_H

#include <stdbool.h>
#include "runtime/runtime.h"

// Struct definitions
typedef struct {
    char* backend;
    bool optimize;
    bool verbose;
    bool dump_ast;
} CompilerConfig;

typedef struct {
    int dummy;
} AST;

// Module stubs with method pointers
typedef struct {
    mlp_array_t* (*tokenize_file)(char* filename);
} LexerModule;

typedef struct {
    AST* (*parse)(mlp_array_t* tokens);
} ParserModule;

typedef struct {
    void (*analyze)(AST* ast);
} SemanticModule;

typedef struct {
    AST* (*optimize)(AST* ast);
} OptimizerModule;

typedef struct {
    char* (*generate)(AST* ast, char* backend);
} CodegenModule;

// Note: parse_flags(), print_usage(), print_debug(), analyze(), write_file()
// are implemented in user code (main.mlp), not stubbed here

// Module instances
extern LexerModule* lexer;
extern ParserModule* parser;
extern OptimizerModule* optimizer;
extern CodegenModule* codegen;

#endif // MLP_STUBS_H
