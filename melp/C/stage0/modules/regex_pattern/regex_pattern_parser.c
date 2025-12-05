#include "regex_pattern.h"
#include <ctype.h>
static const char* src = NULL; static int p = 0;
void regex_parser_init(const char* source) { src = source; p = 0; }
void regex_parse_file(RegexCtx* ctx, RegexStats* stats) {
    memset(stats, 0, sizeof(RegexStats));
    char tok[64];
    while (src[p]) {
        while (src[p] && isspace(src[p])) p++;
        int i = 0;
        while (isalnum(src[p]) || src[p] == '_') tok[i++] = src[p++];
        tok[i] = '\0';
        if (strcmp(tok, "match") == 0) { stats->matches++; ctx->match_count++; }
        else if (strcmp(tok, "replace") == 0) { stats->replaces++; ctx->replace_count++; }
        else if (strcmp(tok, "regex") == 0) { stats->patterns++; }
        else if (src[p]) p++;
    }
}
