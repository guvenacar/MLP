/**
 * MLP Concurrency Module - Standalone Compiler #40 (Stage 1)
 * 
 * MODÜLER ZİNCİR: Tüm önceki modüllerle entegre (39 modül)
 * - Stage 0: 1-37 modül (temel modüller)
 * - Stage 1: preprocessor (#38), reflection (#39)
 * 
 * Bu dosya concurrency modülünün ana giriş noktasıdır.
 * Parser ve Codegen ayrı dosyalarda tanımlanmıştır.
 */

#include "concurrency.h"

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

ConcurrencyCtx* conc_ctx_create(void) {
    ConcurrencyCtx* ctx = calloc(1, sizeof(ConcurrencyCtx));
    return ctx;
}

void conc_ctx_free(ConcurrencyCtx* ctx) {
    if (ctx) free(ctx);
}

void conc_add_op(ConcurrencyCtx* ctx, ConcOpType type, const char* name, const char* args, int line) {
    if (ctx->op_count >= 512) return;
    ConcOp* op = &ctx->ops[ctx->op_count++];
    op->op_type = type;
    op->line = line;
    if (name) {
        strncpy(op->name, name, sizeof(op->name) - 1);
        op->name[sizeof(op->name) - 1] = '\0';
    }
    if (args) {
        strncpy(op->args, args, sizeof(op->args) - 1);
        op->args[sizeof(op->args) - 1] = '\0';
    }
}

void conc_add_thread(ConcurrencyCtx* ctx, const char* name) {
    if (ctx->thread_count >= 64) return;
    strncpy(ctx->threads[ctx->thread_count].name, name, 63);
    ctx->threads[ctx->thread_count].name[63] = '\0';
    ctx->thread_count++;
}

void conc_add_mutex(ConcurrencyCtx* ctx, const char* name) {
    if (ctx->mutex_count >= 32) return;
    strncpy(ctx->mutexes[ctx->mutex_count].name, name, 63);
    ctx->mutexes[ctx->mutex_count].name[63] = '\0';
    ctx->mutex_count++;
}

void conc_add_channel(ConcurrencyCtx* ctx, const char* name, const char* type_param) {
    if (ctx->channel_count >= 32) return;
    ChannelInfo* ch = &ctx->channels[ctx->channel_count++];
    strncpy(ch->name, name, 63);
    ch->name[63] = '\0';
    strncpy(ch->type_param, type_param ? type_param : "any", 31);
    ch->type_param[31] = '\0';
}

void conc_add_atomic(ConcurrencyCtx* ctx, const char* name, const char* type_param, const char* init) {
    if (ctx->atomic_count >= 32) return;
    AtomicInfo* a = &ctx->atomics[ctx->atomic_count++];
    strncpy(a->name, name, 63);
    a->name[63] = '\0';
    strncpy(a->type_param, type_param ? type_param : "numeric", 31);
    a->type_param[31] = '\0';
    strncpy(a->init_value, init ? init : "0", 63);
    a->init_value[63] = '\0';
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv) {
    printf("🔄 Concurrency Module - #40 Stage 1\n");
    printf("=====================================\n");
    printf("📦 Modüler Zincir: 39 modül entegre\n\n");
    
    if (argc < 3) {
        printf("Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    printf("Input: %s\nOutput: %s\n\n", argv[1], argv[2]);
    
    // Read file
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open input file\n");
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* source = malloc(size + 1);
    fread(source, 1, size, f);
    source[size] = '\0';
    fclose(f);
    
    printf("[1/4] ✓ Read %ld bytes\n", size);
    
    // Initialize parser
    conc_parser_init(source);
    printf("[2/4] ✓ Lexer init\n");
    
    // Parse
    ConcurrencyCtx* ctx = conc_ctx_create();
    ConcParseStats stats = {0};
    
    printf("[3/4] Parsing concurrency constructs...\n");
    conc_parse_file(ctx, &stats);
    
    // Print statistics
    printf("\n      ✓ Thread Operations:\n");
    if (stats.spawns > 0) printf("        spawn: %d\n", stats.spawns);
    if (stats.joins > 0) printf("        join: %d\n", stats.joins);
    if (stats.detaches > 0) printf("        detach: %d\n", stats.detaches);
    if (stats.goroutines > 0) printf("        go (goroutine): %d\n", stats.goroutines);
    
    printf("      ✓ Mutex Operations:\n");
    if (stats.mutex_news > 0) printf("        Mutex::new: %d\n", stats.mutex_news);
    if (stats.mutex_locks > 0) printf("        lock: %d\n", stats.mutex_locks);
    if (stats.mutex_unlocks > 0) printf("        unlock: %d\n", stats.mutex_unlocks);
    if (stats.mutex_trylocks > 0) printf("        try_lock: %d\n", stats.mutex_trylocks);
    
    printf("      ✓ Channel Operations:\n");
    if (stats.channel_news > 0) printf("        Channel::new: %d\n", stats.channel_news);
    if (stats.channel_sends > 0) printf("        send: %d\n", stats.channel_sends);
    if (stats.channel_recvs > 0) printf("        recv: %d\n", stats.channel_recvs);
    
    printf("      ✓ Atomic Operations:\n");
    if (stats.atomic_news > 0) printf("        Atomic::new: %d\n", stats.atomic_news);
    if (stats.atomic_loads > 0) printf("        load: %d\n", stats.atomic_loads);
    if (stats.atomic_stores > 0) printf("        store: %d\n", stats.atomic_stores);
    if (stats.atomic_fetch_adds > 0) printf("        fetch_add: %d\n", stats.atomic_fetch_adds);
    if (stats.atomic_cas > 0) printf("        compare_exchange: %d\n", stats.atomic_cas);
    
    printf("      ✓ Sync Primitives:\n");
    if (stats.semaphores > 0) printf("        Semaphore: %d\n", stats.semaphores);
    if (stats.barriers > 0) printf("        Barrier: %d\n", stats.barriers);
    if (stats.condvars > 0) printf("        CondVar: %d\n", stats.condvars);
    
    printf("      ✓ Control:\n");
    if (stats.parallel_fors > 0) printf("        parallel_for: %d\n", stats.parallel_fors);
    if (stats.selects > 0) printf("        select: %d\n", stats.selects);
    
    printf("\n      Summary:\n");
    printf("        Threads: %d\n", ctx->thread_count);
    printf("        Mutexes: %d\n", ctx->mutex_count);
    printf("        Channels: %d\n", ctx->channel_count);
    printf("        Atomics: %d\n", ctx->atomic_count);
    printf("        Operations: %d\n", ctx->op_count);
    
    // Generate code
    printf("\n[4/4] ✓ Code generated\n");
    conc_generate_asm(argv[2], ctx);
    
    printf("\nConcurrency OK!\n");
    
    conc_ctx_free(ctx);
    free(source);
    
    return 0;
}
