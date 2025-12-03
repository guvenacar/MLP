#ifndef MLP_TEST_FRAMEWORK_H
#define MLP_TEST_FRAMEWORK_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum { TEST_FN, TEST_ASSERT, TEST_ASSERT_EQ, TEST_BENCH } TestOpType;
typedef struct { char name[64]; TestOpType type; } TestOp;
typedef struct { TestOp ops[256]; int op_count; int test_count; int assert_count; } TestCtx;
typedef struct { int tests; int asserts; int benches; } TestStats;
TestCtx* test_ctx_create(void); void test_ctx_free(TestCtx* ctx);
void test_parser_init(const char* source); void test_parse_file(TestCtx* ctx, TestStats* stats);
void test_generate_asm(const char* output_file, TestCtx* ctx);
#endif
