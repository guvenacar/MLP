#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <curl/curl.h>
#include "gc.h"  // Phase 9: Garbage Collection

// Forward declarations
void* promise_all_thread(void* arg);
void* promise_race_poll_thread(void* arg);
void* promise_any_poll_thread(void* arg);
void* promise_allSettled_poll_thread(void* arg);
void* timeout_timer_thread(void* arg);
void* timeout_monitor_thread(void* arg);

// ========== Phase 8: Async/Await - Promise Implementation ==========

// Promise states
typedef enum {
    PROMISE_PENDING = 0,
    PROMISE_RESOLVED = 1,
    PROMISE_REJECTED = 2
} PromiseState;

// Promise structure
typedef struct Promise {
    PromiseState state;
    void* value;              // Resolved value
    char* error;              // Rejection reason
    void (**callbacks)(void*); // Array of continuation callbacks
    int callback_count;
    int callback_capacity;
    void* user_data;          // PHASE 8.7: User data (e.g., AsyncState pointer)
} Promise;

// Phase 8.5: Async State Machine
// Holds state for resuming async function execution
typedef struct AsyncState {
    int state_number;          // Current state (0 = start, 1+ = after await points)
    Promise* result_promise;   // Promise this async function will resolve
    void* local_vars;          // Heap-allocated local variables
    size_t local_vars_size;    // Size of local variables struct
    void (*resume_fn)(struct AsyncState*, void*); // Resume function pointer
} AsyncState;

// Create async state for state machine
AsyncState* async_state_create(int initial_state, Promise* result_promise, 
                                void* local_vars, size_t vars_size,
                                void (*resume_fn)(AsyncState*, void*)) {
    AsyncState* state = (AsyncState*)malloc(sizeof(AsyncState));
    state->state_number = initial_state;
    state->result_promise = result_promise;
    state->local_vars = local_vars;
    state->local_vars_size = vars_size;
    state->resume_fn = resume_fn;
    return state;
}

// Free async state
void async_state_free(AsyncState* state) {
    if (state->local_vars) {
        free(state->local_vars);
    }
    free(state);
}

// Phase 8.5: Continuation wrapper
// This is registered as a callback with promise_then()
// When promise resolves, this resumes the async function
void async_continuation_wrapper(void* state_ptr) {
    AsyncState* state = (AsyncState*)state_ptr;
    
    // Resume the async function at the saved state
    if (state->resume_fn) {
        // The resume function will handle state dispatch
        state->resume_fn(state, NULL);
    }
    
    // Don't free state here - it will be freed when async function completes
}

// Create a new promise (PENDING state)
Promise* promise_create() {
    Promise* p = (Promise*)gc_malloc(sizeof(Promise), GC_TYPE_PROMISE);
    if (!p) return NULL;
    p->state = PROMISE_PENDING;
    p->value = NULL;
    p->error = NULL;
    p->callbacks = NULL;
    p->callback_count = 0;
    p->callback_capacity = 0;
    p->user_data = NULL;  // PHASE 8.7: Initialize user_data
    return p;
}

// Resolve a promise with a value
void promise_resolve(Promise* p, void* value) {
    if (p->state != PROMISE_PENDING) {
        return;  // Already resolved/rejected
    }
    
    p->state = PROMISE_RESOLVED;
    p->value = value;
    
    // Execute all registered callbacks
    for (int i = 0; i < p->callback_count; i++) {
        if (p->callbacks[i]) {
            p->callbacks[i](value);
        }
    }
    
    // Clear callbacks after execution
    free(p->callbacks);
    p->callbacks = NULL;
    p->callback_count = 0;
    p->callback_capacity = 0;
}

// Reject a promise with an error
void promise_reject(Promise* p, const char* error) {
    if (p->state != PROMISE_PENDING) {
        return;  // Already resolved/rejected
    }
    
    p->state = PROMISE_REJECTED;
    p->error = gc_strdup(error);  // Copy error message (GC-managed)
    
    // TODO: Execute error callbacks (future enhancement)
}

// Register a callback to be called when promise resolves
void promise_then(Promise* p, void (*callback)(void*)) {
    if (p->state == PROMISE_RESOLVED) {
        // Already resolved - execute callback immediately
        callback(p->value);
        return;
    }
    
    if (p->state == PROMISE_REJECTED) {
        // Already rejected - skip callback
        return;
    }
    
    // PENDING - add to callback list
    if (p->callback_count >= p->callback_capacity) {
        // Grow callback array
        int new_capacity = p->callback_capacity == 0 ? 4 : p->callback_capacity * 2;
        void (**new_callbacks)(void*) = (void(**)(void*))gc_realloc(p->callbacks, 
                                                                     new_capacity * sizeof(void(*)(void*)),
                                                                     GC_TYPE_GENERIC);
        if (!new_callbacks) return;  // Allocation failed
        p->callbacks = new_callbacks;
        p->callback_capacity = new_capacity;
    }
    
    p->callbacks[p->callback_count++] = callback;
}

