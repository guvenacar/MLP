#include "package_management.h"
#include <ctype.h>

static const char* src = NULL;
static int p = 0;

static void skip_ws() { while (src[p] && isspace(src[p])) p++; }

void pkg_parser_init(const char* source) { src = source; p = 0; }

void pkg_parse_file(PkgCtx* ctx, PkgStats* stats) {
    memset(stats, 0, sizeof(PkgStats));
    char tok[64];
    
    while (src[p]) {
        skip_ws();
        if (!src[p]) break;
        
        int i = 0;
        while (isalnum(src[p]) || src[p] == '_') tok[i++] = src[p++];
        tok[i] = '\0';
        
        if (strcmp(tok, "mod") == 0) {
            stats->mod_decls++;
            if (ctx->op_count < 256) ctx->ops[ctx->op_count++].op_type = PKG_MOD;
        }
        else if (strcmp(tok, "use") == 0) {
            stats->use_stmts++;
            if (ctx->op_count < 256) ctx->ops[ctx->op_count++].op_type = PKG_USE;
        }
        else if (strcmp(tok, "pub") == 0) {
            stats->pub_items++;
            if (ctx->op_count < 256) ctx->ops[ctx->op_count++].op_type = PKG_PUB;
        }
        else if (strcmp(tok, "crate") == 0) {
            stats->crate_refs++;
            if (ctx->op_count < 256) ctx->ops[ctx->op_count++].op_type = PKG_CRATE;
        }
        else p++;
    }
}
