// Concurrency Module - Header
// Thread management ve synchronization primitives

#ifndef CONCURRENCY_H
#define CONCURRENCY_H

#include <stddef.h>
#include <stdint.h>

// Thread durumları
typedef enum {
    THREAD_NEW,        // Yeni oluşturuldu
    THREAD_RUNNABLE,   // Çalışmaya hazır
    THREAD_RUNNING,    // Çalışıyor
    THREAD_BLOCKED,    // Bloke oldu
    THREAD_WAITING,    // Bekleme durumunda
    THREAD_TERMINATED  // Sonlandı
} ThreadState;

// Thread öncelik seviyeleri
typedef enum {
    PRIORITY_LOW,
    PRIORITY_NORMAL,
    PRIORITY_HIGH,
    PRIORITY_CRITICAL
} ThreadPriority;

// Lock türleri
typedef enum {
    LOCK_MUTEX,        // Mutual exclusion
    LOCK_SPINLOCK,     // Spin lock
    LOCK_RW_LOCK       // Read-write lock
} LockType;

// Semaphore türleri
typedef enum {
    SEMAPHORE_BINARY,  // Binary semaphore
    SEMAPHORE_COUNTING // Counting semaphore
} SemaphoreType;

// Thread yapısı
typedef struct {
    uint64_t id;
    ThreadState state;
    ThreadPriority priority;
    void* (*function)(void*);  // Thread fonksiyonu
    void* argument;             // Fonksiyon argümanı
    void* return_value;         // Dönüş değeri
    void* stack_base;           // Stack base pointer
    size_t stack_size;          // Stack boyutu
    void* context;              // Thread context
    int is_detached;            // Detached mi?
    int is_joinable;            // Joinable mi?
} Thread;

// Mutex yapısı
typedef struct {
    LockType type;
    Thread* owner;              // Mutex'in sahibi
    int lock_count;             // Recursive lock sayısı
    int is_locked;              // Kilitli mi?
    Thread** waiting_threads;   // Bekleyen thread'ler
    size_t waiting_count;       // Bekleyen sayısı
    size_t waiting_capacity;    // Kapasite
} Mutex;

// Semaphore yapısı
typedef struct {
    SemaphoreType type;
    int value;                  // Mevcut değer
    int max_value;              // Maximum değer
    Thread** waiting_threads;   // Bekleyen thread'ler
    size_t waiting_count;
    size_t waiting_capacity;
} Semaphore;

// Condition variable yapısı
typedef struct {
    Mutex* mutex;               // İlişkili mutex
    Thread** waiting_threads;   // Bekleyen thread'ler
    size_t waiting_count;
    size_t waiting_capacity;
} ConditionVariable;

// Barrier yapısı
typedef struct {
    size_t threshold;           // Barrier eşik değeri
    size_t count;               // Mevcut bekleyen sayısı
    Thread** waiting_threads;   // Bekleyen thread'ler
    size_t waiting_capacity;
    int is_broken;              // Barrier bozuldu mu?
} Barrier;

// Read-write lock yapısı
typedef struct {
    int read_count;             // Okuyucu sayısı
    int write_count;            // Yazıcı sayısı
    Thread* writer;             // Mevcut yazıcı
    Thread** readers;           // Okuyucular
    size_t readers_capacity;
    Thread** waiting_writers;   // Bekleyen yazıcılar
    size_t waiting_writers_count;
    size_t waiting_writers_capacity;
    Thread** waiting_readers;   // Bekleyen okuyucular
    size_t waiting_readers_count;
    size_t waiting_readers_capacity;
} RWLock;

// Thread pool yapısı
typedef struct {
    Thread** threads;           // Worker thread'ler
    size_t thread_count;        // Thread sayısı
    void** task_queue;          // Task queue
    size_t queue_size;
    size_t queue_capacity;
    Mutex* queue_mutex;         // Queue için mutex
    ConditionVariable* queue_cv; // Queue için condition variable
    int is_shutdown;            // Shutdown durumu
} ThreadPool;

// Thread operations
Thread* thread_create(void* (*function)(void*), void* argument);
void thread_free(Thread* thread);
int thread_start(Thread* thread);
int thread_join(Thread* thread, void** return_value);
int thread_detach(Thread* thread);
void thread_exit(void* return_value);
int thread_cancel(Thread* thread);
void thread_yield(void);
int thread_set_priority(Thread* thread, ThreadPriority priority);
ThreadState thread_get_state(Thread* thread);

// Mutex operations
Mutex* mutex_create(void);
void mutex_free(Mutex* mutex);
int mutex_lock(Mutex* mutex);
int mutex_trylock(Mutex* mutex);
int mutex_unlock(Mutex* mutex);
int mutex_is_locked(Mutex* mutex);

// Semaphore operations
Semaphore* semaphore_create(SemaphoreType type, int initial_value, int max_value);
void semaphore_free(Semaphore* semaphore);
int semaphore_wait(Semaphore* semaphore);
int semaphore_trywait(Semaphore* semaphore);
int semaphore_post(Semaphore* semaphore);
int semaphore_get_value(Semaphore* semaphore);

// Condition variable operations
ConditionVariable* condvar_create(Mutex* mutex);
void condvar_free(ConditionVariable* cv);
int condvar_wait(ConditionVariable* cv);
int condvar_signal(ConditionVariable* cv);
int condvar_broadcast(ConditionVariable* cv);

// Barrier operations
Barrier* barrier_create(size_t threshold);
void barrier_free(Barrier* barrier);
int barrier_wait(Barrier* barrier);
int barrier_reset(Barrier* barrier);
int barrier_is_broken(Barrier* barrier);

// Read-write lock operations
RWLock* rwlock_create(void);
void rwlock_free(RWLock* rwlock);
int rwlock_read_lock(RWLock* rwlock);
int rwlock_write_lock(RWLock* rwlock);
int rwlock_try_read_lock(RWLock* rwlock);
int rwlock_try_write_lock(RWLock* rwlock);
int rwlock_read_unlock(RWLock* rwlock);
int rwlock_write_unlock(RWLock* rwlock);

// Thread pool operations
ThreadPool* thread_pool_create(size_t thread_count);
void thread_pool_free(ThreadPool* pool);
int thread_pool_submit(ThreadPool* pool, void* (*function)(void*), void* argument);
int thread_pool_wait(ThreadPool* pool);
int thread_pool_shutdown(ThreadPool* pool);

#endif // CONCURRENCY_H