// Check if promise is resolved
int promise_is_resolved(Promise* p) {
    return p->state == PROMISE_RESOLVED ? 1 : 0;
}

// Check if promise is rejected
int promise_is_rejected(Promise* p) {
    return p->state == PROMISE_REJECTED ? 1 : 0;
}

// Get promise value (blocks if pending - for testing only)
void* promise_get_value(Promise* p) {
    if (p->state == PROMISE_RESOLVED) {
        return p->value;
    }
    return NULL;
}

// Free promise memory
// NOTE (Phase 9): This function is now DEPRECATED
// Promises are managed by the garbage collector
// Keeping this for backward compatibility but it's a no-op
void promise_free(Promise* p) {
    // GC-managed - no manual free needed
    // The garbage collector will automatically free:
    // - Promise struct (gc_malloc)
    // - callbacks array (if we make it GC-managed)
    // - error string (gc_strdup)
    (void)p;  // Suppress unused parameter warning
}

// Phase 8.8: Promise.all() implementation
// Structure to track promise_all state
typedef struct {
    Promise* result_promise;     // The promise we'll resolve when all are done
    void** results;              // Array to store individual results
    int* resolved_flags;         // Track which promises resolved
    int total_count;             // Total number of promises
    int resolved_count;          // How many have resolved so far
    pthread_mutex_t mutex;       // Thread safety
} PromiseAllState;

// Callback for each promise in promise_all
void promise_all_callback(void* result) {
    PromiseAllState* state = (PromiseAllState*)result;
    
    // This is called when a promise resolves, but we need to know WHICH promise
    // For now, we'll use a different approach with polling
}

// promise_all: Wait for all promises to resolve
// Takes array of promises, returns promise that resolves with array of results
Promise* promise_all(void** promises, int count) {
    Promise* result_promise = promise_create();
    
    if (count == 0) {
        // Empty array - resolve immediately with empty result
        promise_resolve(result_promise, NULL);
        return result_promise;
    }
    
    // Allocate state structure
    PromiseAllState* state = malloc(sizeof(PromiseAllState));
    state->result_promise = result_promise;
    state->results = malloc(sizeof(void*) * count);
    state->resolved_flags = calloc(count, sizeof(int));
    state->total_count = count;
    state->resolved_count = 0;
    pthread_mutex_init(&state->mutex, NULL);
    
    // Create a thread to poll all promises
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    // Copy promises array for thread
    void** promises_copy = malloc(sizeof(void*) * count);
    memcpy(promises_copy, promises, sizeof(void*) * count);
    
    // Create monitoring thread
    typedef struct {
        Promise** promises;
        int count;
        Promise* result_promise;
    } PromiseAllThreadArgs;
    
    PromiseAllThreadArgs* args = malloc(sizeof(PromiseAllThreadArgs));
    args->promises = (Promise**)promises_copy;
    args->count = count;
    args->result_promise = result_promise;
    
    // Thread function defined inline (will be moved to proper function)
    void* (*thread_func)(void*) = (void*)promise_all_thread;
    pthread_create(&thread, &attr, thread_func, args);
    
    pthread_attr_destroy(&attr);
    free(state->results);
    free(state->resolved_flags);
    pthread_mutex_destroy(&state->mutex);
    free(state);
    
    return result_promise;
}

// Thread function for promise_all polling
void* promise_all_thread(void* arg) {
    typedef struct {
        Promise** promises;
        int count;
        Promise* result_promise;
    } PromiseAllThreadArgs;
    
    PromiseAllThreadArgs* args = (PromiseAllThreadArgs*)arg;
    Promise** promises = args->promises;
    int count = args->count;
    Promise* result_promise = args->result_promise;
    
    // Allocate result array
    void** results = malloc(sizeof(void*) * count);
    
    // Poll until all promises are resolved
    while (1) {
        int all_resolved = 1;
        int any_rejected = 0;
        
        for (int i = 0; i < count; i++) {
            Promise* p = promises[i];
            
            if (p->state == PROMISE_REJECTED) {
                any_rejected = 1;
                promise_reject(result_promise, p->error ? p->error : "Promise rejected");
                break;
            }
            
            if (p->state == PROMISE_RESOLVED) {
                results[i] = p->value;
            } else {
                all_resolved = 0;
            }
        }
        
        if (any_rejected) {
            free(results);
            free(promises);
            free(args);
            return NULL;
        }
        
        if (all_resolved) {
            // All promises resolved - resolve result promise with array
            promise_resolve(result_promise, results);
            free(promises);
            free(args);
            return NULL;
        }
        
        // Sleep a bit before next check
        usleep(1000); // 1ms
    }
    
    return NULL;
}

