#ifndef MLP_PACKAGE_H
#define MLP_PACKAGE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum { PKG_MOD, PKG_USE, PKG_PUB, PKG_CRATE, PKG_EXTERN_CRATE } PkgOpType;
typedef struct { char name[64]; PkgOpType op_type; } PkgOp;
typedef struct { PkgOp ops[256]; int op_count; } PkgCtx;
typedef struct { int mod_decls; int use_stmts; int pub_items; int crate_refs; } PkgStats;
PkgCtx* pkg_ctx_create(void);
void pkg_ctx_free(PkgCtx* ctx);
void pkg_parser_init(const char* source);
void pkg_parse_file(PkgCtx* ctx, PkgStats* stats);
void pkg_generate_asm(const char* output_file, PkgCtx* ctx);
#endif
