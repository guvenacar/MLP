#ifndef MLP_DOCUMENTATION_H
#define MLP_DOCUMENTATION_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum { DOC_LINE, DOC_BLOCK, DOC_INNER, DOC_ATTR } DocType;
typedef struct { char text[512]; DocType type; } DocOp;
typedef struct { DocOp ops[256]; int op_count; int line_docs; int block_docs; } DocCtx;
typedef struct { int line_comments; int block_comments; int doc_attrs; } DocStats;
DocCtx* doc_ctx_create(void);
void doc_ctx_free(DocCtx* ctx);
void doc_parser_init(const char* source);
void doc_parse_file(DocCtx* ctx, DocStats* stats);
void doc_generate_asm(const char* output_file, DocCtx* ctx);
#endif