// ========== Phase 8.3: Event Loop Implementation ==========

// Task structure for event loop
typedef struct Task {
    void (*function)(void*);  // Task function pointer
    void* argument;           // Task argument
    struct Task* next;        // Next task in queue
} Task;

// Event loop structure
typedef struct EventLoop {
    Task* head;               // Queue head
    Task* tail;               // Queue tail
    int is_running;           // Loop state
    int task_count;           // Number of tasks in queue
} EventLoop;

// Global event loop instance
EventLoop* global_event_loop = NULL;

// ========== Phase 8.6: Pending Async Operations ==========

// Pending async operation (awaiting promise resolution)
typedef struct PendingAsync {
    Promise* promise;              // Promise being awaited
    void (*resume_fn)(void*, int); // Function to resume (func_ptr, state)
    void* context;                 // Context pointer (for stack restoration)
    int state_number;              // State to resume from
    struct PendingAsync* next;     // Next pending operation
} PendingAsync;

// Global list of pending async operations
PendingAsync* pending_async_list = NULL;

// Register an async operation that's waiting on a promise
void async_register_pending(Promise* promise, void (*resume_fn)(void*, int), 
                            void* context, int state) {
    PendingAsync* pending = (PendingAsync*)malloc(sizeof(PendingAsync));
    pending->promise = promise;
    pending->resume_fn = resume_fn;
    pending->context = context;
    pending->state_number = state;
    pending->next = pending_async_list;
    pending_async_list = pending;
}

// Check pending promises and resume resolved ones
void async_check_pending() {
    PendingAsync** current = &pending_async_list;
    
    while (*current) {
        PendingAsync* pending = *current;
        
        if (promise_is_resolved(pending->promise)) {
            // Promise resolved - resume the async function
            void* value = promise_get_value(pending->promise);
            
            // Resume at the saved state
            // Pass value in standard way (will be in RDI register)
            pending->resume_fn(value, pending->state_number);
            
            // Remove from list
            *current = pending->next;
            free(pending);
        } else {
            current = &(pending->next);
        }
    }
}

// Create event loop
EventLoop* event_loop_create() {
    EventLoop* loop = (EventLoop*)malloc(sizeof(EventLoop));
    loop->head = NULL;
    loop->tail = NULL;
    loop->is_running = 0;
    loop->task_count = 0;
    return loop;
}

// Push task to event loop queue
void event_loop_push_task(EventLoop* loop, void (*fn)(void*), void* arg) {
    Task* task = (Task*)malloc(sizeof(Task));
    task->function = fn;
    task->argument = arg;
    task->next = NULL;
    
    if (loop->tail) {
        loop->tail->next = task;
        loop->tail = task;
    } else {
        loop->head = task;
        loop->tail = task;
    }
    
    loop->task_count++;
}

// Pop task from event loop queue
Task* event_loop_pop_task(EventLoop* loop) {
    if (!loop->head) {
        return NULL;
    }
    
    Task* task = loop->head;
    loop->head = task->next;
    
    if (!loop->head) {
        loop->tail = NULL;
    }
    
    loop->task_count--;
    return task;
}

// Run event loop until all tasks complete
void event_loop_run(EventLoop* loop) {
    loop->is_running = 1;
    
    while (loop->is_running && (loop->head || pending_async_list)) {
        // Phase 8.6: Check pending async operations first
        async_check_pending();
        
        // Then execute queued tasks
        Task* task = event_loop_pop_task(loop);
        
        if (task) {
            // Execute task
            task->function(task->argument);
            free(task);
        }
        
        // If no tasks and no pending async, we're done
        if (!loop->head && !pending_async_list) {
            break;
        }
    }
    
    loop->is_running = 0;
}

// Stop event loop
void event_loop_stop(EventLoop* loop) {
    loop->is_running = 0;
}

// Get or create global event loop
EventLoop* get_event_loop() {
    if (!global_event_loop) {
        global_event_loop = event_loop_create();
    }
    return global_event_loop;
}

// Helper: Schedule a callback on the event loop
void schedule_callback(void (*callback)(void*), void* arg) {
    EventLoop* loop = get_event_loop();
    event_loop_push_task(loop, callback, arg);
}

// ========== Phase 8.5: Async I/O Simulation ==========

// Async sleep callback structure
typedef struct AsyncSleepData {
    Promise* promise;
    long milliseconds;
} AsyncSleepData;

// Simulate async sleep (immediate resolution for now)
Promise* mlp_async_sleep(long milliseconds) {
    Promise* p = promise_create();
    
    // For now, resolve immediately (real implementation would use timers)
    // TODO: Add actual delay mechanism in future
    promise_resolve(p, NULL);
    
    return p;
}

