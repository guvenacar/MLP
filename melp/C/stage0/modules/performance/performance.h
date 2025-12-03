#ifndef MLP_PERFORMANCE_H
#define MLP_PERFORMANCE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum { PERF_INLINE, PERF_HOT, PERF_COLD, PERF_SIMD, PERF_LIKELY } PerfOpType;
typedef struct { char name[64]; PerfOpType type; } PerfOp;
typedef struct { PerfOp ops[256]; int op_count; int inline_count; int simd_count; } PerfCtx;
typedef struct { int inlines; int hots; int colds; int simds; } PerfStats;
PerfCtx* perf_ctx_create(void); void perf_ctx_free(PerfCtx* ctx);
void perf_parser_init(const char* source); void perf_parse_file(PerfCtx* ctx, PerfStats* stats);
void perf_generate_asm(const char* output_file, PerfCtx* ctx);
#endif
