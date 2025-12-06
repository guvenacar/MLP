#include "performance.h"
#include <ctype.h>
static const char* src = NULL; static int p = 0;
void perf_parser_init(const char* source) { src = source; p = 0; }
void perf_parse_file(PerfCtx* ctx, PerfStats* stats) {
    memset(stats, 0, sizeof(PerfStats));
    char tok[64];
    while (src[p]) {
        while (src[p] && isspace(src[p])) p++;
        int i = 0;
        while (isalnum(src[p]) || src[p] == '_') tok[i++] = src[p++];
        tok[i] = '\0';
        if (strcmp(tok, "inline") == 0) { stats->inlines++; ctx->inline_count++; }
        else if (strcmp(tok, "hot") == 0) { stats->hots++; }
        else if (strcmp(tok, "cold") == 0) { stats->colds++; }
        else if (strcmp(tok, "simd") == 0) { stats->simds++; ctx->simd_count++; }
        else if (src[p]) p++;
    }
}
