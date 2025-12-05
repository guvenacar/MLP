/**
 * MLP Concurrency Module - Header #40 (Stage 1)
 * 
 * MODÜLER ZİNCİR: Tüm önceki modüllerle entegre (39 modül)
 * 
 * İçerik:
 * - Thread management: spawn, join, detach
 * - Go-style goroutines: go keyword
 * - Mutex operations: Mutex, lock, unlock, try_lock
 * - Channel communication: Channel, send, recv
 * - Atomic operations: Atomic, load, store, fetch_add, compare_exchange
 * - Synchronization: Semaphore, Barrier, CondVar
 * - Parallel constructs: parallel_for, select
 */

#ifndef MLP_CONCURRENCY_H
#define MLP_CONCURRENCY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// CONCURRENCY OPERATION TYPES
// ============================================================================

typedef enum {
    CONC_NONE,
    // Thread operations
    CONC_SPAWN,
    CONC_JOIN,
    CONC_DETACH,
    CONC_GO,
    // Mutex operations
    CONC_MUTEX_NEW,
    CONC_MUTEX_LOCK,
    CONC_MUTEX_UNLOCK,
    CONC_MUTEX_TRYLOCK,
    // Channel operations
    CONC_CHANNEL_NEW,
    CONC_CHANNEL_SEND,
    CONC_CHANNEL_RECV,
    CONC_CHANNEL_CLOSE,
    // Atomic operations
    CONC_ATOMIC_NEW,
    CONC_ATOMIC_LOAD,
    CONC_ATOMIC_STORE,
    CONC_ATOMIC_FETCH_ADD,
    CONC_ATOMIC_COMPARE_EXCHANGE,
    // Sync primitives
    CONC_SEMAPHORE_NEW,
    CONC_SEMAPHORE_WAIT,
    CONC_SEMAPHORE_SIGNAL,
    CONC_BARRIER_NEW,
    CONC_BARRIER_WAIT,
    CONC_CONDVAR_NEW,
    CONC_CONDVAR_WAIT,
    CONC_CONDVAR_NOTIFY,
    // Control
    CONC_PARALLEL_FOR,
    CONC_SELECT
} ConcOpType;

// ============================================================================
// DATA STRUCTURES
// ============================================================================

typedef struct {
    char name[64];
    ConcOpType op_type;
    char args[256];
    int line;
} ConcOp;

typedef struct {
    char name[64];
    char type_param[32];
} ChannelInfo;

typedef struct {
    char name[64];
    char type_param[32];
    char init_value[64];
} AtomicInfo;

typedef struct {
    char name[64];
} MutexInfo;

typedef struct {
    char name[64];
    char body[512];
} ThreadInfo;

typedef struct {
    ConcOp ops[512];
    int op_count;
    ThreadInfo threads[64];
    int thread_count;
    MutexInfo mutexes[32];
    int mutex_count;
    ChannelInfo channels[32];
    int channel_count;
    AtomicInfo atomics[32];
    int atomic_count;
    int semaphore_count;
    int barrier_count;
    int condvar_count;
    int parallel_for_count;
    int select_count;
    int goroutine_count;
} ConcurrencyCtx;

// ============================================================================
// PARSER STATISTICS
// ============================================================================

typedef struct {
    int spawns;
    int joins;
    int detaches;
    int goroutines;
    int mutex_news;
    int mutex_locks;
    int mutex_unlocks;
    int mutex_trylocks;
    int channel_news;
    int channel_sends;
    int channel_recvs;
    int atomic_news;
    int atomic_loads;
    int atomic_stores;
    int atomic_fetch_adds;
    int atomic_cas;
    int semaphores;
    int barriers;
    int condvars;
    int parallel_fors;
    int selects;
} ConcParseStats;

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================

// Context management
ConcurrencyCtx* conc_ctx_create(void);
void conc_ctx_free(ConcurrencyCtx* ctx);

// Context operations
void conc_add_op(ConcurrencyCtx* ctx, ConcOpType type, const char* name, const char* args, int line);
void conc_add_thread(ConcurrencyCtx* ctx, const char* name);
void conc_add_mutex(ConcurrencyCtx* ctx, const char* name);
void conc_add_channel(ConcurrencyCtx* ctx, const char* name, const char* type_param);
void conc_add_atomic(ConcurrencyCtx* ctx, const char* name, const char* type_param, const char* init);

// Parser functions (from concurrency_parser.c)
void conc_parser_init(const char* source);
void conc_parse_file(ConcurrencyCtx* ctx, ConcParseStats* stats);

// Codegen functions (from concurrency_codegen.c)
void conc_generate_asm(const char* output_file, ConcurrencyCtx* ctx);

#endif /* MLP_CONCURRENCY_H */
