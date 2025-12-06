#include "generator.h"
#include <stdlib.h>
#include <string.h>

// Generator oluştur
Generator* generator_create(const char* name) {
    Generator* gen = malloc(sizeof(Generator));
    gen->name = strdup(name);
    gen->body = NULL;
    gen->context = gen_context_create();
    gen->yield_count = 0;
    return gen;
}

// Generator context oluştur
GenContext* gen_context_create(void) {
    GenContext* ctx = malloc(sizeof(GenContext));
    ctx->state = GEN_CREATED;
    ctx->resume_point = 0;
    ctx->local_vars = NULL;
    ctx->locals_size = 0;
    ctx->current_value = NULL;
    return ctx;
}

// Yield oluştur
Yield* yield_create(void* value, int yield_id) {
    Yield* yld = malloc(sizeof(Yield));
    yld->value = value;
    yld->yield_id = yield_id;
    return yld;
}

// Generator çağrısı oluştur
GenCall* gen_call_create(Generator* gen, int next_call) {
    GenCall* call = malloc(sizeof(GenCall));
    call->generator = gen;
    call->next_call = next_call;
    return call;
}

// Iterator oluştur
Iterator* iterator_create(Generator* gen) {
    Iterator* iter = malloc(sizeof(Iterator));
    iter->generator = gen;
    iter->has_next = 1;
    iter->current = NULL;
    return iter;
}

// Generator başlat
void generator_start(Generator* gen) {
    if (!gen || !gen->context) return;
    gen->context->state = GEN_RUNNING;
    gen->context->resume_point = 0;
}

// Generator devam ettir
void generator_resume(Generator* gen) {
    if (!gen || !gen->context) return;
    
    if (gen->context->state == GEN_SUSPENDED) {
        gen->context->state = GEN_RUNNING;
    }
}

// Yield işlemi
void generator_yield(Generator* gen, void* value) {
    if (!gen || !gen->context) return;
    
    gen->context->state = GEN_SUSPENDED;
    gen->context->current_value = value;
    gen->context->resume_point++;
}

// Daha eleman var mı?
int generator_has_next(Generator* gen) {
    if (!gen || !gen->context) return 0;
    return gen->context->state != GEN_FINISHED;
}

// Bellek temizleme
void generator_free(Generator* gen) {
    if (!gen) return;
    free(gen->name);
    if (gen->context) gen_context_free(gen->context);
    free(gen);
}

void gen_context_free(GenContext* ctx) {
    if (!ctx) return;
    if (ctx->local_vars) free(ctx->local_vars);
    free(ctx);
}

void yield_free(Yield* yld) {
    if (!yld) return;
    free(yld);
}

void gen_call_free(GenCall* call) {
    if (!call) return;
    free(call);
}

void iterator_free(Iterator* iter) {
    if (!iter) return;
    free(iter);
}
