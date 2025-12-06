#ifndef ASYNC_H
#define ASYNC_H

#include <stddef.h>
#include <stdbool.h>

// Asenkron Programlama - Async/Await Sistemi
// async/await keywords, Future/Promise, async runtime

// Future durumları
typedef enum {
    FUTURE_PENDING = 0,      // Henüz tamamlanmadı
    FUTURE_RUNNING = 1,      // Şu anda çalışıyor
    FUTURE_COMPLETED = 2,    // Başarıyla tamamlandı
    FUTURE_FAILED = 3,       // Hata ile tamamlandı
    FUTURE_CANCELLED = 4     // İptal edildi
} FutureState;

// Promise durumları
typedef enum {
    PROMISE_UNFULFILLED = 0, // Henüz yerine getirilmedi
    PROMISE_FULFILLED = 1,   // Başarıyla yerine getirildi
    PROMISE_REJECTED = 2     // Reddedildi
} PromiseState;

// Async fonksiyon türü
typedef enum {
    ASYNC_FUNCTION = 0,      // Normal async fonksiyon
    ASYNC_GENERATOR = 1,     // Async generator (yield)
    ASYNC_COROUTINE = 2,     // Coroutine (yield from)
    ASYNC_FN_DECL = 3,       // async fn name() -> T (modern syntax)
    ASYNC_BLOCK = 4,         // async { ... }
    ASYNC_CLOSURE = 5        // async || { ... }
} AsyncFunctionType;

// Await operasyonu türü
typedef enum {
    AWAIT_FUTURE = 0,        // Future await
    AWAIT_PROMISE = 1,       // Promise await
    AWAIT_VALUE = 2,         // Direct value await
    AWAIT_ALL = 3,           // Tüm future'ları bekle
    AWAIT_ANY = 4,           // Herhangi birini bekle
    AWAIT_RACE = 5,          // İlk tamamlananı al
    AWAIT_KEYWORD = 6        // .await keyword (modern syntax)
} AwaitType;

// Executor stratejisi
typedef enum {
    EXECUTOR_SINGLE_THREAD = 0,  // Tek thread
    EXECUTOR_THREAD_POOL = 1,    // Thread pool
    EXECUTOR_WORK_STEALING = 2   // Work-stealing scheduler
} ExecutorStrategy;

// Event loop durumu
typedef enum {
    LOOP_IDLE = 0,           // Boşta
    LOOP_RUNNING = 1,        // Çalışıyor
    LOOP_STOPPED = 2         // Durduruldu
} EventLoopState;

// Forward declarations
typedef struct Future Future;
typedef struct Promise Promise;
typedef struct AsyncFunction AsyncFunction;
typedef struct AwaitExpr AwaitExpr;
typedef struct AsyncRuntime AsyncRuntime;
typedef struct Executor Executor;
typedef struct EventLoop EventLoop;
typedef struct Task Task;

// Future - Gelecekteki bir değeri temsil eder
struct Future {
    FutureState state;       // Future durumu
    void* value;             // Sonuç değeri
    void* error;             // Hata bilgisi
    Promise* promise;        // İlişkili promise
    Task* task;              // İlişkili task
    Future** dependencies;   // Bağımlı future'lar
    size_t dep_count;        // Bağımlılık sayısı
    void (*then_callback)(void*);    // then() callback
    void (*catch_callback)(void*);   // catch() callback
    void (*finally_callback)(void*); // finally() callback
};

// Promise - Future'a değer atamak için kullanılır
struct Promise {
    PromiseState state;      // Promise durumu
    Future* future;          // İlişkili future
    void* result;            // Sonuç değeri
    void* error;             // Hata bilgisi
    bool is_resolved;        // Çözüldü mü?
};

// Async Function - async anahtar kelimesi ile tanımlanan fonksiyon
struct AsyncFunction {
    char* name;              // Fonksiyon adı
    AsyncFunctionType type;  // Fonksiyon türü
    void* body;              // Fonksiyon gövdesi (AST)
    size_t param_count;      // Parametre sayısı
    char** param_names;      // Parametre adları
    Future* return_future;   // Dönüş future'ı
    bool is_generator;       // Generator mı?
    void* yield_state;       // Yield state (generator için)
};

// Await Expression - await anahtar kelimesi
struct AwaitExpr {
    AwaitType type;          // Await türü
    void* target;            // Beklenen hedef (Future/Promise/değer)
    Future** targets;        // Çoklu hedefler (await_all, await_any)
    size_t target_count;     // Hedef sayısı
    void* timeout;           // Timeout süresi (opsiyonel)
    bool is_cancellable;     // İptal edilebilir mi?
};