// Async file read (simulated)
Promise* mlp_async_read(const char* path) {
    Promise* p = promise_create();
    
    // Simulate async read by scheduling on event loop
    FILE* file = fopen(path, "r");
    if (!file) {
        promise_reject(p, "File not found");
        return p;
    }
    
    // Read file content
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = (char*)malloc(size + 1);
    fread(content, 1, size, file);
    content[size] = '\0';
    fclose(file);
    
    // Resolve promise with content
    promise_resolve(p, content);
    
    return p;
}

// Async file write (simulated)
Promise* mlp_async_write(const char* path, const char* content) {
    Promise* p = promise_create();
    
    FILE* file = fopen(path, "w");
    if (!file) {
        promise_reject(p, "Failed to write file");
        return p;
    }
    
    fprintf(file, "%s", content);
    fclose(file);
    
    promise_resolve(p, NULL);
    
    return p;
}

// ========== Type conversion utilities ==========
char* int_to_string(long num) {
    // Allocate buffer for string (max 20 digits for 64-bit int + sign + null)
    char* buffer = (char*)malloc(21);
    sprintf(buffer, "%ld", num);
    return buffer;
}

long string_to_int(const char* str) {
    if (str == NULL) return 0;
    return atol(str);  // Convert string to long integer
}

// Aliases for convenience
char* str(long num) {
    return int_to_string(num);
}

long num(const char* s) {
    return string_to_int(s);
}

// Stub implementations for missing runtime functions
void tyd_substr() {}
void tyd_strcat() {}
void dosya_ac() {}
void dosya_oku() {}
void dosya_yaz() {}
void dosya_kapat() {}
void string_karakter_al() {}
void string_alt() {}
void karakter_kodu() {}
void kodu_karaktere() {}
void runtime_dizin_al() {}
void tyd_fix_cwd() {}
void string_birlestir() {}
void string_karsilastir() {}
void string_uzunluk() {}
void string_esit_mi() {}
void string_length() {}
void string_substring() {}
// Phase 7.9+: String concatenation
char* string_concat(const char* str1, const char* str2) {
    if (!str1) str1 = "";
    if (!str2) str2 = "";
    
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char* result = malloc(len1 + len2 + 1);
    
    if (result) {
        strcpy(result, str1);
        strcat(result, str2);
    }
    
    return result;
}
void string_char_at() {}
void char_code() {}

// ========== Phase 8.8: Async I/O Primitives ==========

// Thread argument structure for async_sleep
typedef struct {
    Promise* promise;
    int milliseconds;
} SleepThreadArgs;

// Thread function for async_sleep
void* async_sleep_thread(void* arg) {
    SleepThreadArgs* args = (SleepThreadArgs*)arg;
    
    // Sleep for specified milliseconds
    usleep(args->milliseconds * 1000);
    
    // Resolve promise with NULL (sleep doesn't return a value)
    promise_resolve(args->promise, NULL);
    
    // Cleanup
    free(args);
    return NULL;
}

// async_sleep: Non-blocking sleep
// Returns a promise that resolves after specified milliseconds
Promise* async_sleep(int milliseconds) {
    Promise* promise = promise_create();
    
    // Create thread arguments
    SleepThreadArgs* args = malloc(sizeof(SleepThreadArgs));
    args->promise = promise;
    args->milliseconds = milliseconds;
    
    // Create detached thread to handle sleep
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    if (pthread_create(&thread, &attr, async_sleep_thread, args) != 0) {
        // Thread creation failed
        promise_reject(promise, "Failed to create sleep thread");
        free(args);
    }
    
    pthread_attr_destroy(&attr);
    return promise;
}

// Thread argument structure for async_read_file
typedef struct {
    Promise* promise;
    char* filepath;
} ReadFileThreadArgs;

