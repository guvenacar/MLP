#ifndef ASYNC_PARSER_H
#define ASYNC_PARSER_H

#include "async.h"

// Async Programlama Parser Interface
// Stage 1'de implement edilecek

// Async function tanımını parse eder
// Örnek: async function fetch_data() -> Future<Data>
AsyncFunction* parse_async_function(void);

// Await expression'ı parse eder
// Örnekler:
// - await future
// - await promise
// - await_all [f1, f2, f3]
// - await_any [f1, f2, f3]
// - await_race [f1, f2, f3]
AwaitExpr* parse_await_expr(void);

// Future.then() chain'i parse eder
// Örnek: future.then(callback).catch(error_handler).finally(cleanup)
void parse_future_chain(Future* future);

// Promise oluşturma ve çözme parse eder
// Örnek: let p = new Promise(); p.resolve(value);
Promise* parse_promise_creation(void);

// Async generator parse eder
// Örnek: async function* generate() { yield value; }
AsyncFunction* parse_async_generator(void);

#endif // ASYNC_PARSER_H
