/**
 * MLP Error Propagation Module - Header #71 (Stage 1)
 * 
 * ? operator for error propagation
 * - result?
 * - option?
 * - try blocks
 */

#ifndef ERROR_PROPAGATION_H
#define ERROR_PROPAGATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_QUESTION_OPS 100
#define MAX_TRY_BLOCKS 50

// Question operator types
typedef enum {
    QUESTION_RESULT,    // Result<T,E>?
    QUESTION_OPTION,    // Option<T>?
    QUESTION_CUSTOM,    // Custom type with ? support
} QuestionType;

// Question operator
typedef struct {
    QuestionType type;
    char expr[256];
    char result_var[64];
    bool has_early_return;
} QuestionOp;

// Try block
typedef struct {
    char body[1024];
    char catch_var[64];
    bool has_catch;
} TryBlock;

// Context
typedef struct {
    QuestionOp question_ops[MAX_QUESTION_OPS];
    TryBlock try_blocks[MAX_TRY_BLOCKS];
    
    int question_count;
    int try_count;
} ErrorPropCtx;

// Parser functions
void errorprop_parse(const char* input, ErrorPropCtx* ctx);
void parse_question_op(const char* line, ErrorPropCtx* ctx);
void parse_try_block(const char* input, ErrorPropCtx* ctx);

// Code generation
void errorprop_generate_asm(const char* output_file, ErrorPropCtx* ctx);

#endif // ERROR_PROPAGATION_H