// Thread function for async_read_file
void* async_read_file_thread(void* arg) {
    ReadFileThreadArgs* args = (ReadFileThreadArgs*)arg;
    
    FILE* file = fopen(args->filepath, "r");
    if (!file) {
        promise_reject(args->promise, "Failed to open file");
        free(args->filepath);
        free(args);
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Read file content
    char* content = malloc(size + 1);
    if (!content) {
        promise_reject(args->promise, "Failed to allocate memory");
        fclose(file);
        free(args->filepath);
        free(args);
        return NULL;
    }
    
    size_t read_size = fread(content, 1, size, file);
    content[read_size] = '\0';
    fclose(file);
    
    // Resolve promise with file content
    promise_resolve(args->promise, content);
    
    // Cleanup
    free(args->filepath);
    free(args);
    return NULL;
}

// async_read_file: Non-blocking file read
// Returns a promise that resolves with file content as string
Promise* async_read_file(const char* filepath) {
    Promise* promise = promise_create();
    
    // Create thread arguments
    ReadFileThreadArgs* args = malloc(sizeof(ReadFileThreadArgs));
    args->promise = promise;
    args->filepath = strdup(filepath);
    
    // Create detached thread
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    if (pthread_create(&thread, &attr, async_read_file_thread, args) != 0) {
        promise_reject(promise, "Failed to create read thread");
        free(args->filepath);
        free(args);
    }
    
    pthread_attr_destroy(&attr);
    return promise;
}

// Thread argument structure for async_write_file
typedef struct {
    Promise* promise;
    char* filepath;
    char* content;
} WriteFileThreadArgs;

// Thread function for async_write_file
void* async_write_file_thread(void* arg) {
    WriteFileThreadArgs* args = (WriteFileThreadArgs*)arg;
    
    FILE* file = fopen(args->filepath, "w");
    if (!file) {
        promise_reject(args->promise, "Failed to open file for writing");
        free(args->filepath);
        free(args->content);
        free(args);
        return NULL;
    }
    
    // Write content to file
    size_t len = strlen(args->content);
    size_t written = fwrite(args->content, 1, len, file);
    fclose(file);
    
    if (written != len) {
        promise_reject(args->promise, "Failed to write complete content");
    } else {
        // Resolve with number of bytes written
        promise_resolve(args->promise, (void*)(long)written);
    }
    
    // Cleanup
    free(args->filepath);
    free(args->content);
    free(args);
    return NULL;
}

// async_write_file: Non-blocking file write
// Returns a promise that resolves when write completes
Promise* async_write_file(const char* filepath, const char* content) {
    Promise* promise = promise_create();
    
    // Create thread arguments
    WriteFileThreadArgs* args = malloc(sizeof(WriteFileThreadArgs));
    args->promise = promise;
    args->filepath = strdup(filepath);
    args->content = strdup(content);
    
    // Create detached thread
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    if (pthread_create(&thread, &attr, async_write_file_thread, args) != 0) {
        promise_reject(promise, "Failed to create write thread");
        free(args->filepath);
        free(args->content);
        free(args);
    }
    
    pthread_attr_destroy(&attr);
    return promise;
}

// Memory buffer for curl response
typedef struct {
    char* data;
    size_t size;
} CurlMemoryBuffer;

// Callback for curl to write response data
size_t mlp_curl_write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    CurlMemoryBuffer* mem = (CurlMemoryBuffer*)userp;
    
    char* ptr = realloc(mem->data, mem->size + realsize + 1);
    if (!ptr) {
        return 0;  // Out of memory
    }
    
    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = '\0';
    
    return realsize;
}

// Thread argument structure for async_http_get
typedef struct {
    Promise* promise;
    char* url;
} HttpGetThreadArgs;

// Thread function for async_http_get
void* async_http_get_thread(void* arg) {
    HttpGetThreadArgs* args = (HttpGetThreadArgs*)arg;
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        promise_reject(args->promise, "Failed to initialize CURL");
        free(args->url);
        free(args);
        return NULL;
    }
    
    CurlMemoryBuffer buffer = {NULL, 0};
    
    curl_easy_setopt(curl, CURLOPT_URL, args->url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, mlp_curl_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&buffer);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "MLP-Async-HTTP/1.0");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    
    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        promise_reject(args->promise, (char*)curl_easy_strerror(res));
        free(buffer.data);
    } else {
        promise_resolve(args->promise, buffer.data);
    }
    
    curl_easy_cleanup(curl);
    free(args->url);
    free(args);
    return NULL;
}

// async_http_get: Non-blocking HTTP GET request
// Returns a promise that resolves with response body as string
Promise* async_http_get(const char* url) {
    Promise* promise = promise_create();
    
    // Initialize curl global (thread-safe after first call)
    static int curl_initialized = 0;
    if (!curl_initialized) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_initialized = 1;
    }
    
    // Create thread arguments
    HttpGetThreadArgs* args = malloc(sizeof(HttpGetThreadArgs));
    args->promise = promise;
    args->url = strdup(url);
    
    // Create detached thread
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    if (pthread_create(&thread, &attr, async_http_get_thread, args) != 0) {
        promise_reject(promise, "Failed to create HTTP thread");
        free(args->url);
        free(args);
    }
    
    pthread_attr_destroy(&attr);
    return promise;
}

void code_to_char() {}

// Phase 8.8: Helper to create array of promises for promise_all
// This makes it easier to use from MLP code
Promise* promise_all_simple(Promise* p1, Promise* p2, Promise* p3, int count) {
    void** promises = malloc(sizeof(void*) * count);
    
    if (count >= 1) promises[0] = p1;
    if (count >= 2) promises[1] = p2;
    if (count >= 3) promises[2] = p3;
    
    Promise* result = promise_all(promises, count);
    
    // promises array will be freed by promise_all_thread
    return result;
}

