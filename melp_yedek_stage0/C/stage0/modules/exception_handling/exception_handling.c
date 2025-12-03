#include "exception_handling.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static ExceptionHandler global_handler = NULL;

// Exception oluştur
Exception* exception_create(const char* type, const char* message) {
    Exception* exc = malloc(sizeof(Exception));
    exc->type = strdup(type);
    exc->message = strdup(message);
    exc->file = NULL;
    exc->line = 0;
    exc->data = NULL;
    exc->cause = NULL;
    return exc;
}

// Try-catch-finally oluştur
TryCatchFinally* try_catch_finally_create(void) {
    TryCatchFinally* tcf = malloc(sizeof(TryCatchFinally));
    tcf->try_block = NULL;
    tcf->catch_blocks = NULL;
    tcf->catch_count = 0;
    tcf->finally_block = NULL;
    tcf->jump_buffer = malloc(sizeof(jmp_buf));
    return tcf;
}

// Catch bloğu oluştur
CatchBlock* catch_block_create(const char* type, const char* var_name) {
    CatchBlock* cb = malloc(sizeof(CatchBlock));
    cb->exception_type = strdup(type);
    cb->var_name = var_name ? strdup(var_name) : NULL;
    cb->handler = NULL;
    return cb;
}

// Throw oluştur
Throw* throw_create(Exception* exception) {
    Throw* thr = malloc(sizeof(Throw));
    thr->exception = exception;
    return thr;
}

// Exception context oluştur
ExceptionContext* exception_context_create(void) {
    ExceptionContext* ctx = malloc(sizeof(ExceptionContext));
    ctx->stack_capacity = 16;
    ctx->stack = malloc(sizeof(TryCatchFinally*) * ctx->stack_capacity);
    ctx->stack_size = 0;
    ctx->current = NULL;
    return ctx;
}

// Exception konum bilgisi
void exception_set_location(Exception* exc, const char* file, int line) {
    if (!exc) return;
    exc->file = file ? strdup(file) : NULL;
    exc->line = line;
}

// Exception zinciri (cause)
void exception_set_cause(Exception* exc, Exception* cause) {
    if (!exc) return;
    exc->cause = cause;
}

// Exception yazdır
void exception_print(Exception* exc) {
    if (!exc) return;
    
    fprintf(stderr, "%s: %s\n", exc->type, exc->message);
    
    if (exc->file) {
        fprintf(stderr, "  at %s:%d\n", exc->file, exc->line);
    }
    
    if (exc->cause) {
        fprintf(stderr, "Caused by:\n");
        exception_print(exc->cause);
    }
}

// Try-catch'e catch bloğu ekle
void try_catch_add_catch(TryCatchFinally* tcf, CatchBlock* cb) {
    if (!tcf) return;
    
    tcf->catch_blocks = realloc(tcf->catch_blocks,
                                sizeof(CatchBlock*) * (tcf->catch_count + 1));
    ((CatchBlock**)tcf->catch_blocks)[tcf->catch_count] = cb;
    tcf->catch_count++;
}

// Finally bloğu ayarla
void try_catch_set_finally(TryCatchFinally* tcf, void* finally_block) {
    if (!tcf) return;
    tcf->finally_block = finally_block;
}

// Context'e try-catch push
void exception_context_push(ExceptionContext* ctx, TryCatchFinally* tcf) {
    if (!ctx) return;
    
    if (ctx->stack_size >= ctx->stack_capacity) {
        ctx->stack_capacity *= 2;
        ctx->stack = realloc(ctx->stack,
                            sizeof(TryCatchFinally*) * ctx->stack_capacity);
    }
    
    ctx->stack[ctx->stack_size] = tcf;
    ctx->stack_size++;
}

// Context'ten try-catch pop
TryCatchFinally* exception_context_pop(ExceptionContext* ctx) {
    if (!ctx || ctx->stack_size == 0) return NULL;
    
    ctx->stack_size--;
    return ctx->stack[ctx->stack_size];
}

// Context'teki son try-catch'e bak
TryCatchFinally* exception_context_peek(ExceptionContext* ctx) {
    if (!ctx || ctx->stack_size == 0) return NULL;
    return ctx->stack[ctx->stack_size - 1];
}

// Mevcut exception'ı ayarla
void exception_context_set_current(ExceptionContext* ctx, Exception* exc) {
    if (!ctx) return;
    ctx->current = exc;
}

// Mevcut exception'ı al
Exception* exception_context_get_current(ExceptionContext* ctx) {
    if (!ctx) return NULL;
    return ctx->current;
}

// Global exception handler ayarla
void set_global_exception_handler(ExceptionHandler handler) {
    global_handler = handler;
}

// Global exception handler tetikle
void trigger_global_exception_handler(Exception* exc) {
    if (global_handler) {
        global_handler(exc);
    } else {
        exception_print(exc);
        exit(1);
    }
}

// Bellek temizleme
void exception_free(Exception* exc) {
    if (!exc) return;
    free(exc->type);
    free(exc->message);
    if (exc->file) free(exc->file);
    if (exc->cause) exception_free(exc->cause);
    free(exc);
}

void try_catch_finally_free(TryCatchFinally* tcf) {
    if (!tcf) return;
    
    if (tcf->catch_blocks) {
        for (int i = 0; i < tcf->catch_count; i++) {
            catch_block_free(((CatchBlock**)tcf->catch_blocks)[i]);
        }
        free(tcf->catch_blocks);
    }
    
    if (tcf->jump_buffer) free(tcf->jump_buffer);
    free(tcf);
}

void catch_block_free(CatchBlock* cb) {
    if (!cb) return;
    free(cb->exception_type);
    if (cb->var_name) free(cb->var_name);
    free(cb);
}

void throw_free(Throw* thr) {
    if (!thr) return;
    free(thr);
}

void exception_context_free(ExceptionContext* ctx) {
    if (!ctx) return;
    free(ctx->stack);
    free(ctx);
}
