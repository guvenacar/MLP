#include "test_framework.h"
#include <ctype.h>
static const char* src = NULL; static int p = 0;
void test_parser_init(const char* source) { src = source; p = 0; }
void test_parse_file(TestCtx* ctx, TestStats* stats) {
    memset(stats, 0, sizeof(TestStats));
    char tok[64];
    while (src[p]) {
        while (src[p] && isspace(src[p])) p++;
        int i = 0;
        while (isalnum(src[p]) || src[p] == '_') tok[i++] = src[p++];
        tok[i] = '\0';
        if (strcmp(tok, "test") == 0) { stats->tests++; ctx->test_count++; }
        else if (strcmp(tok, "assert") == 0) { stats->asserts++; ctx->assert_count++; }
        else if (strcmp(tok, "bench") == 0) { stats->benches++; }
        else if (src[p]) p++;
    }
}
