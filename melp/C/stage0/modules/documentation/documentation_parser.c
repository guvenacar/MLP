#include "documentation.h"
static const char* src = NULL; static int p = 0;
void doc_parser_init(const char* source) { src = source; p = 0; }
void doc_parse_file(DocCtx* ctx, DocStats* stats) {
    memset(stats, 0, sizeof(DocStats));
    while (src[p]) {
        if (src[p] == '/' && src[p+1] == '/' && src[p+2] == '/') {
            stats->line_comments++; ctx->line_docs++;
            p += 3; while (src[p] && src[p] != '\n') p++; continue;
        }
        if (src[p] == '/' && src[p+1] == '*' && src[p+2] == '*') {
            stats->block_comments++; ctx->block_docs++;
            p += 3; while (src[p] && !(src[p] == '*' && src[p+1] == '/')) p++; p += 2; continue;
        }
        p++;
    }
}
