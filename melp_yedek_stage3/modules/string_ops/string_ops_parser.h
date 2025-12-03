#ifndef STRING_OPS_PARSER_H
#define STRING_OPS_PARSER_H

#include "string_ops.h"
#include "../../lexer.h"

// Parser for string operations
typedef struct {
    Lexer* lexer;
    Token* current_token;
} StringOpsParser;

// Parser functions
StringOpsParser* str_parser_create(Lexer* lexer);
void str_parser_free(StringOpsParser* parser);

// Parse string operations
// Concat: expr + expr (when expr is string)
StringOpExpr* parse_string_concat(StringOpsParser* parser);

// Function-style operations: length(s), substr(s, start, len), etc.
StringOpExpr* parse_string_function(StringOpsParser* parser, const char* func_name);

// Free string operation
void string_op_free(StringOpExpr* op);

#endif