// ========== Phase 8.9: Advanced Async Features ==========

// Enhanced error handling functions

// Reject promise with error message and code
void promise_reject_with_error(Promise* p, const char* error, int error_code) {
    if (!p || p->state != PROMISE_PENDING) {
        return;  // Already settled or invalid
    }
    
    p->state = PROMISE_REJECTED;
    p->error = gc_strdup(error);  // GC-managed error string
    p->value = (void*)(long)error_code;  // Store error code in value field
    
    // Execute error callbacks if any
    for (int i = 0; i < p->callback_count; i++) {
        if (p->callbacks[i]) {
            p->callbacks[i](NULL);  // Pass NULL for errors
        }
    }
    
    // Clear callbacks
    if (p->callbacks) {
        free(p->callbacks);
        p->callbacks = NULL;
    }
    p->callback_count = 0;
    p->callback_capacity = 0;
}

// Check if promise has error
int promise_has_error(Promise* p) {
    return (p && p->state == PROMISE_REJECTED) ? 1 : 0;
}

// Get error message from rejected promise
char* promise_get_error(Promise* p) {
    if (!p || p->state != PROMISE_REJECTED || !p->error) {
        return "";
    }
    return p->error;
}

// Get error code from rejected promise
int promise_get_error_code(Promise* p) {
    if (!p || p->state != PROMISE_REJECTED) {
        return 0;
    }
    return (int)(long)p->value;  // Error code stored in value field
}

// promise_race: Returns first settled promise (success or failure)

typedef struct RaceContext {
    Promise* result;
    Promise** promises;
    int count;
    int settled;
    pthread_mutex_t mutex;
} RaceContext;

void race_callback(void* value) {
    // Note: This simplified version doesn't implement full callback mechanism
    // Full implementation would require promise_then() integration
}

Promise* promise_race(Promise** promises, int count) {
    if (!promises || count <= 0) {
        return NULL;
    }
    
    Promise* result = promise_create();
    RaceContext* ctx = (RaceContext*)malloc(sizeof(RaceContext));
    ctx->result = result;
    ctx->promises = promises;
    ctx->count = count;
    ctx->settled = 0;
    pthread_mutex_init(&ctx->mutex, NULL);
    
    // Simple polling implementation
    // In a real implementation, this would use callbacks
    pthread_t thread;
    pthread_create(&thread, NULL, (void*(*)(void*))promise_race_poll_thread, ctx);
    pthread_detach(thread);
    
    return result;
}

// Helper thread for promise_race
void* promise_race_poll_thread(void* arg) {
    RaceContext* ctx = (RaceContext*)arg;
    
    while (1) {
        pthread_mutex_lock(&ctx->mutex);
        
        if (!ctx->settled) {
            // Check each promise
            for (int i = 0; i < ctx->count; i++) {
                Promise* p = ctx->promises[i];
                
                if (p->state == PROMISE_RESOLVED) {
                    ctx->settled = 1;
                    promise_resolve(ctx->result, p->value);
                    pthread_mutex_unlock(&ctx->mutex);
                    pthread_mutex_destroy(&ctx->mutex);
                    free(ctx);
                    return NULL;
                }
                
                if (p->state == PROMISE_REJECTED) {
                    ctx->settled = 1;
                    promise_reject_with_error(ctx->result, p->error, 
                        promise_get_error_code(p));
                    pthread_mutex_unlock(&ctx->mutex);
                    pthread_mutex_destroy(&ctx->mutex);
                    free(ctx);
                    return NULL;
                }
            }
        }
        
        pthread_mutex_unlock(&ctx->mutex);
        
        if (ctx->settled) {
            break;
        }
        
        usleep(1000);  // 1ms polling interval
    }
    
    return NULL;
}

// Helper for 3 promises
Promise* promise_race_simple(Promise* p1, Promise* p2, Promise* p3) {
    Promise* arr[3] = {p1, p2, p3};
    return promise_race(arr, 3);
}

// promise_any: Returns first successful promise (ignores failures)

typedef struct AnyContext {
    Promise* result;
    Promise** promises;
    int count;
    int success_count;
    int failure_count;
    pthread_mutex_t mutex;
} AnyContext;

