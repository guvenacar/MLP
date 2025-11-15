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

// Stub function declarations
CompilerConfig parse_flags(mlp_array_t* args);
void print_usage(void);
void print_debug(void);
void analyze(AST* ast);
void write_file(char* filename, char* content);

// Module instances
extern LexerModule* lexer;
extern ParserModule* parser;
extern OptimizerModule* optimizer;
extern CodegenModule* codegen;

#endif // MLP_STUBS_H
