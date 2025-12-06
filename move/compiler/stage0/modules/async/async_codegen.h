#ifndef ASYNC_CODEGEN_H
#define ASYNC_CODEGEN_H

#include "async.h"

// Async Programlama Codegen Interface
// x86-64 NASM assembly generation

// Async function için assembly üretir
// - Future allocation
// - State machine setup (suspend/resume points)
// - Async context save/restore
void codegen_async_function(AsyncFunction* func);

// Await expression için assembly üretir
// - Future state check
// - Suspend current task
// - Register await callback
// - Resume when ready
void codegen_await_expr(AwaitExpr* expr);

// Future operations için assembly üretir
// - future_create, future_set_value, future_get_value
// - then/catch/finally callbacks
// - future_all, future_any, future_race
void codegen_future_ops(Future* future);

// Promise operations için assembly üretir
// - promise_create, promise_resolve, promise_reject
// - promise_get_future
void codegen_promise_ops(Promise* promise);

// Task scheduling için assembly üretir
// - task_create, task_run
// - executor_submit
// - event_loop_tick
void codegen_task_schedule(Task* task);

// Async runtime için assembly üretir
// - runtime_init, runtime_spawn
// - runtime_block_on, runtime_shutdown
void codegen_async_runtime(AsyncRuntime* runtime);

#endif // ASYNC_CODEGEN_H
