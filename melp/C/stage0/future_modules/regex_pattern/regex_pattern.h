#ifndef MLP_REGEX_H
#define MLP_REGEX_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum { REGEX_MATCH, REGEX_REPLACE, REGEX_CAPTURE, REGEX_SPLIT } RegexOpType;
typedef struct { char pattern[256]; RegexOpType type; } RegexOp;
typedef struct { RegexOp ops[256]; int op_count; int match_count; int replace_count; } RegexCtx;
typedef struct { int patterns; int matches; int replaces; } RegexStats;
RegexCtx* regex_ctx_create(void); void regex_ctx_free(RegexCtx* ctx);
void regex_parser_init(const char* source); void regex_parse_file(RegexCtx* ctx, RegexStats* stats);
void regex_generate_asm(const char* output_file, RegexCtx* ctx);
#endif
