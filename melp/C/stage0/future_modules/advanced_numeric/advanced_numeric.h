#ifndef MLP_ADVANCED_NUMERIC_H
#define MLP_ADVANCED_NUMERIC_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum { NUM_U8, NUM_U16, NUM_U32, NUM_U64, NUM_U128, NUM_I8, NUM_I16, NUM_I32, NUM_I64, NUM_I128, NUM_F32, NUM_F64, NUM_F128 } NumType;
typedef struct { char name[64]; NumType type; } NumOp;
typedef struct { NumOp ops[256]; int op_count; int unsigned_count; int signed_count; int float_count; } NumCtx;
typedef struct { int u_types; int i_types; int f_types; } NumStats;
NumCtx* num_ctx_create(void); void num_ctx_free(NumCtx* ctx);
void num_parser_init(const char* source); void num_parse_file(NumCtx* ctx, NumStats* stats);
void num_generate_asm(const char* output_file, NumCtx* ctx);
#endif
