#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "modules/variable/variable.h"

// Forward declarations
typedef struct Statement Statement;
typedef struct Expression Expression;

typedef struct {
    VarType type;
    char* name;
    char* value;              // Simple literal value (for backward compat)
    Expression* init_expr;    // Complex initialization expression (function call, etc.)
    // Smart storage management
    InternalNumericType internal_num_type;
    InternalStringType internal_str_type;
    StorageLocation storage;
    int has_decimal_point;  // For detecting floats
} Declaration;

typedef struct {
    Declaration** declarations;
    int decl_count;
    int decl_capacity;
    Statement** statements;     // Top-level statements (if, while, for, etc.)
    int stmt_count;
    int stmt_capacity;
    Statement** functions;      // Function definitions
    int func_count;
    int func_capacity;
} Program;

typedef struct {
    Lexer* lexer;
    Token* current_token;
} Parser;

Parser* parser_create(Lexer* lexer);
void parser_free(Parser* parser);
Program* parser_parse(Parser* parser);
void program_free(Program* program);

#endif
