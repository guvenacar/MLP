/**
 * MLP FFI Module - Parser #43 (Stage 1)
 * 
 * Lexer and parser for Foreign Function Interface constructs
 */

#include "ffi.h"
#include <ctype.h>

static const char* source_code = NULL;
static int pos = 0;
static char current_token[256];

typedef enum {
    TOK_EOF, TOK_IDENT, TOK_STRING, TOK_EXTERN, TOK_UNSAFE, 
    TOK_FN, TOK_STRUCT, TOK_LINK, TOK_REPR, TOK_C,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE, TOK_STAR, TOK_CONST, TOK_MUT
} TokType;

static TokType current_tok_type;

static void skip_whitespace() {
    while (source_code[pos] && isspace(source_code[pos])) pos++;
}

static void next_token() {
    skip_whitespace();
    if (!source_code[pos]) { current_tok_type = TOK_EOF; return; }
    
    if (source_code[pos] == '"') {
        pos++; int i = 0;
        while (source_code[pos] && source_code[pos] != '"' && i < 255)
            current_token[i++] = source_code[pos++];
        current_token[i] = '\0';
        if (source_code[pos] == '"') pos++;
        current_tok_type = TOK_STRING;
        return;
    }
    
    if (source_code[pos] == '*') { strcpy(current_token, "*"); pos++; current_tok_type = TOK_STAR; return; }
    if (source_code[pos] == '(') { strcpy(current_token, "("); pos++; current_tok_type = TOK_LPAREN; return; }
    if (source_code[pos] == ')') { strcpy(current_token, ")"); pos++; current_tok_type = TOK_RPAREN; return; }
    if (source_code[pos] == '{') { strcpy(current_token, "{"); pos++; current_tok_type = TOK_LBRACE; return; }
    if (source_code[pos] == '}') { strcpy(current_token, "}"); pos++; current_tok_type = TOK_RBRACE; return; }
    
    if (isalpha(source_code[pos]) || source_code[pos] == '_') {
        int i = 0;
        while ((isalnum(source_code[pos]) || source_code[pos] == '_') && i < 255)
            current_token[i++] = source_code[pos++];
        current_token[i] = '\0';
        
        if (strcmp(current_token, "extern") == 0) current_tok_type = TOK_EXTERN;
        else if (strcmp(current_token, "unsafe") == 0) current_tok_type = TOK_UNSAFE;
        else if (strcmp(current_token, "fn") == 0) current_tok_type = TOK_FN;
        else if (strcmp(current_token, "struct") == 0) current_tok_type = TOK_STRUCT;
        else if (strcmp(current_token, "const") == 0) current_tok_type = TOK_CONST;
        else if (strcmp(current_token, "mut") == 0) current_tok_type = TOK_MUT;
        else current_tok_type = TOK_IDENT;
        return;
    }
    
    current_token[0] = source_code[pos++];
    current_token[1] = '\0';
    current_tok_type = TOK_IDENT;
}

void ffi_parser_init(const char* source) {
    source_code = source;
    pos = 0;
    next_token();
}

void ffi_parse_file(FFICtx* ctx, FFIParseStats* stats) {
    memset(stats, 0, sizeof(FFIParseStats));
    
    while (current_tok_type != TOK_EOF) {
        if (current_tok_type == TOK_EXTERN) {
            next_token();
            if (strcmp(current_token, "C") == 0 || current_tok_type == TOK_STRING) {
                stats->extern_c_blocks++;
                next_token();
                
                if (current_tok_type == TOK_LBRACE) {
                    next_token();
                    while (current_tok_type != TOK_RBRACE && current_tok_type != TOK_EOF) {
                        if (current_tok_type == TOK_FN) {
                            stats->extern_fns++;
                            next_token();
                            if (current_tok_type == TOK_IDENT) {
                                char fn_name[64];
                                strcpy(fn_name, current_token);
                                ffi_add_extern_fn(ctx, fn_name, "", "void", 0);
                                ffi_add_op(ctx, FFI_EXTERN_FN, fn_name, "", "void", 0);
                                next_token();
                            }
                        } else {
                            next_token();
                        }
                    }
                }
            }
        }
        else if (current_tok_type == TOK_UNSAFE) {
            stats->unsafe_blocks++;
            ctx->unsafe_block_count++;
            ffi_add_op(ctx, FFI_UNSAFE_BLOCK, "unsafe", "", "", 0);
            next_token();
        }
        else if (current_tok_type == TOK_STAR) {
            stats->raw_ptrs++;
            ctx->raw_ptr_count++;
            next_token();
            if (current_tok_type == TOK_CONST) {
                ffi_add_op(ctx, FFI_RAW_PTR_CONST, "*const", "", "", 0);
                next_token();
            } else if (current_tok_type == TOK_MUT) {
                ffi_add_op(ctx, FFI_RAW_PTR_MUT, "*mut", "", "", 0);
                next_token();
            }
        }
        else if (current_tok_type == TOK_IDENT) {
            if (strncmp(current_token, "c_", 2) == 0) {
                stats->c_types++;
                ffi_add_op(ctx, FFI_C_INT, current_token, "", "", 0);
            }
            next_token();
        }
        else {
            next_token();
        }
    }
}
