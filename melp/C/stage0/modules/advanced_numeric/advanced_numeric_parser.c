#include "advanced_numeric.h"
#include <ctype.h>
static const char* src = NULL; static int p = 0;
void num_parser_init(const char* source) { src = source; p = 0; }
void num_parse_file(NumCtx* ctx, NumStats* stats) {
    memset(stats, 0, sizeof(NumStats));
    char tok[64];
    while (src[p]) {
        while (src[p] && isspace(src[p])) p++;
        int i = 0;
        while (isalnum(src[p]) || src[p] == '_') tok[i++] = src[p++];
        tok[i] = '\0';
        if (tok[0] == 'u' && isdigit(tok[1])) { stats->u_types++; ctx->unsigned_count++; }
        else if (tok[0] == 'i' && isdigit(tok[1])) { stats->i_types++; ctx->signed_count++; }
        else if (tok[0] == 'f' && isdigit(tok[1])) { stats->f_types++; ctx->float_count++; }
        else if (src[p]) p++;
    }
}
