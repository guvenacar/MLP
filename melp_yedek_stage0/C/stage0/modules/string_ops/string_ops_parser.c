#include "string_ops_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

StringOpsParser* str_parser_create(Lexer* lexer) {
    StringOpsParser* parser = malloc(sizeof(StringOpsParser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

void str_parser_free(StringOpsParser* parser) {
    if (parser->current_token) token_free(parser->current_token);
    free(parser);
}

void string_op_free(StringOpExpr* op) {
    if (!op) return;
    
    // TODO: Free expression operands
    free(op);
}

// Placeholder: Parse string concatenation
StringOpExpr* parse_string_concat(StringOpsParser* parser) {
    // TODO: Implement in Stage 1
    // Expected: string_expr + string_expr
    fprintf(stderr, "TODO: parse_string_concat not implemented\n");
    return NULL;
}

// Placeholder: Parse string function call
StringOpExpr* parse_string_function(StringOpsParser* parser, const char* func_name) {
    // TODO: Implement in Stage 1
    // Expected: length(s), substr(s, start, len), split(s, delim), etc.
    
    StringOp op = STR_OP_LENGTH;  // default
    
    if (strcmp(func_name, "length") == 0) {
        op = STR_OP_LENGTH;
    } else if (strcmp(func_name, "substr") == 0) {
        op = STR_OP_SUBSTR;
    } else if (strcmp(func_name, "split") == 0) {
        op = STR_OP_SPLIT;
    } else if (strcmp(func_name, "replace") == 0) {
        op = STR_OP_REPLACE;
    } else if (strcmp(func_name, "trim") == 0) {
        op = STR_OP_TRIM;
    } else if (strcmp(func_name, "upper") == 0) {
        op = STR_OP_UPPER;
    } else if (strcmp(func_name, "lower") == 0) {
        op = STR_OP_LOWER;
    } else if (strcmp(func_name, "contains") == 0) {
        op = STR_OP_CONTAINS;
    } else if (strcmp(func_name, "indexof") == 0) {
        op = STR_OP_INDEXOF;
    } else {
        fprintf(stderr, "Unknown string function: %s\n", func_name);
        return NULL;
    }
    
    fprintf(stderr, "TODO: parse_string_function '%s' not implemented\n", func_name);
    return NULL;
}
