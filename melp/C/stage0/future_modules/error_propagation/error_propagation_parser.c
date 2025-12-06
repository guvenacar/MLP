/**
 * MLP Error Propagation Module - Parser #71
 */

#include "error_propagation.h"

void errorprop_parse(const char* input, ErrorPropCtx* ctx) {
    ctx->question_count = 0;
    ctx->try_count = 0;
    
    char* input_copy = strdup(input);
    char* line = strtok(input_copy, "\n");
    
    while (line) {
        while (*line == ' ' || *line == '\t') line++;
        if (*line == '\0' || *line == '#' || strncmp(line, "//", 2) == 0) {
            line = strtok(NULL, "\n");
            continue;
        }
        
        if (strchr(line, '?') && !strstr(line, "try")) {
            parse_question_op(line, ctx);
        } else if (strstr(line, "try {")) {
            parse_try_block(input_copy, ctx);
        }
        
        line = strtok(NULL, "\n");
    }
    
    free(input_copy);
}

void parse_question_op(const char* line, ErrorPropCtx* ctx) {
    QuestionOp* op = &ctx->question_ops[ctx->question_count++];
    
    // Find ? position
    const char* q = strchr(line, '?');
    if (!q) return;
    
    // Determine type
    if (strstr(line, "Result") || strstr(line, "Ok") || strstr(line, "Err")) {
        op->type = QUESTION_RESULT;
    } else if (strstr(line, "Option") || strstr(line, "Some") || strstr(line, "None")) {
        op->type = QUESTION_OPTION;
    } else {
        op->type = QUESTION_CUSTOM;
    }
    
    // Extract expression before ?
    int len = q - line;
    const char* equals = strchr(line, '=');
    if (equals && equals < q) {
        sscanf(line, " %63s =", op->result_var);
        line = equals + 1;
        len = q - line;
    }
    
    strncpy(op->expr, line, len);
    op->expr[len] = '\0';
    
    // Trim
    char* p = op->expr;
    while (*p && (*p == ' ' || *p == '\t')) p++;
    memmove(op->expr, p, strlen(p) + 1);
    
    op->has_early_return = true;
}

void parse_try_block(const char* input, ErrorPropCtx* ctx) {
    TryBlock* block = &ctx->try_blocks[ctx->try_count++];
    
    const char* try_start = strstr(input, "try {");
    if (!try_start) return;
    
    const char* try_end = strstr(try_start, "}");
    if (try_end) {
        int len = try_end - try_start;
        if (len > 1023) len = 1023;
        strncpy(block->body, try_start + 5, len);
        block->body[len] = '\0';
    }
    
    // Check for catch
    const char* catch_start = strstr(try_end, "catch");
    if (catch_start) {
        block->has_catch = true;
        sscanf(catch_start, "catch (%63[^)])", block->catch_var);
    } else {
        block->has_catch = false;
    }
}