// Task - Executor tarafından çalıştırılacak görev
struct Task {
    size_t id;               // Task ID
    AsyncFunction* function; // Çalıştırılacak fonksiyon
    void** args;             // Fonksiyon argümanları
    size_t arg_count;        // Argüman sayısı
    Future* future;          // Task'ın future'ı
    Task* parent;            // Parent task
    Task** children;         // Child task'lar
    size_t child_count;      // Child sayısı
    int priority;            // Öncelik
    bool is_running;         // Çalışıyor mu?
};

// Executor - Task'ları çalıştırır
struct Executor {
    ExecutorStrategy strategy; // Çalıştırma stratejisi
    Task** task_queue;       // Task kuyruğu
    size_t queue_size;       // Kuyruk boyutu
    size_t queue_capacity;   // Kuyruk kapasitesi
    size_t thread_count;     // Thread sayısı (pool için)
    bool is_running;         // Çalışıyor mu?
    EventLoop* event_loop;   // İlişkili event loop
};

// Event Loop - Async işlemleri koordine eder
struct EventLoop {
    EventLoopState state;    // Loop durumu
    Executor* executor;      // İlişkili executor
    Task** ready_tasks;      // Hazır task'lar
    size_t ready_count;      // Hazır task sayısı
    Task** waiting_tasks;    // Bekleyen task'lar
    size_t waiting_count;    // Bekleyen task sayısı
    size_t tick_count;       // Tick sayısı
    bool should_stop;        // Durmalı mı?
};

// Async Runtime - Tüm async sistemi yönetir
struct AsyncRuntime {
    Executor* executor;      // Executor
    EventLoop* event_loop;   // Event loop
    Future** active_futures; // Aktif future'lar
    size_t future_count;     // Future sayısı
    Promise** active_promises; // Aktif promise'lar
    size_t promise_count;    // Promise sayısı
    size_t next_task_id;     // Sonraki task ID
    bool is_initialized;     // Initialize edildi mi?
};

// Future işlemleri
Future* future_create(void);
void future_free(Future* future);
void future_set_value(Future* future, void* value);
void future_set_error(Future* future, void* error);
bool future_is_ready(Future* future);
void* future_get_value(Future* future);
void future_then(Future* future, void (*callback)(void*));
void future_catch(Future* future, void (*callback)(void*));
void future_finally(Future* future, void (*callback)(void*));
Future* future_all(Future** futures, size_t count);
Future* future_any(Future** futures, size_t count);
Future* future_race(Future** futures, size_t count);

// Promise işlemleri
Promise* promise_create(void);
void promise_free(Promise* promise);
void promise_resolve(Promise* promise, void* value);
void promise_reject(Promise* promise, void* error);
Future* promise_get_future(Promise* promise);

// Async Function işlemleri
AsyncFunction* async_function_create(char* name, AsyncFunctionType type);
void async_function_free(AsyncFunction* func);
Future* async_function_call(AsyncFunction* func, void** args, size_t arg_count);

// Await işlemleri
AwaitExpr* await_expr_create(AwaitType type);
void await_expr_free(AwaitExpr* expr);
void* await_execute(AwaitExpr* expr);

// Task işlemleri
Task* task_create(AsyncFunction* func, void** args, size_t arg_count);
void task_free(Task* task);
void task_run(Task* task);
void task_cancel(Task* task);
void task_add_child(Task* parent, Task* child);

// Executor işlemleri
Executor* executor_create(ExecutorStrategy strategy);
void executor_free(Executor* executor);
void executor_submit(Executor* executor, Task* task);
void executor_run(Executor* executor);
void executor_stop(Executor* executor);

// Event Loop işlemleri
EventLoop* event_loop_create(void);
void event_loop_free(EventLoop* loop);
void event_loop_run(EventLoop* loop);
void event_loop_stop(EventLoop* loop);
void event_loop_add_task(EventLoop* loop, Task* task);
void event_loop_tick(EventLoop* loop);

// Async Runtime işlemleri
AsyncRuntime* async_runtime_create(void);
void async_runtime_free(AsyncRuntime* runtime);
void async_runtime_init(AsyncRuntime* runtime, ExecutorStrategy strategy);
Future* async_runtime_spawn(AsyncRuntime* runtime, AsyncFunction* func, void** args, size_t arg_count);
void async_runtime_block_on(AsyncRuntime* runtime, Future* future);
void async_runtime_shutdown(AsyncRuntime* runtime);

#endif // ASYNC_H