void* promise_any_poll_thread(void* arg) {
    AnyContext* ctx = (AnyContext*)arg;
    
    while (1) {
        pthread_mutex_lock(&ctx->mutex);
        
        int completed = 0;
        
        // Check each promise
        for (int i = 0; i < ctx->count; i++) {
            Promise* p = ctx->promises[i];
            
            if (p->state == PROMISE_RESOLVED && ctx->success_count == 0) {
                ctx->success_count = 1;
                promise_resolve(ctx->result, p->value);
                completed = 1;
                break;
            }
            
            if (p->state == PROMISE_REJECTED) {
                ctx->failure_count++;
            }
        }
        
        // All failed?
        if (!completed && ctx->failure_count == ctx->count) {
            promise_reject_with_error(ctx->result, "All promises rejected", -1);
            completed = 1;
        }
        
        pthread_mutex_unlock(&ctx->mutex);
        
        if (completed) {
            pthread_mutex_destroy(&ctx->mutex);
            free(ctx);
            break;
        }
        
        usleep(1000);  // 1ms polling
    }
    
    return NULL;
}

Promise* promise_any(Promise** promises, int count) {
    if (!promises || count <= 0) {
        return NULL;
    }
    
    Promise* result = promise_create();
    AnyContext* ctx = (AnyContext*)malloc(sizeof(AnyContext));
    ctx->result = result;
    ctx->promises = promises;
    ctx->count = count;
    ctx->success_count = 0;
    ctx->failure_count = 0;
    pthread_mutex_init(&ctx->mutex, NULL);
    
    pthread_t thread;
    pthread_create(&thread, NULL, promise_any_poll_thread, ctx);
    pthread_detach(thread);
    
    return result;
}

Promise* promise_any_simple(Promise* p1, Promise* p2, Promise* p3) {
    Promise* arr[3] = {p1, p2, p3};
    return promise_any(arr, 3);
}

// promise_allSettled: Wait for all promises (never rejects)

typedef struct AllSettledContext {
    Promise* result;
    Promise** promises;
    int count;
    int settled_count;
    pthread_mutex_t mutex;
} AllSettledContext;

void* promise_allSettled_poll_thread(void* arg) {
    AllSettledContext* ctx = (AllSettledContext*)arg;
    
    while (1) {
        pthread_mutex_lock(&ctx->mutex);
        
        int count = 0;
        
        // Count settled promises
        for (int i = 0; i < ctx->count; i++) {
            Promise* p = ctx->promises[i];
            if (p->state != PROMISE_PENDING) {
                count++;
            }
        }
        
        // All settled?
        if (count == ctx->count) {
            promise_resolve(ctx->result, (void*)1);  // Always resolve
            pthread_mutex_unlock(&ctx->mutex);
            pthread_mutex_destroy(&ctx->mutex);
            free(ctx);
            break;
        }
        
        pthread_mutex_unlock(&ctx->mutex);
        usleep(1000);  // 1ms polling
    }
    
    return NULL;
}

Promise* promise_allSettled(Promise** promises, int count) {
    if (!promises || count <= 0) {
        return NULL;
    }
    
    Promise* result = promise_create();
    AllSettledContext* ctx = (AllSettledContext*)malloc(sizeof(AllSettledContext));
    ctx->result = result;
    ctx->promises = promises;
    ctx->count = count;
    ctx->settled_count = 0;
    pthread_mutex_init(&ctx->mutex, NULL);
    
    pthread_t thread;
    pthread_create(&thread, NULL, promise_allSettled_poll_thread, ctx);
    pthread_detach(thread);
    
    return result;
}

Promise* promise_allSettled_simple(Promise* p1, Promise* p2, Promise* p3) {
    Promise* arr[3] = {p1, p2, p3};
    return promise_allSettled(arr, 3);
}

// async_timeout: Wrap promise with timeout

typedef struct TimeoutContext {
    Promise* result;
    Promise* original;
    int milliseconds;
    int completed;
    pthread_mutex_t mutex;
} TimeoutContext;

void* timeout_timer_thread(void* arg) {
    TimeoutContext* ctx = (TimeoutContext*)arg;
    
    usleep(ctx->milliseconds * 1000);  // Convert ms to microseconds
    
    pthread_mutex_lock(&ctx->mutex);
    
    if (!ctx->completed) {
        ctx->completed = 1;
        promise_reject_with_error(ctx->result, "Operation timed out", -2);
    }
    
    pthread_mutex_unlock(&ctx->mutex);
    return NULL;
}

void* timeout_monitor_thread(void* arg) {
    TimeoutContext* ctx = (TimeoutContext*)arg;
    
    // Wait for original promise
    while (ctx->original->state == PROMISE_PENDING) {
        usleep(1000);  // 1ms polling
    }
    
    pthread_mutex_lock(&ctx->mutex);
    
    if (!ctx->completed) {
        ctx->completed = 1;
        
        if (promise_has_error(ctx->original)) {
            promise_reject_with_error(ctx->result,
                promise_get_error(ctx->original),
                promise_get_error_code(ctx->original));
        } else {
            promise_resolve(ctx->result, ctx->original->value);
        }
    }
    
    pthread_mutex_unlock(&ctx->mutex);
    pthread_mutex_destroy(&ctx->mutex);
    free(ctx);
    
    return NULL;
}

