#include "advanced_numeric.h"
#include <ctype.h>
#include <math.h>

static const char* src = NULL; 
static int p = 0;

NumCtx* num_ctx_create(void) {
    NumCtx* ctx = (NumCtx*)malloc(sizeof(NumCtx));
    memset(ctx, 0, sizeof(NumCtx));
    return ctx;
}

void num_ctx_free(NumCtx* ctx) {
    if (ctx) free(ctx);
}

void num_parser_init(const char* source) { 
    src = source; 
    p = 0; 
}

// Parse advanced numeric types: u8, u16, u32, u64, i8, i16, i32, i64, f32, f64
void num_parse_file(NumCtx* ctx, NumStats* stats) {
    memset(stats, 0, sizeof(NumStats));
    char tok[64];
    
    while (src[p]) {
        // Skip whitespace
        while (src[p] && isspace(src[p])) p++;
        if (!src[p]) break;
        
        // Read token
        int i = 0;
        while ((isalnum(src[p]) || src[p] == '_') && i < 63) {
            tok[i++] = src[p++];
        }
        tok[i] = '\0';
        
        if (strlen(tok) == 0) {
            if (src[p]) p++;
            continue;
        }
        
        // Detect numeric types
        if (tok[0] == 'u' && isdigit(tok[1])) { 
            // Unsigned types: u8, u16, u32, u64, u128
            stats->u_types++; 
            ctx->unsigned_count++;
            
            // Add operation
            if (ctx->op_count < 256) {
                strncpy(ctx->ops[ctx->op_count].name, tok, 63);
                if (strcmp(tok, "u8") == 0) ctx->ops[ctx->op_count].type = NUM_U8;
                else if (strcmp(tok, "u16") == 0) ctx->ops[ctx->op_count].type = NUM_U16;
                else if (strcmp(tok, "u32") == 0) ctx->ops[ctx->op_count].type = NUM_U32;
                else if (strcmp(tok, "u64") == 0) ctx->ops[ctx->op_count].type = NUM_U64;
                else ctx->ops[ctx->op_count].type = NUM_U128;
                ctx->op_count++;
            }
        }
        else if (tok[0] == 'i' && isdigit(tok[1])) { 
            // Signed types: i8, i16, i32, i64, i128
            stats->i_types++; 
            ctx->signed_count++;
            
            if (ctx->op_count < 256) {
                strncpy(ctx->ops[ctx->op_count].name, tok, 63);
                if (strcmp(tok, "i8") == 0) ctx->ops[ctx->op_count].type = NUM_I8;
                else if (strcmp(tok, "i16") == 0) ctx->ops[ctx->op_count].type = NUM_I16;
                else if (strcmp(tok, "i32") == 0) ctx->ops[ctx->op_count].type = NUM_I32;
                else if (strcmp(tok, "i64") == 0) ctx->ops[ctx->op_count].type = NUM_I64;
                else ctx->ops[ctx->op_count].type = NUM_I128;
                ctx->op_count++;
            }
        }
        else if (tok[0] == 'f' && isdigit(tok[1])) { 
            // Float types: f32, f64, f128
            stats->f_types++; 
            ctx->float_count++;
            
            if (ctx->op_count < 256) {
                strncpy(ctx->ops[ctx->op_count].name, tok, 63);
                if (strcmp(tok, "f32") == 0) ctx->ops[ctx->op_count].type = NUM_F32;
                else if (strcmp(tok, "f64") == 0) ctx->ops[ctx->op_count].type = NUM_F64;
                else ctx->ops[ctx->op_count].type = NUM_F128;
                ctx->op_count++;
            }
        }
    }
}
