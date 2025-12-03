// Concurrency Codegen - Header
// Thread ve synchronization için assembly generation

#ifndef CONCURRENCY_CODEGEN_H
#define CONCURRENCY_CODEGEN_H

#include "concurrency.h"

// Thread codegen
void codegen_thread_create(Thread* thread);
void codegen_thread_start(Thread* thread);
void codegen_thread_join(Thread* thread);
void codegen_thread_exit(void);

// Mutex codegen
void codegen_mutex_lock(Mutex* mutex);
void codegen_mutex_unlock(Mutex* mutex);
void codegen_mutex_trylock(Mutex* mutex);

// Semaphore codegen
void codegen_semaphore_wait(Semaphore* semaphore);
void codegen_semaphore_post(Semaphore* semaphore);

// Condition variable codegen
void codegen_condvar_wait(ConditionVariable* cv);
void codegen_condvar_signal(ConditionVariable* cv);
void codegen_condvar_broadcast(ConditionVariable* cv);

// Barrier codegen
void codegen_barrier_wait(Barrier* barrier);

// RWLock codegen
void codegen_rwlock_read_lock(RWLock* rwlock);
void codegen_rwlock_write_lock(RWLock* rwlock);
void codegen_rwlock_read_unlock(RWLock* rwlock);
void codegen_rwlock_write_unlock(RWLock* rwlock);

// Thread pool codegen
void codegen_thread_pool_submit(ThreadPool* pool);

// Atomic operations codegen
void codegen_atomic_increment(void);
void codegen_atomic_decrement(void);
void codegen_atomic_compare_and_swap(void);

#endif // CONCURRENCY_CODEGEN_H