Promise* async_timeout(Promise* promise, int milliseconds) {
    if (!promise) {
        return NULL;
    }
    
    // Handle immediate timeout (0ms or negative)
    if (milliseconds <= 0) {
        Promise* result = promise_create();
        promise_reject(result, "Operation timed out immediately");
        return result;
    }
    
    Promise* result = promise_create();
    TimeoutContext* ctx = (TimeoutContext*)malloc(sizeof(TimeoutContext));
    ctx->result = result;
    ctx->original = promise;
    ctx->milliseconds = milliseconds;
    ctx->completed = 0;
    pthread_mutex_init(&ctx->mutex, NULL);
    
    // Start timeout timer
    pthread_t timer_thread;
    pthread_create(&timer_thread, NULL, timeout_timer_thread, ctx);
    pthread_detach(timer_thread);
    
    // Start monitor thread
    pthread_t monitor_thread;
    pthread_create(&monitor_thread, NULL, timeout_monitor_thread, ctx);
    pthread_detach(monitor_thread);
    
    return result;
}

// ============================================================================
// PHASE 8.9: Promise API Extension
// Wrapper functions to expose Promise* directly to MLP
// ============================================================================

// Get Promise* from async primitive (for advanced usage)
Promise* promise_from_async(Promise* p) {
    return p;  // Direct passthrough
}

// Create a new pending promise (advanced API)
Promise* promise_new() {
    return promise_create();
}

// Resolve a promise with a numeric value
void promise_resolve_numeric(Promise* p, long value) {
    promise_resolve(p, (void*)value);
}

// Resolve a promise with a string value
void promise_resolve_string(Promise* p, const char* value) {
    char* copied = strdup(value);
    promise_resolve(p, copied);
}

// Check if promise is pending
int promise_is_pending(Promise* p) {
    return p ? (p->state == PROMISE_PENDING ? 1 : 0) : 0;
}

// Get numeric value from resolved promise
long promise_get_numeric_value(Promise* p) {
    if (!p || p->state != PROMISE_RESOLVED) {
        return 0;
    }
    return (long)p->value;
}

// Get string value from resolved promise
const char* promise_get_string_value(Promise* p) {
    if (!p || p->state != PROMISE_RESOLVED) {
        return "";
    }
    return (const char*)p->value;
}

// Async sleep that returns Promise* (for consistency)
Promise* async_sleep_promise(int milliseconds) {
    return async_sleep(milliseconds);
}

// Promise.race with direct Promise* array access
Promise* promise_race_array(Promise** promises, int count) {
    return promise_race(promises, count);
}

// Promise.any with direct Promise* array access
Promise* promise_any_array(Promise** promises, int count) {
    return promise_any(promises, count);
}

// Promise.allSettled with direct Promise* array access
Promise* promise_allSettled_array(Promise** promises, int count) {
    return promise_allSettled(promises, count);
}

// Async timeout with direct Promise* access
Promise* promise_with_timeout(Promise* promise, int milliseconds) {
    return async_timeout(promise, milliseconds);
}

// Manual promise rejection (for testing/debugging)
void promise_reject_manual(Promise* p, const char* reason) {
    promise_reject(p, reason);
}

// Get promise state as integer (0=pending, 1=resolved, 2=rejected)
int promise_get_state(Promise* p) {
    if (!p) return -1;
    return (int)p->state;
}

// Safe await: Wait for promise completion (resolved or rejected)
// Returns: 0 for resolved, -1 for rejected, -2 for invalid/null promise
int promise_await_safe(Promise* p) {
    if (!p) return -2;
    
    // Spin-wait until promise is no longer pending
    while (p->state == PROMISE_PENDING) {
        usleep(1000);  // 1ms polling interval
    }
    
    // Return status based on final state
    if (p->state == PROMISE_RESOLVED) {
        return 0;  // Success
    } else if (p->state == PROMISE_REJECTED) {
        return -1;  // Rejected/Error
    }
    
    return -2;  // Shouldn't happen
}

// Check if promise is completed (resolved OR rejected)
int promise_is_completed(Promise* p) {
    if (!p) return 0;
    return (p->state == PROMISE_RESOLVED || p->state == PROMISE_REJECTED) ? 1 : 0;
}

// Wait for promise and get result (blocking)
// Returns value if resolved, NULL if rejected
void* promise_await_value(Promise* p) {
    if (!p) return NULL;
    
    // Wait for completion
    int status = promise_await_safe(p);
    
    if (status == 0) {
        // Resolved - return value
        return p->value;
    } else {
        // Rejected or error - return NULL
        return NULL;
    }
}
