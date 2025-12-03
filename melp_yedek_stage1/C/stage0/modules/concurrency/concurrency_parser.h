// Concurrency Parser - Header
// Thread ve synchronization yapılarının parsing'i

#ifndef CONCURRENCY_PARSER_H
#define CONCURRENCY_PARSER_H

#include "concurrency.h"

// Parser fonksiyonları (Stage 1 için placeholder)
Thread* parse_thread_declaration(void* tokens);
Mutex* parse_mutex_declaration(void* tokens);
Semaphore* parse_semaphore_declaration(void* tokens);
ConditionVariable* parse_condvar_declaration(void* tokens);
Barrier* parse_barrier_declaration(void* tokens);
RWLock* parse_rwlock_declaration(void* tokens);
ThreadPool* parse_thread_pool_declaration(void* tokens);

// Thread operation parsing
void* parse_thread_create(void* tokens);
void* parse_thread_start(void* tokens);
void* parse_thread_join(void* tokens);
void* parse_thread_detach(void* tokens);

// Synchronization operation parsing
void* parse_lock_operation(void* tokens);
void* parse_unlock_operation(void* tokens);
void* parse_wait_operation(void* tokens);
void* parse_signal_operation(void* tokens);

#endif // CONCURRENCY_PARSER_H
