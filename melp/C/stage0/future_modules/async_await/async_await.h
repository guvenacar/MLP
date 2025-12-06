/**
 * MLP Async/Await Module - Header #70 (Stage 1)
 * 
 * Modern async/await syntax support
 * - async fn declarations
 * - await keyword
 * - Future<T> types
 * - Task scheduling
 */

#ifndef ASYNC_AWAIT_H
#define ASYNC_AWAIT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_ASYNC_FNS 100
#define MAX_AWAITS 100
#define MAX_FUTURES 100
#define MAX_TASKS 100

// Async function types
typedef enum {
    ASYNC_FN_DECL,      // async fn name() -> T
    ASYNC_FN_CALL,      // name().await
    ASYNC_BLOCK,        // async { ... }
    ASYNC_CLOSURE,      // async || { ... }
} AsyncFnType;

// Future states
typedef enum {
    FUTURE_PENDING,
    FUTURE_READY,
    FUTURE_CANCELLED,
} FutureState;

// Async function
typedef struct {
    AsyncFnType type;
    char name[64];
    char return_type[64];
    char params[256];
    FutureState state;
} AsyncFn;

// Await expression
typedef struct {
    char expr[256];
    char result_var[64];
    bool has_timeout;
    int timeout_ms;
} AwaitExpr;

// Future type
typedef struct {
    char name[64];
    char inner_type[64];
    FutureState state;
    char value[256];
} Future;

// Task
typedef struct {
    char name[64];
    char async_fn[64];
    int priority;
    bool is_detached;
} Task;

// Context
typedef struct {
    AsyncFn async_fns[MAX_ASYNC_FNS];
    AwaitExpr awaits[MAX_AWAITS];
    Future futures[MAX_FUTURES];
    Task tasks[MAX_TASKS];
    
    int async_fn_count;
    int await_count;
    int future_count;
    int task_count;
} AsyncAwaitCtx;

// Parser functions
void asyncawait_parse(const char* input, AsyncAwaitCtx* ctx);
void parse_async_fn(const char* line, AsyncAwaitCtx* ctx);
void parse_await(const char* line, AsyncAwaitCtx* ctx);
void parse_future(const char* line, AsyncAwaitCtx* ctx);
void parse_task(const char* line, AsyncAwaitCtx* ctx);

// Code generation
void asyncawait_generate_asm(const char* output_file, AsyncAwaitCtx* ctx);

#endif // ASYNC_AWAIT_H
