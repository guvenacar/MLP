/**
 * MLP Result/Option Module - Header #41 (Stage 1)
 * 
 * MODÜLER ZİNCİR: Tüm önceki modüllerle entegre (40 modül)
 * 
 * İçerik:
 * - Result type: Result<T, E>, Ok(value), Err(error)
 * - Option type: Option<T>, Some(value), None
 * - Unwrap operations: unwrap(), expect(msg)
 * - Safe access: unwrap_or(default), unwrap_or_else(fn)
 * - Combinators: map(), and_then(), or_else()
 * - Type checks: is_ok(), is_err(), is_some(), is_none()
 * - Error propagation: ? operator
 */

#ifndef MLP_RESULT_OPTION_H
#define MLP_RESULT_OPTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// RESULT/OPTION OPERATION TYPES
// ============================================================================

typedef enum {
    RESOPT_NONE,
    // Result operations
    RESOPT_RESULT_TYPE,          // Result<T, E> type declaration
    RESOPT_OK,                   // Ok(value)
    RESOPT_ERR,                  // Err(error)
    RESOPT_IS_OK,                // is_ok()
    RESOPT_IS_ERR,               // is_err()
    RESOPT_UNWRAP,               // unwrap()
    RESOPT_EXPECT,               // expect(msg)
    RESOPT_UNWRAP_OR,            // unwrap_or(default)
    RESOPT_UNWRAP_OR_ELSE,       // unwrap_or_else(fn)
    RESOPT_MAP,                  // map(fn)
    RESOPT_AND_THEN,             // and_then(fn)
    RESOPT_OR_ELSE,              // or_else(fn)
    RESOPT_QUESTION_OP,          // ? operator
    // Option operations
    RESOPT_OPTION_TYPE,          // Option<T> type declaration
    RESOPT_SOME,                 // Some(value)
    RESOPT_NONE_VAL,             // None
    RESOPT_IS_SOME,              // is_some()
    RESOPT_IS_NONE,              // is_none()
    RESOPT_UNWRAP_OPT,           // unwrap() for Option
    RESOPT_EXPECT_OPT,           // expect(msg) for Option
    RESOPT_UNWRAP_OR_OPT,        // unwrap_or(default) for Option
    RESOPT_MAP_OPT,              // map(fn) for Option
    RESOPT_AND_THEN_OPT,         // and_then(fn) for Option
    RESOPT_OR_ELSE_OPT           // or_else(fn) for Option
} ResOptOpType;

// ============================================================================
// DATA STRUCTURES
// ============================================================================

typedef struct {
    char name[64];
    ResOptOpType op_type;
    char args[256];
    char type_params[128];  // e.g., "<numeric, text>"
    int line;
} ResOptOp;

typedef struct {
    char name[64];
    char ok_type[64];
    char err_type[64];
} ResultInfo;

typedef struct {
    char name[64];
    char some_type[64];
} OptionInfo;

typedef struct {
    char var_name[64];
    char value[128];
} OkInfo;

typedef struct {
    char var_name[64];
    char error[128];
} ErrInfo;

typedef struct {
    char var_name[64];
    char value[128];
} SomeInfo;

typedef struct {
    ResOptOp ops[512];
    int op_count;
    ResultInfo results[64];
    int result_count;
    OptionInfo options[64];
    int option_count;
    OkInfo oks[64];
    int ok_count;
    ErrInfo errs[64];
    int err_count;
    SomeInfo somes[64];
    int some_count;
    int none_count;
    int unwrap_count;
    int expect_count;
    int unwrap_or_count;
    int map_count;
    int and_then_count;
    int or_else_count;
    int question_op_count;
    int is_ok_count;
    int is_err_count;
    int is_some_count;
    int is_none_count;
} ResOptCtx;

// ============================================================================
// PARSER STATISTICS
// ============================================================================

typedef struct {
    int result_types;
    int option_types;
    int ok_constructors;
    int err_constructors;
    int some_constructors;
    int none_constructors;
    int unwraps;
    int expects;
    int unwrap_ors;
    int unwrap_or_elses;
    int maps;
    int and_thens;
    int or_elses;
    int question_ops;
    int is_oks;
    int is_errs;
    int is_somes;
    int is_nones;
} ResOptParseStats;

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================

// Context management
ResOptCtx* resopt_ctx_create(void);
void resopt_ctx_free(ResOptCtx* ctx);

// Context operations
void resopt_add_op(ResOptCtx* ctx, ResOptOpType type, const char* name, const char* args, const char* type_params, int line);
void resopt_add_result(ResOptCtx* ctx, const char* name, const char* ok_type, const char* err_type);
void resopt_add_option(ResOptCtx* ctx, const char* name, const char* some_type);
void resopt_add_ok(ResOptCtx* ctx, const char* var_name, const char* value);
void resopt_add_err(ResOptCtx* ctx, const char* var_name, const char* error);
void resopt_add_some(ResOptCtx* ctx, const char* var_name, const char* value);

// Parser functions (from result_option_parser.c)
void resopt_parser_init(const char* source);
void resopt_parse_file(ResOptCtx* ctx, ResOptParseStats* stats);

// Codegen functions (from result_option_codegen.c)
void resopt_generate_asm(const char* output_file, ResOptCtx* ctx);

#endif /* MLP_RESULT_OPTION_H */
